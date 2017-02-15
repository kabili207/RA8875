//
// This file is heavily based on the adafruit project for the RA8875
// https://github.com/adafruit/Adafruit_RA8875
//
#pragma once

#include "IDevice.h"
#include "SPI.h"

// Colors (RGB565)
#define	RA8875_BLACK            0x0000
#define	RA8875_BLUE             0x001F
#define	RA8875_RED              0xF800
#define	RA8875_GREEN            0x07E0
#define RA8875_CYAN             0x07FF
#define RA8875_MAGENTA          0xF81F
#define RA8875_YELLOW           0xFFE0
#define RA8875_WHITE            0xFFFF

#define RA8875_PWM_CLK_DIV1     0x00
#define RA8875_PWM_CLK_DIV2     0x01
#define RA8875_PWM_CLK_DIV4     0x02
#define RA8875_PWM_CLK_DIV8     0x03
#define RA8875_PWM_CLK_DIV16    0x04
#define RA8875_PWM_CLK_DIV32    0x05
#define RA8875_PWM_CLK_DIV64    0x06
#define RA8875_PWM_CLK_DIV128   0x07
#define RA8875_PWM_CLK_DIV256   0x08
#define RA8875_PWM_CLK_DIV512   0x09
#define RA8875_PWM_CLK_DIV1024  0x0A
#define RA8875_PWM_CLK_DIV2048  0x0B
#define RA8875_PWM_CLK_DIV4096  0x0C
#define RA8875_PWM_CLK_DIV8192  0x0D
#define RA8875_PWM_CLK_DIV16384 0x0E
#define RA8875_PWM_CLK_DIV32768 0x0F

namespace hw
{
	class RA8875
	{
	public:
		enum DisplaySize
		{
			_480x272,
			_800x480
		};

		RA8875(IDevice& device, Pin cs = Pin::D3, Pin rst = Pin::D4);
		~RA8875();

		bool    begin(DisplaySize s);
		void    softReset();
		void    displayOn(bool on);
		void    sleep(bool sleep);

		/* Text functions */
		void    textMode();
		void    textSetCursor(uint16_t x, uint16_t y);
		void    textColor(uint16_t foreColor, uint16_t bgColor);
		void    textTransparent(uint16_t foreColor);
		void    textEnlarge(uint8_t scale);
		void    textWrite(const char* buffer, uint16_t len = 0);

		/* Graphics functions */
		void    graphicsMode();
		void    setXY(uint16_t x, uint16_t y);
		void    fillRect();

		/* HW accelerated wrapper functions (override Adafruit_GFX prototypes) */
		void    fillScreen(uint16_t color);
		void    drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
		void    drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
		void    fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
		void    drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
		void    fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
		void    drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
		void    fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
		void    drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color);
		void    fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color);
		void    drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color);
		void    fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color);

		/* Backlight */
		void    GPIOX(bool on);
		void    PWM1config(bool on, uint8_t clock);
		void    PWM2config(bool on, uint8_t clock);
		void    PWM1out(uint8_t p);
		void    PWM2out(uint8_t p);

		/* Touch screen */
		void    touchEnable(bool on);
		bool    touched();
		bool    touchRead(uint16_t *x, uint16_t *y);

		/* Low level access */
		void     writeReg(uint8_t reg, uint8_t val);
		uint8_t  readReg(uint8_t reg);
		void     writeData(uint8_t d);
		uint8_t  readData();
		void     writeCommand(uint8_t d);
		uint8_t  readStatus();
		bool     waitPoll(uint8_t r, uint8_t f);
		uint16_t width() const;
		uint16_t height() const;

	private:
		void PLLinit();
		void initialize();

		/* GFX Helper Functions */
		void circleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled);
		void rectHelper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled);
		void triangleHelper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled);
		void ellipseHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color, bool filled);
		void curveHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color, bool filled);

		/* timing helper */
		static void delay(int ms);

	private:
		IDevice*    m_device;
		SPI         m_spi;
		Pin         m_rst;
		uint16_t    m_width;
		uint16_t    m_height;
		uint8_t     m_textScale;
		DisplaySize m_size;
	};
}

