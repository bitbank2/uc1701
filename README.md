UC1701
![UC1701](/uc1701.jpg?raw=true "UC1701")

Copyright(c) 2018 BitBank Software, Inc.<br>
<br>

UC1701 is a simple C library for working with the 128x64 mononchrome LCD display
when connected to the SPI bus.
The idea is to provide a simple interface for C programmers to make use of 
those low-cost LCD displays without having to read the data sheet or figure
out SPI programming. Unfortunately the display controller needs a couple of
control lines (GPIO pins) for Reset, backlight and Data/Command; this requires extra work.
In this case, I use my ArmbianIO library (https://github.com/bitbank2/ArmbianIO)
<br>
To build the library, simply type 'make' in the terminal. To build the sample
app, type 'make -f make_sample'. This will build the demo executable to show
that the library is working. Press ENTER to exit the demo.
<br>
Troubleshooting tips:<br>
<b4>
1) Make sure to enable the SPI interface and check the bus number (e.g. /dev/spidev0.0)<br>
2) Connect the "CS" pin of the UC1701 to the CE0 pin on the header.<br>
3) You can use any GPIO pins to control the D/C,RST/LED lines; initialize the
   library with the pin numbers used (not GPIO numbers)<br>

Power Consumption:<br>
------------------<br>
This LCD controller is advertised as being extremely low power. It is, but the PCB I received had several power issues that I had to manually solve. There is a 3.3V and 5V version of the PCB (usually JLX12864G). The 5V version includes a 3.3v linear regulator. The regulator doesn't stop it from working when given 3.3V, but it does was a little energy. My board started out at 2.25mA when active and 2.0mA when sleeping. This is quite a bit different from the current draw claimed in the UC1701 datasheet. I removed the linear regulator, but that only reduced the current a small amount. I then discovered that my PCB included a Chinese character font EPROM (U2). This chip when idle was drawing nearly 2mA. I removed it and now the LCD draws nearly 0 when idle and 276uA when active. The backlight (with included 120 ohm resistor) draws about 4.8mA. A good improvement over the Nokia 5110 (400uA active current).<br>

Written by Larry Bank<br>
Project started 2/22/2018<br>
bitbank@pobox.com<br>

