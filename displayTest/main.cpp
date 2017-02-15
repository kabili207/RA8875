#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "FT232H.h"
#include "RA8875.h"
#include <thread>

static void delay(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main(void)
{
	hw::FT232H ft232h;
	if (!ft232h.open())
	{
		return -1;
	}

	hw::RA8875 tft(ft232h);

	if (!tft.begin(hw::RA8875::_800x480)) {
		fprintf(stderr, "RA8875 Not Found!\n");
		return -1;
	}
	
	tft.displayOn(true);
	tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
	tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	tft.PWM1out(255);
	tft.fillScreen(0x1111);

	/* Switch to text mode */
	tft.textMode();

	/* Set a solid for + bg color ... */
	/* ... or a fore color plus a transparent background */

	/* Set the cursor location (in pixels) */
	tft.textSetCursor(10, 10);

	/* Render some text! */
	char string[15] = "Hello, World! ";
	tft.textTransparent(RA8875_WHITE);
	tft.textWrite(string);
	tft.textColor(RA8875_WHITE, RA8875_RED);
	tft.textWrite(string);
	tft.textTransparent(RA8875_CYAN);
	tft.textWrite(string);
	tft.textTransparent(RA8875_GREEN);
	tft.textWrite(string);
	tft.textColor(RA8875_YELLOW, RA8875_CYAN);
	tft.textWrite(string);
	tft.textColor(RA8875_BLACK, RA8875_MAGENTA);
	tft.textWrite(string);

	/* Change the cursor location and color ... */
	tft.textSetCursor(100, 100);
	tft.textTransparent(RA8875_RED);
	/* If necessary, enlarge the font */
	tft.textEnlarge(1);
	/* ... and render some more text! */
	tft.textWrite(string);
	tft.textSetCursor(100, 150);
	tft.textEnlarge(2);
	tft.textWrite(string);
	delay(500);

	// the other testing stuff.
	tft.graphicsMode();

	tft.fillScreen(RA8875_RED);
	delay(500);
	tft.fillScreen(RA8875_YELLOW);
	delay(500);
	tft.fillScreen(RA8875_GREEN);
	delay(500);
	tft.fillScreen(RA8875_CYAN);
	delay(500);
	tft.fillScreen(RA8875_MAGENTA);
	delay(500);
	tft.fillScreen(RA8875_BLACK);

	// Try some GFX acceleration!
	for (int i = 0; i < 50; ++i)
	{
		int x = rand() % tft.width();
		int y = rand() % tft.height();
		tft.fillCircle(x, y, 50, rand() & 0xffff);
		tft.drawCircle(x, y, 50, RA8875_BLACK);
	}

	tft.fillRect(11, 11, 398, 198, RA8875_BLUE);
	tft.drawRect(10, 10, 400, 200, RA8875_GREEN);
	//tft.fillRoundRect(200, 10, 200, 100, 10, RA8875_RED);
	//tft.drawPixel(10, 10, RA8875_BLACK);
	//tft.drawPixel(11, 11, RA8875_BLACK);
	tft.drawLine(10, 10, 200, 100, RA8875_RED);
	tft.drawTriangle(200, 15, 250, 100, 150, 125, RA8875_BLACK);
	tft.fillTriangle(200, 16, 249, 99, 151, 124, RA8875_YELLOW);
	tft.drawEllipse(300, 100, 100, 40, RA8875_BLACK);
	tft.fillEllipse(300, 100, 98, 38, RA8875_GREEN);
	// Argument 5 (curvePart) is a 2-bit value to control each corner (select 0, 1, 2, or 3)
	tft.drawCurve(50, 100, 80, 40, 2, RA8875_BLACK);
	tft.fillCurve(50, 100, 78, 38, 2, RA8875_WHITE);

	delay(1000);

	tft.displayOn(false);
	tft.softReset();

	ft232h.close();
	return 0;
}
