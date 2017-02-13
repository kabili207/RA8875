#include "FT232H.h"
#include <stdio.h>
#include <ftdi.h>

#include <chrono>
#include <thread>
#include <iso646.h>

FT232H::FT232H()
	: m_ftdi(nullptr)
{
}


FT232H::~FT232H()
{
	close();
}


bool FT232H::open()
{
	m_ftdi = ftdi_new();
	if (m_ftdi == nullptr)
	{
		fprintf(stderr, "ftdi_new failed\n");
		return false;
	}

	auto ret = ftdi_usb_open(m_ftdi, 0x0403, 0x6014);
	if (ret < 0)
	{
		fprintf(stderr, "Unable to open ftdi device: %d (%s)\n", ret, ftdi_get_error_string(m_ftdi));
		return false;
	}

	// Reset device.
	ftdi_usb_reset(m_ftdi);

	// Change read & write buffers to maximum size, 65535 bytes.
	ftdi_read_data_set_chunksize(m_ftdi, 65535);
	ftdi_write_data_set_chunksize(m_ftdi, 65535);

	// Clear pending read data & write buffers.
	ftdi_usb_purge_buffers(m_ftdi);

	// Enable MPSSE and syncronize communication with device.
	mpsse_enable();
	mpsse_sync();

	// Initialize all GPIO as inputs.
	write({ 0x80, 0, 0, 0x82, 0, 0 });
	m_gpio_direction = 0x0000;
	m_gpio_values = 0x0000;
	return true;
}


void FT232H::close()
{
	if (m_ftdi != nullptr)
	{
		ftdi_usb_close(m_ftdi);
		ftdi_free(m_ftdi);
		m_ftdi = nullptr;
	}
}

/// 
/// Set the clock speed of the MPSSE engine.  Can be any value from 450hz
/// to 30mhz and will pick that speed or the closest speed below it.
/// 
void FT232H::mpsse_set_clock(int clock_hz, bool adaptive, bool three_phase)
{
	// Disable clock divisor by 5 to enable faster speeds on FT232H.
	write(0x8A);
	// Turn on / off adaptive clocking.
	if (adaptive)
		write(0x96);
	else
		write(0x97);

	// Turn on / off three phase clock(needed for I2C).
	// Also adjust the frequency for three - phase clocking as specified in section 2.2.4
	// of this document:
	//   http ://www.ftdichip.com/Support/Documents/AppNotes/AN_255_USB%20to%20I2C%20Example%20using%20the%20FT232H%20and%20FT201X%20devices.pdf
	if (three_phase)
		write(0x8C);
	else
		write(0x8D);

	// Compute divisor for requested clock.
	// Use equation from section 3.8.1 of:
	//  http ://www.ftdichip.com/Support/Documents/AppNotes/AN_108_Command_Processor_for_MPSSE_and_MCU_Host_Bus_Emulation_Modes.pdf
	// Note equation is using 60mhz master clock instead of 12mhz.
	int divisor = int(ceil((30000000.0 - float(clock_hz)) / float(clock_hz))) & 0xFFFF;
	if (three_phase)
		divisor = int(divisor*(2.0 / 3.0));

	// Send command to set divisor from low and high byte values.
	write(0x86);
	write(uint8_t(divisor & 0xFF));
	write(uint8_t(divisor >> 8));
}


/// Read both GPIO bus states and return a 16 bit value with their state.
/// D0 - D7 are the lower 8 bits and C0 - C7 are the upper 8 bits.
///
uint16_t FT232H::mpsse_read_gpio()
{
	// Send command to read low byte and high byte.
	write({ 0x81, 0x83 });

	// Wait for 2 byte response.
	uint8_t data[2];
	_poll_read(data, 2);

	// return result;
	return (data[1] << 8) | data[0];
}

/// Write the current MPSSE GPIO state to the FT232H chip.
///
void FT232H::mpsse_write_gpio()
{
	uint8_t level_low = m_gpio_values & 0xff;
	uint8_t level_high = m_gpio_values >> 8;
	uint8_t dir_low = m_gpio_direction & 0xff;
	uint8_t dir_high = m_gpio_direction >> 8;

	write({ 0x80, level_low, dir_low, 0x82, level_high, dir_high });
}


void FT232H::setup(Pin pin, Mode mode)
{
	_setup_pin(pin, mode);
	mpsse_write_gpio();
}

void FT232H::output(Pin pin, bool value)
{
	_output_pin(pin, value);
	mpsse_write_gpio();
}

bool FT232H::input(Pin pin)
{
	m_gpio_values = mpsse_read_gpio();
	return (m_gpio_values & pinToMask(pin)) != 0;
}

int FT232H::write(uint8_t data)
{
	return ftdi_write_data(m_ftdi, &data, 1);
}

int FT232H::write(const uint8_t* data, size_t length)
{
	return ftdi_write_data(m_ftdi, data, static_cast<int>(length));
}

int FT232H::write(const std::initializer_list<uint8_t>& list)
{
	return write(list.begin(), list.size());
}

int FT232H::_poll_read(uint8_t* data, int expected, int timeout_s)
{
	int index = 0;
	auto start = std::chrono::high_resolution_clock::now();
	while ((std::chrono::high_resolution_clock::now() - start) < std::chrono::seconds(timeout_s))
	{
		int ret = ftdi_read_data(m_ftdi, data + index, expected - index);
		if (ret < 0)
		{
			fprintf(stderr, "Unable to read ftdi device: %d (%s)\n", ret, ftdi_get_error_string(m_ftdi));
			return -1;
		}

		index += ret;

		if (index >= expected)
		{
			return expected;
		}

		std::this_thread::yield();
	}

	return index;
}

void FT232H::mpsse_enable()
{
	// Reset MPSSE by sending mask = 0 and mode = 0
	ftdi_set_bitmode(m_ftdi, 0, 0);
	// Enable MPSSE by sending mask = 0 and mode = 2
	ftdi_set_bitmode(m_ftdi, 0, 2);
}

void FT232H::mpsse_sync(int max_retries)
{
	// Send a bad / unknown command(0xAB), then read buffer until bad command 
	// response is found.
	write(0xAB);

	// Keep reading until bad command response(0xFA 0xAB) is returned.
	// Fail if too many read attempts are made to prevent sticking in a loop.
	int tries = 0;
	bool sync = false;
	while (!sync)
	{
		uint8_t data[2];
		_poll_read(data, 2);
		if ((data[0] == 0xFA) && (data[1] == 0xAB)) {
			sync = true;
		}
		tries += 1;
		if (tries >= max_retries) {
			fprintf(stderr, "'Could not synchronize with FT232H!");
			exit(-1);
		}
	}
}

void FT232H::_setup_pin(Pin pin, Mode mode)
{
	if (mode == Mode::In)
	{
		// Set the direction and level of the pin to 0.
		m_gpio_direction &= ~pinToMask(pin);
		m_gpio_values &= ~pinToMask(pin);
	}
	else
	{
		// Set the direction of the pin to 1.
		m_gpio_direction |= pinToMask(pin);
	}
}

void FT232H::_output_pin(Pin pin, bool value)
{
	if (value)
		m_gpio_values |= pinToMask(pin);
	else
		m_gpio_values &= ~pinToMask(pin);
}

uint16_t FT232H::pinToMask(Pin pin) const
{
	return uint16_t(1 << static_cast<int>(pin));
}


SPI::SPI(FT232H* device, Pin cs, int max_speed_hz, int mode, bool lsbFirst)
	: m_device(device)
	, m_cs(cs)
	, m_write_clock_ve()
	, m_read_clock_ve()
	, m_clock_base()
	, m_lsbfirst(0)
{
	// Initialize chip select pin if provided to output high.
	m_device->setup(cs, Mode::Out);
	m_device->set_high(m_cs);

	// Initialize clock, mode, and bit order.
	set_clock_hz(max_speed_hz);
	set_mode(mode);
	set_bit_order(lsbFirst);
}

SPI::~SPI()
{
}

///
/// Set the speed of the SPI clock in hertz.  Note that not all speeds
/// are supported and a lower speed might be chosen by the hardware.
///
void SPI::set_clock_hz(int hz)
{
	m_device->mpsse_set_clock(hz);
}


///
/// Set SPI mode which controls clock polarity and phase.  Should be a
/// numeric value 0, 1, 2, or 3.  See wikipedia page for details on meaning:
/// http://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus
///
void SPI::set_mode(int mode)
{
	// Set clock and DO as output, DI as input.
	m_device->setup(Pin::D0, Mode::Out);
	m_device->setup(Pin::D1, Mode::Out);
	m_device->setup(Pin::D2, Mode::In);

	if (mode == 0)
	{
		// Mode 0 captures on rising clock, propagates on falling clock
		m_write_clock_ve = 1;
		m_read_clock_ve = 0;
		// Clock base is low.
		m_device->output(Pin::D0, false);
	}
	else if (mode == 1)
	{
		// Mode 1 capture of falling edge, propagate on rising clock
		m_write_clock_ve = 0;
		m_read_clock_ve = 1;
		// Clock base is low.
		m_device->output(Pin::D0, false);
	}
	else if (mode == 2)
	{
		// Mode 2 capture on rising clock, propagate on falling clock
		m_write_clock_ve = 1;
		m_read_clock_ve = 0;
		// Clock base is high.
		m_device->output(Pin::D0, true);
	}
	else if (mode == 3)
	{
		// Mode 3 capture on falling edge, propagage on rising clock
		m_write_clock_ve = 0;
		m_read_clock_ve = 1;
		// Clock base is high.
		m_device->output(Pin::D0, true);
	}
}


/// 
/// Set order of bits to be read/written over serial lines.  Should be
/// either MSBFIRST for most - significant first, or LSBFIRST for
/// least - signifcant first.
/// 
void SPI::set_bit_order(bool lsbFirst)
{
	m_lsbfirst = lsbFirst ? 1 : 0;
}

/// 
/// Half-duplex SPI write.  The specified array of bytes will be clocked
/// out the MOSI line.
/// 
void SPI::write(const uint8_t* data, uint16_t length)
{
	// Build command to read and write SPI data.
	uint8_t command = uint8_t(0x10 | (m_lsbfirst << 3) | m_write_clock_ve);
	uint8_t len_low = uint8_t((length - 1) & 0xff);
	uint8_t len_high = uint8_t((length - 1) >> 8);

	// execute.
	m_device->set_low(m_cs);
	m_device->write(command);
	m_device->write(len_low);
	m_device->write(len_high);
	m_device->write(data, length);
	m_device->write(0x87);
	m_device->set_high(m_cs);
}

/// 
/// Half-duplex SPI read.  The specified length of bytes will be clocked
/// in the MISO line and returned as a bytearray object.
/// 
int SPI::read(uint8_t* data, uint16_t length)
{
	// Build command to read and write SPI data.
	uint8_t command = uint8_t(0x20 | (m_lsbfirst << 3) | (m_read_clock_ve << 2));
	uint8_t len_low = uint8_t((length - 1) & 0xff);
	uint8_t len_high = uint8_t((length - 1) >> 8);

	// execute.
	m_device->set_low(m_cs);
	m_device->write(command);
	m_device->write(len_low);
	m_device->write(len_high);
	m_device->write(0x87);
	m_device->set_high(m_cs);

	return m_device->_poll_read(data, length);
}

/// 
/// Full-duplex SPI read and write.  The specified array of bytes will be
/// clocked out the MOSI line, while simultaneously bytes will be read from
/// the MISO line.Read bytes will be returned as a bytearray object.
///
int SPI::transfer(const uint8_t* output, uint8_t* response, uint16_t length)
{
	// Build command to read and write SPI data.
	uint8_t command = uint8_t(0x30 | (m_lsbfirst << 3) | (m_read_clock_ve << 2) | m_write_clock_ve);
	uint8_t len_low = uint8_t((length - 1) & 0xff);
	uint8_t len_high = uint8_t((length - 1) >> 8);

	// execute.
	m_device->set_low(m_cs);
	m_device->write(command);
	m_device->write(len_low);
	m_device->write(len_high);
	m_device->write(output, length);
	m_device->write(0x87);
	m_device->set_high(m_cs);

	return m_device->_poll_read(response, length);
}