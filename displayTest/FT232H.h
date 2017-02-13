#pragma once

#include <stdint.h>
#include <initializer_list>

enum class Pin
{
	D0, D1, D2, D3, D4, D5, D6, D7,
	C0, C1, C2, C3, C4, C5, C6, C7,
};

enum class Mode
{
	In, Out,
};

class FT232H
{
public:
	FT232H();
	~FT232H();

	bool open();
	void close();

	void mpsse_set_clock(int clock_hz, bool adaptive = false, bool three_phase = false);
	uint16_t mpsse_read_gpio();
	void mpsse_write_gpio();

	void setup(Pin pin, Mode mode);
	void output(Pin pin, bool value);
	bool input(Pin pin);

	void set_high(Pin pin) { output(pin, true); }
	void set_low(Pin pin)  { output(pin, false); }
	bool is_high(Pin pin) { return input(pin); }
	bool is_low(Pin pin)  { return !input(pin); }

	int write(uint8_t data);
	int write(const uint8_t* data, size_t length);
	int write(const std::initializer_list<uint8_t>& list);

	int _poll_read(uint8_t* data, int expected, int timeout_s = 5.0);

private:
	void mpsse_enable();
	void mpsse_sync(int max_retries = 10);

	void     _setup_pin(Pin pin, Mode mode);
	void     _output_pin(Pin pin, bool value);
	uint16_t pinToMask(Pin pin) const;

private:
	struct ftdi_context* m_ftdi;

	uint16_t m_gpio_direction;
	uint16_t m_gpio_values;
};


class SPI
{
public:
	SPI(FT232H* device, Pin cs, int max_speed_hz = 1000000, int mode = 0, bool lsbFirst = false);
	~SPI();

	void set_clock_hz(int hz);
	void set_mode(int mode);
	void set_bit_order(bool lsbFirst);

	void write(const uint8_t* data, uint16_t length);
	int  read(uint8_t* data, uint16_t length);
	int  transfer(const uint8_t* output, uint8_t* response, uint16_t length);

private:
	FT232H*     m_device;
	Pin         m_cs;
	int         m_write_clock_ve;
	int         m_read_clock_ve;
	bool        m_clock_base;
	int         m_lsbfirst;
};