// uc1701 LCD library
// Written by Larry Bank (bitbank@pobox.com)
// Project started 2/22/2018
//
// The uc1701 LCD display is controlled through the SPI interface
// and three GPIO pins to control the RST (reset), D/C (data/command),
// and LED backlight control lines. 
// The LCD controller is set to "horizontal mode". This divides the display
// into 8 128x8 "pages" or strips. Each data write advances the output
// automatically to the next address. The bytes are arranged such that the LSB
// is the topmost pixel and the MSB is the bottom.
// A copy of the display memory is maintained by this code so that single pixel
// writes can occur without having to read from the display controller.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "uc1701.h"
#include <armbianio.h>

extern unsigned char ucFont[], ucSmallFont[];
static int iScreenOffset; // current write offset of screen data
static unsigned char ucScreen[1024]; // local copy of the image buffer
static int file_spi = 0;
static int iDCPin, iResetPin, iLEDPin;

#define LOW 0
#define HIGH 1

static void uc1701WriteCommand(unsigned char);
int uc1701Fill(unsigned char);

typedef enum
{
 MODE_DATA = 0,
 MODE_COMMAND
} DC_MODE;

// Sets the D/C pin to data or command mode
void uc1701SetMode(int iMode)
{
	AIOWriteGPIO(iDCPin, (iMode == MODE_DATA));
} /* uc1701SetMode() */

// Opens a file system handle to the SPI device
// Initializes the LCD controller into "horizontal write mode"
// Prepares the font data for the orientation of the display
// Parameters: GPIO pin numbers used for the DC/RST/LED control lines
//
int uc1701Init(int iChannel, int iDC, int iReset, int iLED)
{

        iDCPin = iDC;
        iResetPin = iReset;
        iLEDPin = iLED;

        if (!AIOInit(NULL))
	{
		fprintf(stderr, "Error initializing ArmbianIO library\n");
		return -1;
	}
	file_spi = AIOOpenSPI(iChannel, 4000000); // Initialize SPI channel at 4Mhz
	if (file_spi == -1)
	{
		fprintf(stderr, "Failed to open the SPI bus\n");
		file_spi = 0;
		return -1;
	}

        AIOAddGPIO(iDCPin, GPIO_OUT);
        AIOAddGPIO(iResetPin, GPIO_OUT);
	AIOAddGPIO(iLEDPin, GPIO_OUT);

	// Start by reseting the LCD controller
        AIOWriteGPIO(iResetPin, HIGH);
	usleep(50000);
	AIOWriteGPIO(iResetPin, LOW);
	usleep(5000);
	AIOWriteGPIO(iResetPin, HIGH); // take it out of reset
	uc1701Backlight(1); // turn on the backlight

	uc1701SetMode(MODE_COMMAND);
	uc1701WriteCommand(0xe2); // system reset
	uc1701WriteCommand(0x40); // set LCD start line to 0
	uc1701WriteCommand(0xa0); // set SEG direction (A1 to flip horizontal)
	uc1701WriteCommand(0xc8); // set COM direction (C0 to flip vert)
	uc1701WriteCommand(0xa2); // set LCD bias mode 1/9
	uc1701WriteCommand(0x2c); // set boost on
	uc1701WriteCommand(0x2e); // set voltage regulator on
	uc1701WriteCommand(0x2f); // Voltage follower on
	uc1701WriteCommand(0xf8); // set booster ratio to
	uc1701WriteCommand(0x00); // 4x
	uc1701WriteCommand(0x23); // set resistor ratio = 4
	uc1701WriteCommand(0x81);
	uc1701WriteCommand(0x30); // set contrast = 48
	uc1701WriteCommand(0xac); // set static indicator off
	uc1701WriteCommand(0x00);
	uc1701WriteCommand(0xa6); // disable inverse
	uc1701WriteCommand(0xaf); // enable display
	usleep(100000);
	uc1701WriteCommand(0xa5); // display all points
	usleep(200000);
	uc1701WriteCommand(0xa4); // normal display

	uc1701Fill(0); // erase memory
	return 0;

} /* uc1701Init() */

// Controls the LED backlight
void uc1701Backlight(int bOn)
{
	AIOWriteGPIO(iLEDPin, (bOn) ? HIGH:LOW);
} /* uc1701Backlight() */

// Sends a command to turn off the LCD display
// Closes the SPI file handle
void uc1701Shutdown()
{
	if (file_spi != 0)
	{
		uc1701SetMode(MODE_COMMAND);
		uc1701Backlight(0); // turn off the backlight
		uc1701WriteCommand(0xae); // power down
		AIOCloseSPI(file_spi);
		file_spi = 0;
	}
}

void uc1701WriteCommand(unsigned char c)
{
unsigned char buf[2];

	uc1701SetMode(MODE_COMMAND);
	buf[0] = c;
	AIOWriteSPI(file_spi, buf, 1);
} /* uc1701WriteCommand() */

int uc1701SetContrast(unsigned char ucContrast)
{
        if (file_spi == 0)
                return -1;

	uc1701WriteCommand(0x81); // set contrast 
	uc1701WriteCommand(ucContrast);
	return 0;
} /* uc1701SetContrast() */

// Send commands to position the "cursor" to the given
// row and column
static void uc1701SetPosition(int x, int y)
{
	uc1701WriteCommand(0xb0 | y); // set Y
	uc1701WriteCommand(0x10 | (x >> 4)); // set X (high MSB)
	uc1701WriteCommand(0x00 | (x & 0xf)); // set X (low MSB)
	iScreenOffset = (y*128)+x;
}

// Write a block of pixel data to the LCD
// Length can be anything from 1 to 504 (whole display)
static void uc1701WriteDataBlock(unsigned char *ucBuf, int iLen)
{

	uc1701SetMode(MODE_DATA);
	AIOWriteSPI(file_spi, ucBuf, iLen);
	// Keep a copy in local buffer
	memcpy(&ucScreen[iScreenOffset], ucBuf, iLen);
	iScreenOffset += iLen;
}

// Set (or clear) an individual pixel
// The local copy of the frame buffer is used to avoid
// reading data from the display controller
int uc1701SetPixel(int x, int y, unsigned char ucColor)
{
int i;
unsigned char uc, ucOld;

	if (file_spi == 0)
		return -1;

	i = ((y >> 3) * 128) + x;
	if (i < 0 || i > 1023) // off the screen
		return -1;
	uc = ucOld = ucScreen[i];
	uc &= ~(0x1 << (y & 7));
	if (ucColor)
	{
		uc |= (0x1 << (y & 7));
	}
	if (uc != ucOld) // pixel changed
	{
		uc1701SetPosition(x, y>>3);
		uc1701WriteDataBlock(&uc, 1);
	}
	return 0;
} /* uc1701SetPixel() */

//
// Read the pixel at the given x,y
// if the library is not initialized, or the coordinates are
// not within the valid range, it returns 0
//
int uc1701GetPixel(int x, int y)
{
int i;

   if (file_spi == 0)
	return 0; // invalid request returns "black" pixel
   i = ((y >> 3) * 128) + x;
   if (i < 0 || i > 1023) // off the screen
	return 0;
   return (ucScreen[i] & (1<< (y & 7)));
} /* uc1701GetPixel() */

// Draw a string of small (8x8) or large (16x24) characters
// At the given col+row
int uc1701WriteString(int x, int y, char *szMsg, int iSize)
{
int i, iLen;
unsigned char *s;

	if (file_spi == 0) return -1; // not initialized

	iLen = strlen(szMsg);
	if (iSize == FONT_BIG) // draw 16x32 font
	{
		if ((iLen*16)+x > 128) iLen = (128-x)/16;
		if (iLen < 0) return -1;
		for (i=0; i<iLen; i++)
		{
			s = &ucFont[9728 + (unsigned char)szMsg[i]*64];
			uc1701SetPosition(x+(i*16), y);
			uc1701WriteDataBlock(s, 16);
			uc1701SetPosition(x+(i*16), y+1);
			uc1701WriteDataBlock(s+16, 16);	
			uc1701SetPosition(x+(i*16), y+2);
			uc1701WriteDataBlock(s+32, 16);	
		}
	}
	else if (iSize == FONT_NORMAL) // draw 8x8 font
	{
		uc1701SetPosition(x, y);
		if ((8*iLen) + x > 128) iLen = (128 - x)/8; // can't display it all
		if (iLen < 0)return -1;

		for (i=0; i<iLen; i++)
		{
			s = &ucFont[(unsigned char)szMsg[i] * 8];
			uc1701WriteDataBlock(s, 8); // write character pattern
		}	
	}
	else // small
	{
		uc1701SetPosition(x, y);
		if ((6*iLen) + x > 128) iLen = (128 - x)/6;
		if (iLen < 0) return -1;
		for (i=0; i<iLen; i++)
		{
			s = &ucSmallFont[(unsigned char)szMsg[i]*6];
			uc1701WriteDataBlock(s, 6);
		}
	}
	return 0;
} /* uc1701WriteString() */

// Fill the frame buffer with a byte pattern
// e.g. all off (0x00) or all on (0xff)
int uc1701Fill(unsigned char ucData)
{
int y;
unsigned char temp[128];

	if (file_spi == 0) return -1; // not initialized

	memset(temp, ucData, 128);
	for (y=0; y<8; y++)
	{
		uc1701SetPosition(0,y); // set to (0,Y)
		uc1701WriteDataBlock(temp, 128); // fill with data byte
	} // for y
	return 0;
} /* uc1701Fill() */

