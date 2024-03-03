
#include <Wire.h>
#include <Adafruit_MCP23X08.h>
#include <Adafruit_TinyUSB.h>
#include "Cougar.h"

Adafruit_MCP23X08 mcp;
CougarHIDReport cougar;

/* 
 * The thrustmaster cougar throttle uses a mostly standard matrix for the
 * buttons, apart from the fact that there are only 10 buttons - So
 * one of the rows has only two columns populated. 
 * https://pit.uriba.org/uriba/standalone-cougar-tqs-part-i/
 * https://pit.uriba.org/uriba/standalone-cougar-tqs-part-ii/
 * We're connecting the matrix to an MCP23008 chip on i2c as
 * the QT PY samd21 board we're using doesn't have enough GPIO pins
 * for both the 5 analog inputs and the button matrix
*/
// MCP23008 IO expander pin assignments
const int blinkPin = 0;                 // LED that will light up for some user feedback, such as button press.
const int numRows = 3;
const int numCols = 4;
const int rowPins[numRows] = {5,6,7};   // The mcp23008 pins for the ROWS in our button matrix
const int colPins[numCols] = {1,2,3,4}; // the mcp23008 pins for the COLUMNS in our bytton matrix

// For ROW x, COL y, maps to the cougar button maps ID:
// Note that this is 1 indexed, not zero indexed like the
// rest of the code, as 0 here means 'no matching button'
const uint8_t ButtonMap[numRows][numCols] = {
  { 6, 1, 0, 0 },  // This row only has two colums connected. The last two will never 'trigger', so don't need to be checked.
  { 7, 8, 9, 10},
  { 2, 3, 4, 5 }
};

// QT Py board pin assignments for each control
#define THROTTLE_PIN A0
#define RNG_PIN      A1
#define ANT_PIN      A2
#define MY_PIN       A3
#define MX_PIN       A6 // A4/5 are used for the ic2 bus.
const int readResolution = 12;  // default read resolution is 10 bits, this samd21 board supports 12 bit. We only need 8 bit though/ Read at 10, then smooth, then downcast to 8 bit.

/* READ NOISE
 Our reads have a lot of noise. We have three techniques for reducing the noise:
1. Perform multiple reads for each sample.
2. Use a smoothing library on the final sample with an exponential sampling
3. Ignore a new read if it's delta is smaller than a threshold.
Some of these settings are handled in the Cougar.h class.
*/
uint16_t numReads   = 3;   // Number of times we read each pin. We use multiple samples and average the results instead

// Board state.
//bool lightOn = false;
//uint16_t debounceState    = 0u; // if keystate changes, store it here. We don't issue changes UNTIL the debounce state from 1 ms ago matches current read keystate

// every second or so, we calculate what our FPS is for performance monitoring
unsigned long lastFPSCheck     = 0;     // When did we last check and write performance stats? (in milliseconds since board start)
unsigned long fpsCheckInterval = 1000;  // Interval between calculating and dumping stats in milliseconds. Byu default, we'll check and dump stats every second.
unsigned int  fpsCounter       = 0;     // The number of frames since the lastFPSCheck
unsigned int  fps              = 0;     // fps calculated during the previous interval
unsigned int  updateCounter    = 0;
unsigned int  updates          = 0;     // number of updates we're issuing per second. Can be different from the fps, which is number of checks

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid(cougar_report_desc, sizeof(cougar_report_desc), HID_ITF_PROTOCOL_NONE, 2, false);

void setup() {
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
 #endif

  // Notes: following commented-out functions has no affect on ESP32
  // usb_hid.setPollInterval(2);
  // usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  bool usbInit = usb_hid.begin();
  // wait until device mounted
  while( !TinyUSBDevice.mounted() ) delay(1);

  // Waiting for serial to initialise with the while() causes USB to fail to 
  // init properly. It seems safe to 'begin' serial, just not to wait. 
  // Either way, to be super safe, just leaving it all here
  Serial.begin(115200);
  //while (!Serial);             // wait for serial monitor. On windows, this causes device to fail to init, so commented out.
  Serial.println("USB initialised, serial started, continuing setup.");
  if (!usbInit)
    Serial.println("USB HID failed to start correctly");

  
  // Connect to the MCP23008 via i2c
  Wire.begin(20); // Default address is 20 for the MCP23008
  if (mcp.begin_I2C())
    Serial.println("Initialised MCP successfully");
  else
    Serial.println("Error initialising MCP");

  // Configure pins on MCP
  mcp.pinMode(blinkPin, OUTPUT);  // This is the light. Set HIGH to switch on
  
  // Set the ROWS as OUTPUT, HIGH.  
  for (int r = 0; r < numRows; r++){
    mcp.pinMode(rowPins[r], OUTPUT);
    mcp.digitalWrite(rowPins[r], HIGH);
  }

  // Set the COLS for INPUT. 
  for (int c = 0; c < numCols; c++)
    mcp.pinMode(colPins[c], INPUT); // no need for INPUT_PULLUP, the throttle already has a pullup resistor in the circuit.

  analogReadResolution(readResolution);
  cougar.setReadResolution(readResolution); // So the cougar knows how to downsample if we're reading > 8

  lastFPSCheck = millis(); // Initialise our 'last fps check' just prior to doing our first loop
  Serial.println("Finished setup, Beginning loop");
}

uint16_t readAxis(uint16_t pin)
{
  // Read each input multiple times and calculate average to try reduce noise.
  // This makes very little difference to performance, as it's the matrix 
  // keyboard read on the i2c mcp below that makes up for 80% of our compute.
  uint16_t newState = 0;
  for (int n = 0; n < numReads; n++)
      newState += analogRead(pin);
  return newState/numReads;
}

void loop() {
  if ( !usb_hid.ready() ) return;

  // numAnalogPins now contains totals for numReads reads: 
  // So divide each check by numReads to get the average.
  cougar.setThrottle(readAxis(THROTTLE_PIN)); // throttle is inverted, but it seems like games read it like that.
  cougar.setRng(readAxis(RNG_PIN));
  cougar.setAnt(readAxis(ANT_PIN));
  cougar.setMx(readAxis(MX_PIN));
  cougar.setMy(readAxis(MY_PIN));   
  
  // Next, read the button matrix on the MCP23008 extender
  // Keep newly read state in keyState which we will compare to previousKeyState later.
  // We read the state of all buttons in this loop.
  uint16_t keyState = 0u;  // What the state of the buttons are right now, as we read them.
  for (uint8_t row = 0; row < numRows; row++) {
    mcp.digitalWrite(rowPins[row], LOW);
    for (uint8_t col = 0; col < numCols; col++) {
      // Not all matrix postions have a valid button on the Cougar!
      if (ButtonMap[row][col])
      {
        uint8_t buttonIndex = ButtonMap[row][col] - 1;
        bool value = !mcp.digitalRead(colPins[col]); // HIGH means not pressed, LOW means pressed.
        cougar.setButton(buttonIndex, value);
      }
    }
    mcp.digitalWrite(rowPins[row], HIGH);
  }

  if (cougar.hasChanged())
  {
    // If there are button state changes, then make sure the LED light
    // reflects our button state: we light up if a button is on.
    if (cougar.detectButtonChanges())
    {
      if (cougar.isAnyButtonOn())
        mcp.digitalWrite(blinkPin, HIGH);
      else
        mcp.digitalWrite(blinkPin, LOW);
    }

    updateCounter++;
    //cougar.dumpState();
    //Serial.printf("      - (@ %3u fps with %3u updates/s) %2u report size", fps, updates, cougar.getReportSize());
    //Serial.println("");
    if (!usb_hid.sendReport(0, cougar.getReportData(), cougar.getReportSize()))
      Serial.println("Failed to send report.");

    cougar.changesSent();
  }

  // Lastly, for debug purposes, lets check if it's time to
  // calculate the number of frames/updates per second we're processing.
  fpsCounter++;   // Increment our count of loops since last time.
  unsigned long currentMS = millis();
  if ( currentMS - lastFPSCheck >= fpsCheckInterval)
  {
    fps = fpsCounter * (1000.0f/(currentMS - lastFPSCheck));  // do the calc in case there's loop overhead, or we change interval away from 1 second.
    fpsCounter = 0;

    updates = updateCounter * (1000.0f/(currentMS - lastFPSCheck));
    updateCounter = 0;

    lastFPSCheck = currentMS;
  }
 
}




