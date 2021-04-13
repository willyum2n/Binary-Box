/*******************************************************************
*  Will Johnson and Steve Johnson
*
* A simple program to visually show the user the different number
* representations from a binary value to hex, octal, and decimal
* 
* This program is being written for the ESP32S. Make sure you pick the correct board in the tools menu
* 
*******************************************************************/

//--------- LCD Display -----------------------------
//TODO: Detemine with I2C address this display uses. There have been reports that it can be 0x27 or 0x3F
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

//--------- Binary Switches -----------------------------
#define SW_0 13 // 1's Place
#define SW_1 12 // 2's Place
#define SW_2 14 // 4's Place
#define SW_3 27 // 8's Place
#define SW_4 26 // 16's Place
#define SW_5 25 // 32's Place
#define SW_6 33 // 64's Place
#define SW_7 32 // 128's Place

byte swValue = 4; // This is the variable that holds all the switch positions between loop iterations

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

String btos(byte var) {
  String buffer = "";
  for (byte test = 0x80; test; test >>= 1) {
    buffer += (var  & test ? '1' : '0');
  }
  return buffer;
}

void updateDisplay(byte value) {
  
  /* LCD Display Graphical space - Here are two different ways to display the value on the LCD
   * 1234567890123456
   * ----------------
   * dec: 255 hex: ff    
   * binary: 11111111
   * 
   * 1234567890123456
   * ----------------
   * 255  0b377  0xff    
   * binary: 11111111  
   */

    char buff[9];

    lcd.setCursor(0, 0); // set the cursor to column 0, line 0
    lcd.print(value, DEC);

    lcd.setCursor(5,0);
    sprintf(buff, "0b%03o", value);
    lcd.print(buff);

    lcd.setCursor(12,0);
    sprintf(buff, "0x%02x", value);
    lcd.print(buff);

    lcd.setCursor(0,1);
    lcd.print("binary: " + btos(value));        
}


void setup() {
    Serial.begin(115200);

    // Init GPIO Inputs coming from Switches - This is a polling app (No interrupts needed)
    pinMode(SW_0, INPUT_PULLUP);
    pinMode(SW_1, INPUT_PULLUP);
    pinMode(SW_2, INPUT_PULLUP);
    pinMode(SW_3, INPUT_PULLUP);
    pinMode(SW_4, INPUT_PULLUP);
    pinMode(SW_5, INPUT_PULLUP);
    pinMode(SW_6, INPUT_PULLUP);
    pinMode(SW_7, INPUT_PULLUP);
    
    // Look for I2C devices
    Serial.println("Scan for devices on I2C bus");
    I2Cscan();

    // Init LCD Display
    lcd.init();  //initialize the lcd
    lcd.backlight();  //open the backlight
}

void loop() {

    byte currentValue = 0;
    // Read Switch Values and shift them into our currentValue buffer variable
    Serial.print(digitalRead(SW_7));
    Serial.print(digitalRead(SW_6));
    Serial.print(digitalRead(SW_5));
    Serial.print(digitalRead(SW_4));
    Serial.print(digitalRead(SW_3));
    Serial.print(digitalRead(SW_2));
    Serial.print(digitalRead(SW_1));
    Serial.println(digitalRead(SW_0));
    
    currentValue = (!digitalRead(SW_0) & 0x01) || currentValue;
    currentValue = (!digitalRead(SW_1) & 0x02) || currentValue;
    currentValue = (!digitalRead(SW_2) & 0x04) || currentValue;
    currentValue = (!digitalRead(SW_3) & 0x08) || currentValue;
    currentValue = (!digitalRead(SW_4) & 0x10) || currentValue;
    currentValue = (!digitalRead(SW_5) & 0x20) || currentValue;
    currentValue = (!digitalRead(SW_6) & 0x40) || currentValue;
    currentValue = (!digitalRead(SW_7) & 0x80) || currentValue;

    Serial.print("currentValue=");
    Serial.println(currentValue);
    
    // Check if the values have changed
    if (currentValue != swValue) {
        swValue = currentValue;
        updateDisplay(swValue);
    }

}
