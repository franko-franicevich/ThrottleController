
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
const int ButtonMap[numRows][numCols] = {
  { 2, 3, 5, 5 },
  { 7, 8, 9, 10},
  { 6, 1, 0, 0 }  // This row only has two colums connected. The last two will never 'trigger', so don't need to be checked.
};

// QT Py board pin assignments. 
// Index in to the analogPins array to map the controller feature to a board pin. 
// eg, the throttle is on analogPins[THROTTLE_PIN] - that is, A0
// All these pins had 12-bit read resolution
#define THROTTLE_PIN = 0
#define RNG_PIN      = 1
#define ANT_PIN      = 2
#define MY_PIN       = 3
#define MX_PIN       = 4
const int analogPins[] = {A0, A1, A2, A3, A6}; // The QT Py board pins these pots are connected to. A4/5 are used for the ic2 bus.
const int numAnalogPins = 5;
const int readResolution = 12;  // default read resolution is 10 bits, but this samd21 board supports 12 bit.

// Our reads have a lot of noise. Thes two parameters can reduce it
// in different ways. Jitterbits is not the same as reducing read resolution:
// it impacts how likely we are to detect changes and notify the host OS.
// numReads is how many times we read each pin, and getting the average result of all the reads.
size_t   jitterBits = 2;   // number of bits we'll ignore for purpose of detecting changes. Testing shows that we have to go to  4 bits jitter to make a difference, so not worth it.
uint16_t numReads   = 4;   // Number of times we read each pin. We use multiple samples and average the results instead

// Board state.
bool lightOn = false;
uint16_t previousKeyState = 0u; // This is a bitmap containing state of buttons 1-10. Default to 'off'
uint16_t debounceState    = 0u; // if keystate changes, store it here. We don't issue changes UNTIL the debounce state from 1 ms ago matches current read keystate
uint16_t oldAnalogState[]  = {0, 0, 0, 0, 0};

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
  // init properly. It seems safe to 'begin' serial, just not to way. 
  // Either way, to be super safe, just leaving it all here
  Serial.begin(115200);
  while (!Serial);             // wait for serial monitor
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

  lastFPSCheck = millis(); // Initialise our 'last fps check' just prior to doing our first loop
  Serial.println("Finished setup, Beginning loop");
}

int loopCounter = 0;
bool bOn = false;

void loop() {
  if ( !usb_hid.ready() ) return;

  if (loopCounter++ > 200)
  {
    loopCounter = 0;
    if ( !usb_hid.ready() )
    {
      Serial.println("USB not yet ready, skipping.");
      return;
    }

    Serial.printf("Setting button %u", bOn); Serial.println();
    CougarHIDReport report;
    report.setButton(1, bOn);
    if (!usb_hid.sendReport(0, report.getReportData(), report.getReportSize()))
      Serial.println("Failed to send report.");
    bOn = !bOn;
  }

  bool changesDetected = false;

  // First read the analog pins on the QT Py board itself
  uint16_t newAnalogState[] = {0,0,0,0,0};
  // Read each input multiple times and calculate average to try reduce noise.
  // This makes very little difference to performance, as it's the matrix 
  // keyboard read on the i2c mcp below that makes up for 80% of our compute.
  for (int n = 0; n < numReads; n++)
    for (int i = 0; i < numAnalogPins; i++)
      newAnalogState[i] += analogRead(analogPins[i]);
  // numAnalogPins now contains totals for numReads reads: 
  // So divide each check by numReads to get the average.
  for (int i = 0; i < numAnalogPins; i++)
    newAnalogState[i] = newAnalogState[i]/numReads;

  // see if there were any changes.... we ignore least significant X bits for the comparison
  // to reduce jitter impacting our detection of too many changes.
  // Testing shows we can half the number of updates by ignoreing low 4 bits for 'changes'
  if (detectChangesWithJitter(oldAnalogState, newAnalogState, 5, jitterBits)) {
    changesDetected = true;
    memcpy(oldAnalogState, newAnalogState, sizeof(oldAnalogState));
  }

  
  // Next, read the button matrix on the MCP23008 extender
  // Keep newly read state in keyState which we will compare to previousKeyState later.
  // We read the state of all buttons in this loop.
  uint16_t keyState = 0u;  // What the state of the buttons are right now, as we read them.
  for (uint8_t row = 0; row < numRows; row++) {
    mcp.digitalWrite(rowPins[row], LOW);
    for (uint8_t col = 0; col < numCols; col++) {
      if (!mcp.digitalRead(colPins[col])) {
        bitWrite(keyState, row * numCols + col, 1);
      }
    }
    mcp.digitalWrite(rowPins[row], HIGH);
  }

  // Is the new state different from what's in the old state?
  // If it's different from OLD STATE, but the same as debounceState
  // then act on the changes
  // If it's difference and also different to debounce state, then
  // it's not stabilsed. Update debounce state till next loop

  if ( keyState != previousKeyState && keyState != debounceState)
  {
    // We've not stabilised. Store debounce, and move on.
    //dumpKeyState( "Changes detected, need to debounce.", previousKeyState, debounceState, keyState );
    debounceState = keyState;
  }
  else if ( keyState != previousKeyState && keyState == debounceState )
  {
    // we've stabilised, debounce successful, so now act on the changes.
    // dumpKeyState( "Changes detected, debounced, acting.", previousKeyState, debounceState, keyState );

    // Set the light as appropriate. We could check to see if it's already on, but
    // the code here will only act when the state changes, so setting it on or off won't happen often.
    if (keyState)
      mcp.digitalWrite(blinkPin, HIGH);
    else
      mcp.digitalWrite(blinkPin, LOW);    
    
    previousKeyState = keyState;
    changesDetected = true;
  }

  if (changesDetected)
  {
    updateCounter++;
    //dumpState();
    //Serial.println("");
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
    
  //delay(1); // wait a ms, then loop again. Gives us time to debounce.
  
}


/* The analog inputs or the controller seems to have a couple bits of read noise
 * where it will jitter in the low order bits. We perform the 'change detect'
 * comparison, but ignore the lowest ignoreBits number of bits
 * the comparison itself compares each matching element in each array in sequence.
 *
*/
bool detectChangesWithJitter( uint16_t oldVals[], uint16_t newVals[], size_t arraySize, size_t ignoreBits) {
  bool changes = false;
  for (int i = 0; i < arraySize; i++)
  {
    // Bitshift the old and new values to remove jitter, then compare them
    changes = changes || (( oldVals[i] >> ignoreBits) != (newVals[i] >> ignoreBits)); 
  }
  return changes;
}


// Dump the current state of the inputs to serial
void dumpState(){
  char s1[13];
  uint16_to_binary_string(previousKeyState, s1, 13);
  Serial.printf("%s   %4u %4u %4u %4u %4u      - (@ %3u fps with %3u updates/s)", s1, oldAnalogState[0] >> jitterBits, oldAnalogState[1] >> jitterBits, oldAnalogState[2] >> jitterBits, oldAnalogState[3] >> jitterBits, oldAnalogState[4] >> jitterBits, fps, updates);
}

/* 
 * Convert uint16 to a formatted binary string with 'X' representing 1,
 * and '.' representing 0. 
 * We use these rather than 1's and 0s to make it easier to spot the difference.
 * bufferLength should be enough to include the null terminator
 */
void uint16_to_binary_string( uint16_t value, char *buffer, uint8_t bufferLength)
{
  // We're going to start with the least significant bit, 
  // and write it at the end of the buffer, then count down.
  int len = min(16, bufferLength-1);
  buffer[len] = 0; // Ensure null termination.
  for (int i = 0; i < len; i++) {
    if (value & (1 << i)) {
      buffer[len - i - 1] = 'X';
    } else {
      buffer[len - i - 1] = '.';
    }
  }
}
