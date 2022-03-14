# requirements

## Hardware Requirements

### DIO Extension
to extend the Digital In- and Outputs of the Arduino, we need PCF8574 Chips (Boards) which comes with I²C support.
I'm using the following Library for the PCF8574 in this code:
**PCF8574library** by *Renzo Mischianti* (https://www.mischianti.org/category/my-libraries/pcf8574/)

### AIO Extension
to extend the Analog In- and Outputs of the Arduino, we need a PCF8591 Chip (Board) which comes with I²C support as well.
In my project I'm using the following library:
**PCF8591library** by *Renzo Mischianti* (https://www.mischianti.org/2019/01/03/pcf8591-i2c-analog-i-o-expander/)

### Torpedo Launcher
The Model comes with one Torpedo Launcher on the Battle-Section, which is the default setting of this sketch.
Alternative configuration has added a second Torpedo Launcher on the StarDrive Section, which will be the "rearTorpedoLauncher".
The amount of Torpedo Launches can be set up either with a Rotary Encoder or a Potentiometer.
Depending on which device will be used, there are some further Hardware changes required.

The Rotary Encoder does have an additional "Button" input, which can be used to launch the Torpedo.
When using the Potentiometer, it is required to add another button or Touchsensor for starting the Torpedo Launch Sequence.

The current amount of Torpedo Launches will be shown on a 7 Segment Display
