#include <Wire.h>
#include <Adafruit_MCP23X08.h>

Adafruit_MCP23X08 mcp;

#define BLINKPIN 7
const int BUTTONS[] = {4, 5, 6};

bool lightOn = false;

void setup() {
  // put your setup code here, to run once:

  Wire.begin(20);

  Serial.begin(9600);
  while (!Serial);             // wait for serial monitor
  Serial.println("\nI2C Scanner Starting Up");
  #ifdef WIRE_INTERFACES_COUNT
    Serial.printf("Number of I2C interfaces %d\n", WIRE_INTERFACES_COUNT);
  #elif
    Serial.println("Only one I2C interface");
  #endif

  if (mcp.begin_I2C())
    Serial.println("Initialised MCP successfully");
  else
    Serial.println("Error initialising MCP");
  
  mcp.pinMode(BLINKPIN, OUTPUT);
  mcp.digitalWrite(BLINKPIN, HIGH);
  delay(1000);
  mcp.digitalWrite(BLINKPIN, LOW);
  Serial.println("Beginning loop");

  for (int c = 0; c < 3; c++){
    mcp.pinMode(BUTTONS[c], INPUT_PULLUP); // Try INPUT_PULLUP)
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  int anyButtonPressed = false;

  for (int c = 0; c < 3; c++) {
    if (!mcp.digitalRead(BUTTONS[c])) {
      Serial.printf("Button [%d] on pin [%d] pressed\n", c, BUTTONS[c]);
      anyButtonPressed = true;
    }
  }

  if (anyButtonPressed != lightOn)
  {
    lightOn = anyButtonPressed;
    // State has changed...
    if (lightOn)
      mcp.digitalWrite(BLINKPIN, HIGH);
    else
      mcp.digitalWrite(BLINKPIN, LOW);
  }
  
}
