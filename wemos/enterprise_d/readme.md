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

### WiFi Manager
To allow a custom configuration for the WiFi, I am using the following WiFi Manager library: https://github.com/tzapu/WiFiManager
You can implement it by using the **WiFiManager** by tzapu from the Library Manager
It will try to connect to an existing Wifi - and if not, it will create its own Wifi including a Profile Manager where the user can setup the WiFi settings for his Network.
