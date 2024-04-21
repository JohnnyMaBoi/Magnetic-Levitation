# Magnetic-Levitation

## Requirements

Physical Components: Arduino uno, uxcell 12v 180N P34/18 (Electromagnet),
Adafruit DRV8871 (Motor Driver), and TRI DRV5055A4QLPGM (Hall Effect Sensor) to
levitate a magnet.

Required packages (see useful links): Platformio vscode extension + package
(arduino development),Teleplot

## Quick Start:

After installing platformio, click the sidebar icon and use the option "select
folder" and select the folder containing platformio.ini \
To upload a main script to the arduino, use the command pio run -t upload -e {name_after_main}
\
i.e. pio run -t upload -e ramp to run main-ramp.cpp

## Useful Links

https://community.platformio.org/t/multiple-main-cpp-files/26181 \
// multiple platformio main scripts compiled through {pio run -t upload -e blink}
to upload script main-blink.cpp for example

https://docs.platformio.org/en/latest/integration/ide/vscode.html#serial-udp-plotter
\
// idk if you already used this but plots serial data live

https://electronics.stackexchange.com/questions/255040/control-electromagnet-with-pwm-any-risks
\
// interesting resource to understand some PWM implications

## General Notes:

-hall effect testing script: magnet at different ranges -live output plot -break
capacitance?

analogwrite uses PWM

DRIVER - current depends on resistor 7.3.3
