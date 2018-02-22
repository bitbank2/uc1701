UC1701
![UC1701](/uc1701.jpg?raw=true "UC1701")

Copyright(c) 2018 BitBank Software, Inc.<br>
<br>

UC1701 is a simple C library for working with the 128x64 mononchrome LCD display
when connected to the SPI bus.
The idea is to provide a simple interface for C programmers to make use of 
those low-cost LCD displays without having to read the data sheet or figure
out SPI programming. Unfortunately the display controller needs a couple of
control lines (GPIO pins) for Reseti, backlight and Data/Command; this requires extra work.
In this case, I use my ArmbianIO library (https://github.com/bitbank2/ArmbianIO)
<br>
To build the library, simply type 'make' in the terminal. To build the sample
app, type 'make -f make_sample'. This will build the n5110 executable to show
that the library is working.
<br>
Troubleshooting tips:<br>
<b4>
1) Make sure to enable the SPI interface and check the bus number (e.g. /dev/spidev0.0)<br>
2) Connect the "CS" pin of the UC1701 to the CE0 pin on the header.<br>
3) You can use any GPIO pins to control the D/C,RST/LED lines; initialize the
   library with the pin numbers used (not GPIO numbers)<br>
<br>
Written by Larry Bank<br>
Project started 2/22/2018<br>
bitbank@pobox.com<br>

