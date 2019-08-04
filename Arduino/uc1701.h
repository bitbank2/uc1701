//
// UC1701 LCD display library
// written by Larry Bank
// Copyright (c) 2018 BitBank Software, Inc.
// converted from Linux 2/23/2018
//
#define FONT_NORMAL 0
#define FONT_SMALL 1
#define FONT_STRETCHED 2
#define FONT_LARGE 3

// Rotation and flip angles to draw tiles
enum {
  ANGLE_0=0,
  ANGLE_90,
  ANGLE_180,
  ANGLE_270,
  ANGLE_FLIPX,
  ANGLE_FLIPY
};

//
// Uncomment this define to allow the pixel functions to work
// It requires an extra 1024 bytes of memory (rare on Arduinos)
//
#define BACKING_RAM

// Write a block of pixel data from FLASH ROM to the LCD
void uc1701WriteFlashBlock(unsigned char *ucBuf, int iLen);
// Write a repeating byte to the LCD
void uc1701RepeatByte(unsigned char uc, int iLen);

// Send commands to position the "cursor" to the given
// row and column
void uc1701SetPosition(int x, int y);
//
// Write a block of pixel data to the LCD
// Length can be anything from 1 to 504 (whole display)
void uc1701WriteDataBlock(unsigned char *ucBuf, int iLen);
//
// Load a 128x64 1-bpp Windows bitmap
//
int uc1701LoadBMP(byte *pBMP);
//
// Returns a pointer to the internal display buffer
//
uint8_t * uc1701GetDisplayBuffer(void);

//
// Power up the display
//
void uc1701PowerUp(void);
//
// Set the display into low power mode
//
void uc1701PowerDown(void);
//
// Initializes the LCD controller into "horizontal write mode"
// Prepares the font data for the orientation of the display
// Parameters: GPIO pin numbers used for the DC/RST/LED control lines
// If not using the LED backlight, set iLED to -1
//
int uc1701Init(int iDC, int iReset, int iLED, int iCS, byte bFlip180, byte bInvert, int32_t iClock);
//
// Fill the display with a byte pattern
//
void uc1701Fill(byte pattern);

//
// Fill the display with a byte pattern
//
void uc1701Fill(byte pattern);

//
// Controls the LED backlight
//
void uc1701Backlight(int bOn);
//
// Set the contrast value (0 = min, 63 = max, 32 = default)
//
int uc1701SetContrast(unsigned char ucContrast);
//
// Set (or clear) an individual pixel
// The local copy of the frame buffer is used to avoid
// reading data from the display controller
//
int uc1701SetPixel(int x, int y, unsigned char ucColor);
//
// Read the pixel at the given x,y
// if the library is not initialized, or the coordinates are
// not within the valid range, it returns 0
//
int uc1701GetPixel(int x, int y);
//
// Draw a string of normal (8x8), small (6x8) or large (16x24) characters
// At the given col+row
//
int uc1701WriteString(int x, int y, char *szMsg, int bInverted, int iSize);
//
// Draw an arbitrary line from x1,y1 to x2,y2
//
void uc1701DrawLine(int x1, int y1, int x2, int y2);
//
// Draw a 16x16 tile in any of 4 rotated positions
// Assumes input image is laid out like "normal" graphics with
// the MSB on the left and 2 bytes per line
// On AVR, the source image is assumed to be in FLASH memory
// The function can draw the tile on byte boundaries, so the x value
// can be from 0 to 112 and y can be from 0 to 7
//
void uc1701DrawTile(uint8_t *pTile, int x, int y, int iRotation);

