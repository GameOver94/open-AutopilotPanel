#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_MCP23017.h>
#include <Rotary.h>
#include <Joystick.h>

// Pin Mapping --------------------------------------------------------------------------------
#define ALT 8
#define AP 7
#define APR 4
#define FD 2
#define FLC 11
#define HDG 0
#define LVL 6
#define NAV 5
#define TRK 1
#define VN 10
#define VS 9
#define YD 3

#define rotHDGButton 6 // Pushbutton
#define rotHDG_A 4     // Rotary A
#define rotHDG_B 5     // Rotary B

#define rotALTButton 9 // Pushbutton
#define rotALT_A 8     // Rotary A
#define rotALT_B 7     // Rotary B

#define rotVS_A 16 // Rotary A
#define rotVS_B 10 // Rotary B

// Function prototypes ------------------------------------------------------------------------
void pollButton();
void pollRotary();
void pollRotaryButton();
void updateButton(boolean buttonState[15]);
void release();

// Setup the joystick library -----------------------------------------------------------------
/* Create the Joystick */
Joystick_ Joystick;

/* Joystock Button Mapping
rotary HDG right      1
rotary HDG left       2
rotary HDG button     3
rotary ALT right      4
rotary ALT left       5
rotary ALT button     6
rotary VS up          7
rotary VS down        8

button ALT            17
button AP             16
button APR            13
button FD             11
button FLC            20
button HDG            9
button LVL            15
button NAV            14
button TRK            10
button VN             19
button VS             18
button YD             12
*/

// Setup the MCP23017 GPIO expander -----------------------------------------------------------
Adafruit_MCP23017 mcp;

// Setup rotary encoder------------------------------------------------------------------------
Rotary rotHDG = Rotary(rotHDG_A, rotHDG_B);
Rotary rotALT = Rotary(rotALT_A, rotALT_B);
Rotary rotVS = Rotary(rotVS_A, rotVS_B);

// Global variables
const uint8_t numRotary = 9;            // The Joystick library defines 32 buttons by default
unsigned long releaseTime[numRotary];    // Stores the release time for the buttons (rotary encoder).
bool rotaryPressed[numRotary] = {false}; // Stores the button state for the rotary enceder (gets automaticly reset)
bool buttonPressed[numRotary] = {false}; // Stores the button state for other Buttons (don't get automaticly reset)
const int holdTime = 50;                  // time the a Button is held down in ms (rotaty encoder)
uint16_t lastButtonState = 0;             // Stores the button state for the MCP23017 Port Expander

void setup()
{
  Serial.begin(115200);
  Serial.println("open AutopilotPanel v1.x");

  // Initialize Joystick Library
  Joystick.begin();

  // Initialize the MCP23017
  mcp.begin(); // use default address 0

  for (int i = 0; i <= 15; i++)
  {
    mcp.pinMode(i, INPUT); // set pin as input
    mcp.pullUp(i, HIGH);   // turn on a 100K pullup internally
  }

  // Initialize the Rotary Encoder
  rotHDG.begin(true);
  rotALT.begin(true);
  rotVS.begin(true);

  // Setup aditional pins
  pinMode(rotHDGButton, INPUT_PULLUP);
  pinMode(rotALTButton, INPUT_PULLUP);
}

void loop()
{
  pollButton();
  pollRotary();
  pollRotaryButton();

  release();
}

// Polling the MCPs to ceck for changes
void pollButton()
{
  //We could also read each pin seperatly, however that will cause an I2C transfern for pin.
  //It's faster to read the whole register, and decode the register.
  uint16_t newButtonState = mcp.readGPIOAB();

  if (lastButtonState != newButtonState)
  {

    boolean buttonState[16];
    lastButtonState = newButtonState;

    for (uint8_t i = 0; i < 16; i++) // binary masking startig at the least significant bit
    {
      buttonState[i] = newButtonState & (1 << (i)) ? 1 : 0;
    }

    updateButton(buttonState);
  }
}

// Update the Joystick with the input from the Buttons connectet to the MCP
void updateButton(boolean buttonState[15])
{
  for (uint8_t i = 0; i < 12; i++)
  {
    Joystick.setButton(i + 8, !buttonState[i]);
  }
}

// Evaluates the rotary encoders and updates the Joystick
void pollRotary()
{
  unsigned char resultHDG = rotHDG.process();
  if (resultHDG)
  {
    if (!rotaryPressed[0 + resultHDG / 16 - 1]) // rejeckts input when button wasn't releast before
    {
      Joystick.setButton(0 + resultHDG / 16 - 1, 1);
      releaseTime[0 + resultHDG / 16 - 1] = millis() + holdTime;
      rotaryPressed[0 + resultHDG / 16 - 1] = true;
    }
  }

  unsigned char resultALT = rotALT.process();
  if (resultALT)
  {
    if (!rotaryPressed[3 + resultALT / 16 - 1]) // rejeckts input when button wasn't releast before
    {
      Joystick.setButton(3 + resultALT / 16 - 1, 1);
      releaseTime[3 + resultALT / 16 - 1] = millis() + holdTime;
      rotaryPressed[3 + resultALT / 16 - 1] = true;
    }
  }

  unsigned char resultVS = rotVS.process();
  if (resultVS)
  {
    if (!rotaryPressed[6 + resultVS / 16 - 1]) // rejeckts input when button wasn't releast before
    {
      Joystick.setButton(6 + resultVS / 16 - 1, 1);
      releaseTime[6 + resultVS / 16 - 1] = millis() + holdTime;
      rotaryPressed[6 + resultVS / 16 - 1] = true;
    }
  }
}

// Updates the Joystick with the Button directly connectet to the Arduino
void pollRotaryButton()
{
  boolean valHDG = !digitalRead(rotHDGButton);
  if (buttonPressed[2] != valHDG)
  {
    Joystick.setButton(2, valHDG);
    buttonPressed[2] = valHDG;
  }

  boolean valALT = !digitalRead(rotALTButton);
  if (buttonPressed[5] != valALT)
  {
    Joystick.setButton(5, valALT);
    buttonPressed[5] = valALT;
  }
}

// Release buttons of the Joystick triggert bei the Rotary Encoder
void release()
{

  for (int i = 0; i < numRotary; i++)
  {
    if (releaseTime[i] < millis())
    {
      Joystick.setButton(i, 0);
      releaseTime[i] = millis() + holdTime;
      if (rotaryPressed[i])
      {
        rotaryPressed[i] = false;
      }
    };
  };
}