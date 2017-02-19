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

		RA8875(IDevice& device, Pin cs = Pin::D3, Pin rst = Pin::D4, Pin wait = Pin::D5, Pin interrupt = Pin::D6);
		~RA8875();

		bool    begin(DisplaySize s);
		void    softReset() const;
		void    displayOn(bool on) const;
		void    sleep(bool sleep) const;

		/* Text functions */
		void    textMode() const;
		void    textSetCursor(uint16_t x, uint16_t y) const;
		void    textColor(uint16_t foreColor, uint16_t bgColor) const;
		void    textTransparent(uint16_t foreColor) const;
		void    textEnlarge(uint8_t scale);
		void    textWrite(const char* buffer) const;

		/* Graphics functions */
		void    graphicsMode() const;
		void    setXY(uint16_t x, uint16_t y) const;
		void    fillRect() const;

		/* HW accelerated wrapper functions (override Adafruit_GFX prototypes) */
		void    fillScreen(uint16_t color) const;
		void    drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) const;
		void    drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) const;
		void    fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) const;
		void    drawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color) const;
		void    fillCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color) const;
		void    drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) const;
		void    fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) const;
		void    drawEllipse(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color) const;
		void    fillEllipse(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color) const;
		void    drawCurve(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint8_t curvePart, uint16_t color) const;
		void    fillCurve(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint8_t curvePart, uint16_t color) const;

		/* Backlight */
		void    GPIOX(bool on) const;
		void    PWM1config(bool on, uint8_t clock) const;
		void    PWM2config(bool on, uint8_t clock) const;
		void    PWM1out(uint8_t p) const;
		void    PWM2out(uint8_t p) const;

		/* Touch screen */
		void    touchEnable(bool on) const;
		bool    touched() const;
		bool    touchRead(uint16_t *x, uint16_t *y) const;

		/* Low level access */
		void     setRegister8(uint8_t reg, uint8_t val) const;
		void     setRegister16(uint8_t reg, uint16_t val) const;
		void     setColorRegister(uint8_t reg, uint16_t color) const;
		uint8_t  readRegister8(uint8_t reg) const;

		void     writeData(uint8_t d) const;
		uint8_t  readData() const;
		void     writeCommand(uint8_t d) const;
		uint8_t  readStatus() const;
		bool     waitPoll(uint8_t r, uint8_t f) const;
		uint16_t width() const;
		uint16_t height() const;

	private:
		void PLLinit() const;
		void initialize() const;

		/* GFX Helper Functions */
		void circleHelper(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color, bool filled) const;
		void rectHelper(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, bool filled) const;
		void triangleHelper(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, bool filled) const;
		void ellipseHelper(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color, bool filled) const;
		void curveHelper(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint8_t curvePart, uint16_t color, bool filled) const;

		/* timing helper */
		static void delay(int ms);

	private:
		IDevice*    m_device;
		SPI         m_spi;
		Pin         m_rst;
		Pin         m_wait;
		Pin         m_interrupt;
		uint16_t    m_width;
		uint16_t    m_height;
		uint8_t     m_textScale;
		DisplaySize m_size;
	};
}

