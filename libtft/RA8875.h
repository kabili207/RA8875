//
// This file is heavily based on the adafruit project for the RA8875
// https://github.com/adafruit/Adafruit_RA8875
//
#pragma once

#define __PRGMTAG_

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

enum TFT_Register : uint8_t;

enum TFT_DisplaySize
{
	_480x272 = 0,
	_800x480 = 1
};

typedef struct {
		const uint8_t 	*data;
		uint8_t 		image_width;
		int				image_datalen;
} tImage;

typedef struct {
		uint8_t 		char_code;
		const tImage 	*image;
} tChar;

typedef struct {
		uint8_t 		length;
		const tChar 	*chars;
		uint8_t			font_width;
		uint8_t			font_height;
		bool 			rle;
} tFont;

enum TFT_Font
{
	Internal = 0,
	Calibri20,
	Calibri24,
	Calibri30,
	Consolas20,
	Consolas24,
	ComicNeue20,
	ComicNeue24,
};

// Font Parameters
// index:x -> w,h,baselineLowOffset,baselineTopOffset,variableWidth
const static uint8_t fontDimPar[4][5] = {
	{8,16,2,4,0},// INT font
	{8,16,3,0,0},// ROM X16
	{12,24,2,2,0},//ROM X24
	{16,32,2,2,0},//ROM X32
};

namespace hw
{
	class RA8875
	{
	public:

		RA8875(IDevice& device, Pin cs = Pin::D3, Pin rst = Pin::D4, Pin wait = Pin::D5, Pin interrupt = Pin::D6);
		~RA8875();

		bool    begin(TFT_DisplaySize s);
		void    softReset() const;
		void    displayOn(bool on) const;
		void    sleep(bool sleep) const;
		void    setActiveWindow(void);
		void    setActiveWindow(uint16_t XL,uint16_t XR ,uint16_t YT ,uint16_t YB);
		void    getActiveWindow(uint16_t &XL,uint16_t &XR ,uint16_t &YT ,uint16_t &YB);

		/* Text functions */
		void    textMode() const;
		void    textSetCursor(uint16_t x, uint16_t y);
		void    textColor(uint16_t foreColor, uint16_t bgColor);
		void    textTransparent(uint16_t foreColor);
		void    textEnlarge(uint8_t scale);
		void    textWrite(const char* buffer);
		void    setFontScale(uint8_t xscale,uint8_t yscale);
		void    setFont(TFT_Font font);
		void    setInternalFont();
		void    setUserFont(const tFont *font);
	
	private:
		void    _setFNTdimensions(uint8_t index);
		void    _textWrite(const char* buffer, uint16_t len);
		void    _charWriteR(const char c,uint8_t offset,uint16_t fcolor,uint16_t bcolor);
		void    _charWrite(const char c,uint8_t offset);
		int     _getCharCode(uint8_t ch);
		void    _textPosition(int16_t x, int16_t y,bool update);
		int16_t _STRlen_helper(const char*, uint16_t);
		void    _drawChar_unc(int16_t x,int16_t y,int charW,int index,uint16_t fcolor);
		void    _charLineRender(bool lineBuffer[],int charW,int16_t x,int16_t y,int16_t currentYposition,uint16_t fcolor);
	
	public:
		/* Graphics functions */
		void    graphicsMode() const;
		void    setXY(uint16_t x, uint16_t y) const;
		//void    fillRect() const;

		/* HW accelerated wrapper functions (override Adafruit_GFX prototypes) */
		void    fillScreen(uint16_t color) const;
		void    drawPixel(int16_t x, int16_t y, uint16_t color) const;
		void    drawPixels(uint16_t p[], uint16_t count, int16_t x, int16_t y) const;
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
		void    brightness(uint8_t val);
		void    backlight(bool on) const;
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
		void     setRegister8(TFT_Register reg, uint8_t val) const;
		void     setRegister16(TFT_Register reg, uint16_t val) const;
		void     setColorRegister(TFT_Register reg, uint16_t color) const;
		uint8_t  readRegister8(TFT_Register reg) const;

		void     writeData(uint8_t d) const;
		uint8_t  readData() const;
		void     writeCommand(uint8_t d) const;
		uint8_t  readStatus() const;
		bool     waitPoll(TFT_Register reg, uint8_t f) const;
		void     waitBusy(uint8_t res=0x80);//0x80, 0x40(BTE busy), 0x01(DMA busy)
		uint16_t width() const;
		uint16_t height() const;

	private:
		void _updateActiveWindow(bool full) const;
		void _setSysClock(uint8_t pll1, uint8_t pll2, uint8_t pixclk);
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
		int16_t     m_activeWindowXL;
		int16_t     m_activeWindowXR;
		int16_t     m_activeWindowYT;
		int16_t     m_activeWindowYB;
		uint8_t     m_brightness;
		uint16_t    m_foreColor;
		uint16_t    m_backColor;
		uint16_t    m_cursorX;
		uint16_t    m_cursorY;
		uint8_t     m_scaleX;
		uint8_t     m_scaleY;
		bool        m_scaling;
		bool        m_renderFonts;
		uint8_t     m_FNTspacing;
		uint8_t     m_FNTinterline;
		uint8_t     m_FNTwidth;
		uint8_t     m_FNTheight;
		uint8_t     m_FNTbaselineLow;
		uint8_t     m_FNTbaselineTop;
		bool        m_FNTcompression;
		int         m_spaceCharWidth;
		TFT_DisplaySize m_size;
		const tFont * m_currentFont;
	
	};
}

