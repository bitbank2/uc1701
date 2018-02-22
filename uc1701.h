#ifndef UC1701_H
#define UC1701_H
//
// UC1701
// Library for accessing the UC1701 128x64 LCD display
// Written by Larry Bank (bitbank@pobox.com)
// Copyright (c) 2018 BitBank Software, Inc.
// Project started 2/22/2018
//
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

typedef enum
{
	FONT_NORMAL=0,
	FONT_BIG,
	FONT_SMALL
} FONTSIZE;

// Initialize the library
int uc1701Init(int iSPIChannel, int iDC, int iReset, int iLED);

// Turns off the display and frees the resources
void uc1701Shutdown(void);

// Fills the display with the byte pattern
int uc1701Fill(unsigned char ucPattern);

// Turns the backlight on or off
void uc1701Backlight(int bOn);

// Write a text string to the display at x (column 0-83) and y (row 0-5)
// iSize = 0 - 8x8 font, iSize = 1 - 16x24 font, iSize = 2 - 6x8 font
int uc1701WriteString(int x, int y, char *szText, int iSize);

// Sets a pixel to On (1) or Off (0)
// Coordinate system is pixels, not text rows (0-83, 0-47)
int uc1701SetPixel(int x, int y, unsigned char ucPixel);

// Reads a pixel (as 0 or 1)
// Coordinate system is pixel, not text rows (0-83, 0-47)
int uc1701GetPixel(int x, int y);

// Sets the contrast level of the display
// Valid values are 0-255 where 0=lightest and 255=darkest
int uc1701SetContrast(unsigned char ucContrast);

#endif // UC1701_H
