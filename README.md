# open AutopilotPanel
 A 3D printable autopilot panel for Microsoft Flightsimulator modeled after the Garmin GFC 500

 ## Features
* 3 Rotary Encoder (HDG / ALT / VS up/down)
* 12 Pushbuttons
* Emulates a standard gamecontroller (no additional software needed)

## Uses Materials
 * 1 [Arduino Pro Micro](https://www.amazon.de/gp/product/B07J2Q3ZD5/ref=ppx_yo_dt_b_asin_title_o03_s00?ie=UTF8&psc=1)
 * 1 [MCP23017 I²C Port Expander](https://www.amazon.de/gp/product/B086W6ZQ36/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)
 * 3 [Rotary Encoder](https://www.amazon.de/gp/product/B08728PS6N/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1)
 * 12 [Miro Pushbuttons 6x6x5](https://www.amazon.de/gp/product/B07Q1BXV7T/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&psc=1)
 * 26 AWG silicone wire
 * Dupont connectors
 * Double sided tape

 ## Assembly
 The order the Buttons are wired is not important because you will assigen them in the Flight Simulator. It is only important to wire all the Buttons to the MCP23017 Port Expander and the Rotary Encoder directly to the Arduino. Also the reset pin on MCP23017 has to be tied to 5V and the address pins to ground. A [wiring diagram](https://github.com/GameOver94/open-AutopilotPanel/blob/main/figures/Schematic_open-Autopilot.pdf) can be found in the figures folder.

 ![image_assemby1](https://github.com/GameOver94/open-AutopilotPanel/blob/develop/figures/assembly1.jpg)
 ![image_assembly2](https://github.com/GameOver94/open-AutopilotPanel/blob/develop/figures/assembly2.jpg)
 ![image_assembly3](https://github.com/GameOver94/open-AutopilotPanel/blob/develop/figures/assembly3.jpg)

## ToDo
* Add PCB design to simplify assambly
* Maybe a version 2 with an added lcd to show AP mode, set altitude usw. (requires software running on the PC)
