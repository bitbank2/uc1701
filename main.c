//
// UC1701 LCD display test program
//
// Written by Larry Bank 2/22/2018
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <uc1701.h>

int main(int argc, char* argv[])
{
int rc;
int x;

	// SPI Channel, D/C, RESET, LED
	rc = uc1701Init(0, 18, 13, 11);
	if (rc != 0)
	{
		printf("Problem initializing uc17015110 library\n");
		return 0;
	}
	uc1701Backlight(1);
	uc1701WriteString(36, 0, "Another", FONT_NORMAL);
	uc1701WriteString(36, 1, "BitBank", FONT_NORMAL);
	uc1701WriteString(28, 2, "FOSS tool", FONT_NORMAL);
	uc1701WriteString(30, 3, "Narrow Font", FONT_SMALL);
	uc1701WriteString(0, 5, "BIG Font", FONT_BIG);
	// draw a box around the whole display
	for (x=0; x<30; x++)
	{
		uc1701SetPixel(x, 29-x, 1);
		uc1701SetPixel(127-x, 29-x, 1);
	}
	getchar(); // wait for user to press enter to quit
	uc1701Backlight(0);
	uc1701Shutdown();

   return 0;
} /* main() */
