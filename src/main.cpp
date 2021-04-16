#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_MCP23017.h>
#include <Rotary.h>
#include <Joystick.h>

// Pin Mapping --------------------------------------------------------------------------------
#define HDG 11    // Key 1
#define TRK 10    // Key 2
#define NAV 9     // Key 3
#define APR 8     // Key 4
#define FD 7      // Key 5
#define AP 6      // Key 6
#define YD 5      // Key 7
#define LVL 4     // Key 8
#define VS 3      // Key 9
#define VN 2      // Key 10
#define FLC 1     // Key 11
#define ALT 0     // Key 12

#define rotHDGButton 12     // SW2
#define rotALTButton 13     // SW3

#define rotHDG_A 6     // SW2
#define rotHDG_B 7     // SW2

#define rotALT_A 4     // SW3
#define rotALT_B 5     // SW3

#define rotVS_A 8 // SW1
#define rotVS_B 9 // SW1

// Function prototypes ------------------------------------------------------------------------
void pollButton();
void pollRotary();
void updateButton(boolean buttonState[16]);
void release();

// Setup the joystick library -----------------------------------------------------------------
/* Create the Joystick */
Joystick_ Joystick;

/* Joystock Button Mapping
rotary HDG right      0
rotary HDG left       1
rotary ALT right      2
rotary ALT left       3
rotary VS up          4
rotary VS down        5

button ALT            6
button AP             12
button APR            14
button FD             13
button FLC            7
button HDG            17
button LVL            10
button NAV            15
button TRK            16
button VN             8
button VS             9
button YD             11
rotary ALT            19
rotary HDG            18
*/

// Setup the MCP23017 GPIO expander -----------------------------------------------------------
Adafruit_MCP23017 mcp;

// Setup rotary encoder------------------------------------------------------------------------
Rotary rotHDG = Rotary(rotHDG_A, rotHDG_B);
Rotary rotALT = Rotary(rotALT_A, rotALT_B);
Rotary rotVS = Rotary(rotVS_A, rotVS_B);

// Global variables
const uint8_t numRotary = 6;            // For each rotary encoder two (one per direction)
unsigned long releaseTime[numRotary];    // Stores the release time for the buttons (rotary encoder).
bool rotaryPressed[numRotary] = {false}; // Stores the button state for the rotary enceder (gets automaticly reset)
const int rotaryHoldTime = 50;                  // time a Button is held down in ms (rotaty encoder)
const int buttonHoldTime = 100;            // time a Button is held down in ms (buttons)
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

}

void loop()
{
  pollRotary();
  pollButton();

  release();
}

// Polling the MCPs to ceck for changes
void pollButton()
{
  //We could also read each pin seperatly, however that will cause an I2C transfern for pin.
  //It's faster to read the whole register, and decode the register.
  uint32_t newButtonState = mcp.readGPIOAB();

  if (lastButtonState != newButtonState)
  {

    boolean buttonState[16];
    lastButtonState = newButtonState;
    uint16_t mask = 1;

    for (uint8_t i = 0; i <= 15; i++) // binary masking startig at the least significant bit
    {
      buttonState[i] = newButtonState & (mask << i) ? 1 : 0;
    }

    updateButton(buttonState);
  }
}

// Update the Joystick with the input from the Buttons connectet to the MCP
void updateButton(boolean buttonState[15])
{
  for (uint8_t i = 0; i < 15; i++)
  {
    Joystick.setButton(i + 6, !buttonState[i]);
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
      releaseTime[0 + resultHDG / 16 - 1] = millis() + rotaryHoldTime;
      rotaryPressed[0 + resultHDG / 16 - 1] = true;
    }
  }

  unsigned char resultALT = rotALT.process();
  if (resultALT)
  {
    if (!rotaryPressed[2 + resultALT / 16 - 1]) // rejeckts input when button wasn't releast before
    {
      Joystick.setButton(2 + resultALT / 16 - 1, 1);
      releaseTime[2 + resultALT / 16 - 1] = millis() + rotaryHoldTime;
      rotaryPressed[2 + resultALT / 16 - 1] = true;
    }
  }

  unsigned char resultVS = rotVS.process();
  if (resultVS)
  {
    if (!rotaryPressed[4 + resultVS / 16 - 1]) // rejeckts input when button wasn't releast before
    {
      Joystick.setButton(4 + resultVS / 16 - 1, 1);
      releaseTime[4 + resultVS / 16 - 1] = millis() + rotaryHoldTime;
      rotaryPressed[4 + resultVS / 16 - 1] = true;
    }
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
      releaseTime[i] = millis() + rotaryHoldTime;
      if (rotaryPressed[i])
      {
        rotaryPressed[i] = false;
      }
    };
  };
}