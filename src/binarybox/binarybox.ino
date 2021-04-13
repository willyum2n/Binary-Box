/*********************************************************************************************************
* Authors: Will Johnson and Steve Johnson
* Platform: ESP32
* 
* A simple program to visually show the user the different number
* representations from a binary value to hex, octal, and decimal
* 
* This program is being written for the ESP32S. Make sure you pick the correct board in the tools menu
* 
* This code is part of a class, so the code here will be commented heavily
* as a way for students to understand what is going on
* 
* To keep this program simple, no switch debouncing will be added to the code. 
* For this application debouncing isn't necessary as we are working in the 
* human time domain. ~25ms of inaccuracy won't matter because user wont' see it :)
**********************************************************************************************************/

//--------- LCD Display -----------------------------
#include <Wire.h>               /**< Gives us access to the I2C bus */
#include <LiquidCrystal_I2C.h>  /**< Gives us access to the LCD object we need for writing to the display */

/** I have read online that this LCD display has been reported to be 1 of 2 addresses: 0x27 or 0x3F
 * As of 2021-04-13, the LCD display used 0x27 on the I2C bus
 * The LCD Display object that we will use to write out our strings so we see them on the display
 */
LiquidCrystal_I2C lcd(0x27, 16, 2);   /**< set the LCD address to 0x27 for a 16 chars on a line with 2 lines */

/*--------- Binary Switches -----------------------------
 * You will find a picture for the ESP32S in the "datasheet" section of this repo. It will show you the 
 * GPIO numbers for this processor and where they appear physically on the MCU circuit board. Instead of using
 * the GPIO numbers directly in our code, we are going to define a name for each pin so we don't have to remember
 * what "Pin 13" means. We'll instead be able to think in terms of the switches.
 * There are 8 physical switches in a row that represent the different bits in a full byte
 * with the far right switch being the "1's" switch and the far left being the "128's" place
 * All 8 switches have to be wired to our Microcontroller (MCU), so here we are defining the pins that
 * we will be wiring the switches to
 */
#define SW_0 13 // 1's Place
#define SW_1 12 // 2's Place
#define SW_2 14 // 4's Place
#define SW_3 27 // 8's Place
#define SW_4 26 // 16's Place
#define SW_5 25 // 32's Place
#define SW_6 33 // 64's Place
#define SW_7 32 // 128's Place

// We are going to need to keep track of "value" that all 8 switches are making over time. So we
// will store the value of the switches into a variable
byte swValue = 0; // This is the variable that holds all the switch positions between loop iterations

/**
 * This is a helper function that can help us debug any issues with communications on the I2C communications bus.
 * When called, this function will scan every address on the entire bus for devices. If a device exists at the
 * address, it will reply and this function will display which addresses replied. If you have a wiring mistake,
 * this function will return "No I2C devices found". Then you'll know that there is a problem
 */
void I2Cscan(TwoWire *wire = &Wire) {
    // scan for i2c devices
    byte error, address;
    int nDevices;

    Serial.print("Scanning for I2C devices (pointer=...");
    Serial.print((long)wire);
    
    nDevices = 0;
    for(address = 1; address < 127; address++ ) {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        wire->beginTransmission(address);
        error = wire->endTransmission();

        if (error == 0) {
            Serial.print("     - I2C device found at address 0x");
            if (address<16) 
                Serial.print("0");
            Serial.print(address,HEX);
            Serial.println("  !");

            nDevices++;
        } else if (error==4) {
            Serial.print("     - Unknown error at address 0x");
            if (address<16)
                Serial.print("0");
    
            Serial.println(address,HEX);
        }    
    }
    if (nDevices == 0)
        Serial.println("     - No I2C devices found\n");
    else
        Serial.println("     - done\n");

}

/**
 * The function name "btos" is shorthand for "byte to string". There are no helper functions for displaying a byte
 * to a string, so we've made our own.
 */
String btos(byte var) {
    String buffer = "";   /*< Create an empty String to contain our characters */
  
    // Loop thru each bit in the byte starting at the most significant bit using
    // a bit mask. For each iteration after, we'll look at the next bit by shifting (">>") our
    // bit mask one to the right. We can then "AND" this with our "var" to see if we have a 1 or 0
    // Here is a list of the iterations this for loop will do:
    //     test = 0x80 -->  1000 0000 In binary
    //     test = 0x40 -->  0100 0000 In binary
    //     test = 0x20 -->  0010 0000 In binary
    //     test = 0x10 -->  0001 0000 In binary
    //     test = 0x08 -->  0000 1000 In binary
    //     test = 0x04 -->  0000 0100 In binary
    //     test = 0x02 -->  0000 0010 In binary
    //     test = 0x01 -->  0000 0001 In binary
    // You can see how the value keeps getting "shifted" to the right each time we loop
    for (byte test = 0x80; test; test >>= 1) {
        // "AND" our current bitmask "test" with "var". 
        // If the value is zero, then this bit is not set
        // If the value is non-zero, then this bit is set
        // Append the correct character to the string
        buffer += (var  & test ? '1' : '0');
    }
    return buffer;
}

void updateDisplay(byte value) {
  
  /* LCD Display Graphical space
   * 0123456789012345     <--- The Column numbers
   * ----------------
   * 255  0b377  0xff     <-- We want to print the Decimal number followed by the octal number followed by the hexidecimal number
   * binary: 11111111     <-- Show the binary value controlled by the hardware switches
   */

    /** Character Array to hold our string value
     *  Notice how this is 9 characters instead of 8. Don't forget that strings need 
     *  to be terminated by a null character ('\0') so that we know where the string ends.
     *  So we have to make room for our 8 characters on a line with the null terminator
     */ 
    char buff[9];   

    lcd.setCursor(0, 0);    // set the cursor to column 0, line 0
    lcd.print(value, DEC);  // No special handling needs to be done with the decimal value. Just print it to the display in DEC format

    lcd.setCursor(5,0);             // set the cursor to column 5, line 0
    sprintf(buff, "0b%03o", value); // We want our octal value to always be 3 characters long with a "0b" in front
    lcd.print(buff);

    lcd.setCursor(12,0);            // set the cursor to column 12, line 0
    sprintf(buff, "0x%02x", value); // We want our hexidecimal value to always be 2 characters long with a "0x" in front. If you want uppercase character instead of lowercase, use "0x%02X" instead
    lcd.print(buff);

    lcd.setCursor(0,1);                   // set the cursor to column 0, line 1
    lcd.print("binary: " + btos(value));  // Show the state of the buttons which are binary
}


void setup() {
    Serial.begin(115200);   // Configure our serial port so we can send log messages back to the computer

    /** Init GPIO Inputs coming from Switches - This is a polling app (No interrupts needed)
     * One side of each switch is wired to ground, and the other side of the switch is wired to
     * a GPIO pin. Thes means that we are "Active Low". 
     * When we have a 0 (Low or Ground), we are "ON"
     * When we have a 1 (High or Vcc [which on a ESP32S is 3.3 Volts]), we are "OFF"
     * So you can see the logic is reversed. 
     * When the switch is "Open", the GPIO pin will be floating (Not connected to either GND or VCC)
     * But when the switch is "Closed" it will connect GND to our GPIO pin. This means the value at our
     * GPIO (Which we are reading to get the value) is either GND or Floating. Well, floating is really
     * really bad, because we have no real value which means the pin can "float" between GND and VCC
     * depending on what electrical noise is around us. This means the GPIO pin is indeterminate. We
     * can't know what it's value is and that is not useful. To solve this problem we can "Pull Up" or
     * "Pull down" a GPIO pin. When you "Pull Up" a pin, you are saying "When this pin is
     * floating, I want it to be UP (Or connected to Vcc)". That way when the pin isn't physically
     * connected to anything, the value will be known.
     * 
     * Some processors have Pull Up and/or Pull Down resistors built right into the pin. This is great
     * because you don't have to wire in your own resistor to get the UP or DOWN, you can just configure
     * the pin. Most MCU's have PULL UP resistors, but not PULL DOWN. While the ESP32S does have PULLDOWN
     * resistors, we are going to practice with PULLUP because that is normally what you'll see with other
     * MCUs
     */
    
    pinMode(SW_0, INPUT_PULLUP);
    pinMode(SW_1, INPUT_PULLUP);
    pinMode(SW_2, INPUT_PULLUP);
    pinMode(SW_3, INPUT_PULLUP);
    pinMode(SW_4, INPUT_PULLUP);
    pinMode(SW_5, INPUT_PULLUP);
    pinMode(SW_6, INPUT_PULLUP);
    pinMode(SW_7, INPUT_PULLUP);
    
    // Look for I2C devices and print any we find to the log
    Serial.println("Scan for devices on I2C bus");
    I2Cscan();

    // Init LCD Display
    lcd.init();       // initialize the lcd
    lcd.backlight();  // turn on the backlight

    // Force the Display to display a value of 0. This is needed because if the current LOGICAL
    // value of the switches is 0, then, until a switch is toggled, the display will be blank.
    // To ensure this doesn't happen, we will just make the display show a 0 value on startup
    // We don't have to worry if the value of the switches is not actually 0, because it will
    // update to the correct value on the first iteration of the loop() function
    updateDisplay(0);
}

long logTime = 0; /*< used to print a log message every second that shows the current value of the switches */
void loop() {
    
    byte currentValue = 0;  /*< Variable used to build up the current values of the switches */
    
    // Each switch represents a bit's worth of info in the byte. We need to get each switch's 
    // value into our "currentValue" variable on bit at a time
    // DON'T FORGET, WE ARE ACTIVE LOW!!!
    // When we "digitalRead" a pin, we are going to get a 1 or a 0 back
    // Whatever the digital value of the switch is, we need to reverse the logic BEFORE we 
    // insert it into our "currentValue" variable. The "!" is a "not" operator. It reverses the logic.
    // Next we pull out the bit we are interested in by bitwise AND'ing at the right bit
    // Next we combine the interesting bit into our current value
    // 
    // Consider the DIGITAL value from all the switches: 0010 0101
    // "Not" these values to get the LOGICAL value:      1101 1010
    // We are interested in the LOGICAL value of the switches because this is the true numeric value (Reversed because switches are ACTIVE LOW instead of ACTIVE HIGH)
    // We will shift each bit value from each switch into the correct place in our currentValue byte to build our number

    currentValue = (!digitalRead(SW_0) << 0) | currentValue;    // This can be reduced since shifting a value 0 spaces is just the value.
    currentValue = (!digitalRead(SW_1) << 1) | currentValue;
    currentValue = (!digitalRead(SW_2) << 2) | currentValue;
    currentValue = (!digitalRead(SW_3) << 3) | currentValue;
    currentValue = (!digitalRead(SW_4) << 4) | currentValue;
    currentValue = (!digitalRead(SW_5) << 5) | currentValue;
    currentValue = (!digitalRead(SW_6) << 6) | currentValue;
    currentValue = (!digitalRead(SW_7) << 7) | currentValue;
    
    // Only update the Display if the value has changed
    // Check if the values have changed
    if (currentValue != swValue) {
        // The switches have been changed! Update the display
        swValue = currentValue;
        updateDisplay(swValue);
    }

    // We only want to print the current values of the switches to the log once per second
    if (logTime < millis()) {
        // Read Switch Values and shift them into our currentValue buffer variable
        Serial.print("Switch Digital Values=");
        Serial.print(digitalRead(SW_7));
        Serial.print(digitalRead(SW_6));
        Serial.print(digitalRead(SW_5));
        Serial.print(digitalRead(SW_4));
        Serial.print(digitalRead(SW_3));
        Serial.print(digitalRead(SW_2));
        Serial.print(digitalRead(SW_1));
        Serial.print(digitalRead(SW_0));
        Serial.print(",  Current Logical binary Value=" + String(btos(currentValue)));
        Serial.println(",  Current Logical decimal Value=" + String(currentValue));
        logTime = millis() + 1000;    // Set our next log time 1000 milliseconds from now
    }

}
