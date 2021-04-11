/*******************************************************************
*  Will Johnson and Steve Johnson
*
* A simple program to visually show the user the different number
* representations from a binary value to hex, octal, and decimal
* 
* This program is being written for the ESP32S. Make sure you pick the correct board in the tools menu
* 
*******************************************************************/
//TODO: Don't have I2C LED displays yet. Will need to determine which libraries this display needs
//TODO: Detemine with I2C address this display uses. There have been reports that it can be 0x27 or 0x3F
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define SW_0 ? //TODO: Pick a pin on the ESP32
#define SW_1 ? //TODO: Pick a pin on the ESP32
#define SW_2 ? //TODO: Pick a pin on the ESP32
#define SW_3 ? //TODO: Pick a pin on the ESP32
#define SW_4 ? //TODO: Pick a pin on the ESP32
#define SW_5 ? //TODO: Pick a pin on the ESP32
#define SW_6 ? //TODO: Pick a pin on the ESP32
#define SW_7 ? //TODO: Pick a pin on the ESP32

byte swValue = 0; // This is the variable that holds all the switch positions

void setup() {
    Serial.begin(115200);

    // Init GPIO Inputs coming from Switches
    //TODO:
    
    // Look for I2C devices
    Serial.println("Scan for devices on I2C bus");
    I2Cscan();

    // TODO: Init LCD Display
}

void loop() {

    byte currentValue = 0;
    // Read Switch Values and shift them into our currentValue buffer variable
    //TODO:

    // Check if the values have changed
    if (currentValue != swValue) {
        swValue = currentValue;
        updateDisplay(swValue);
    }
}

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

    //TODO: take the value input and turn it into a display string using the following format:

}
