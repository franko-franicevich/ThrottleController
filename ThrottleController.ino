#include <Wire.h>
#include <Adafruit_MCP23X08.h>

Adafruit_MCP23X08 mcp;

/* 
 * The thrustmaster cougar throttle uses a mostly standard matrix for the
 * buttons, apart from the fact that there are only 10 buttons - So
 * one of the rows has only two columns populated. 
 * https://pit.uriba.org/uriba/standalone-cougar-tqs-part-i/
 * We're connecting the matrix to an MCP23008 chip on i2c as
 * the QT PY samd21 board we're using doesn't have enough GPIO pins
 * for both the 5 analog inputs and the button matrix
*/
const int blinkPin = 0;                // We're throwing in a light during testing to light up when buttons pressed
const int numRows = 3;
const int numCols = 4;
const int rowPins[numRows] = {5,6,7}; // {1,2,3}; //        // The mcp23008 pins for the ROWS in our button matrix
const int colPins[numCols] = {1,2,3,4}; //{4,5,6,7};  // the mcp23008 pins for the COLUMNS in our bytton matrix

// For ROW x, COL y, maps to the cougar button maps ID:
const int ButtonMap[numRows][numCols] = {
  { 2, 3, 5, 5 },
  { 7, 8, 9, 10},
  { 6, 1, 0, 0 }  // This row only has two colums connected. The last two will never 'trigger', so don't need to be checked.
};

bool lightOn = false;
uint16_t previousKeyState = 0u; // This is a bitmap containing state of buttons 1-10. Default to 'off'
uint16_t debounceState    = 0u; // if keystate changes, store it here. We don't issue changes UNTIL the debounce state from 1 ms ago matches current read keystate

uint16_t oldAnalogRead = 0;

void setup() {

  Serial.begin(9600);
  while (!Serial);             // wait for serial monitor
  Serial.println("Beginning setup.");
  
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
    
  Serial.println("Finished setup, Beginning loop");

}


void loop() {
  /*
  int newAnalogRead = analogRead(A0);
  if (newAnalogRead  != oldAnalogRead)
  {
    Serial.println(newAnalogRead);
    oldAnalogRead=newAnalogRead;
  }
  */
  
  // Read the button matrix, keeping newly read state in keyState
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
    dumpKeyState( "Changes detected, debounced, acting.", previousKeyState, debounceState, keyState );

    // Set the light as appropriate. We could check to see if it's already on, but
    // the code here will only act when the state changes, so setting it on or off won't happen often.
    if (keyState)
      mcp.digitalWrite(blinkPin, HIGH);
    else
      mcp.digitalWrite(blinkPin, LOW);    
    
    previousKeyState = keyState;
  }
    
  delay(1); // wait a ms, then loop again. Gives us time to debounce.
  
}


// Write a log line with the 3 keystates formatted as binary
void dumpKeyState( const char* message, uint16_t b1, uint16_t b2, uint16_t b3 ) {
  char s1[13], s2[13], s3[13];
  uint16_to_binary_string(b1, s1, 13);
  uint16_to_binary_string(b2, s2, 13);
  uint16_to_binary_string(b3, s3, 13);

  //Serial.printf("%s %s %s - %s", s1, s2, s3, message);
  Serial.printf("%s - %s", s3, message);
  Serial.println("");
}

/*

// Write a log line with the 3 keystates formatted as binary
void dumpKeyState( const char* message, uint16_t b1, uint16_t b2, uint16_t b3 ) {
  Serial.print(b1, BIN); Serial.print(" ");
  Serial.print(b2, BIN); Serial.print(" ");
  Serial.print(b3, BIN); Serial.print(" ");
  Serial.println(message);
  Serial.println("");
}
 */

// convert uint16 to a formatted binary string. bufferLength should be enough to include the null terminator
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
