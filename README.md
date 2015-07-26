# TempMonitor

This sketch is a temperature monitor. It is based on a Arduino UNO and grove sensors:

* a [Temperature sensor](http://www.seeedstudio.com/wiki/Grove_-_Temperature_Sensor), connected to the A0 port.
* a [Rotaty angle sensor](http://www.seeedstudio.com/wiki/Grove_-_Rotary_Angle_Sensor), connected to the A1 port.
* a [96x96 OLED display](http://www.seeedstudio.com/wiki/index.php?title=Grove_-_OLED_Display_96*96), connected to any I2C port (make sure to install the LCD_Display library).


Temperature is measured every minute, and averaged over a period of 15 minutes.

The potentiometer is used to browse a 24h history of the temperature.

## License

This source code is released under the terms of the MIT License. See the LICENSE.txt file for more details.