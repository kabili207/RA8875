#include "FT232H.h"
#include <stdio.h>
#include <ftdi.h>

#include <chrono>
#include <thread>
#include <iso646.h>

namespace hw
{
	FT232H::FT232H()
		: m_ftdi(nullptr)
		, m_gpio_direction(0)
		, m_gpio_values(0)
	{
	}

	FT232H::FT232H(FT232H&& lhs)
		: m_ftdi(lhs.m_ftdi)
		, m_gpio_direction(lhs.m_gpio_direction)
		, m_gpio_values(lhs.m_gpio_values)
	{
		lhs.m_ftdi = nullptr;
	}


	FT232H::~FT232H()
	{
		close();
	}

	FT232H& FT232H::operator=(FT232H&& lhs) 
	{
		close();
		std::swap(m_ftdi, lhs.m_ftdi);
		m_gpio_direction = lhs.m_gpio_direction;
		m_gpio_values = lhs.m_gpio_values;
		return *this;
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
		writeList({ 0x80, 0, 0, 0x82, 0, 0 });
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

	void FT232H::setPinDirection(Pin pin, Direction mode)
	{
		setupPin(pin, mode);
		mpsse_write_gpio();
	}

	Direction FT232H::getPinDirection(Pin pin)
	{
		bool value = (m_gpio_direction & pinToMask(pin)) != 0;
		return value ? Direction::Out : Direction::In;
	}

	void FT232H::setPinValue(Pin pin, bool value)
	{
		outputPin(pin, value);
		mpsse_write_gpio();
	}

	bool FT232H::getPinValue(Pin pin)
	{
		return (readPins() & pinToMask(pin)) != 0;
	}

	uint16_t FT232H::readPins()
	{
		m_gpio_values = mpsse_read_gpio();
		return m_gpio_values;
	}

	/// 
	/// Set the clock speed of the MPSSE engine.  Can be any value from 450hz
	/// to 30mhz and will pick that speed or the closest speed below it.
	/// 
	void FT232H::setClock(int clock_hz, bool adaptive, bool three_phase)
	{
		// Disable clock divisor by 5 to enable faster speeds on FT232H.
		writeByte(0x8A);
		// Turn on / off adaptive clocking.
		if (adaptive)
			writeByte(0x96);
		else
			writeByte(0x97);

		// Turn on / off three phase clock(needed for I2C).
		// Also adjust the frequency for three - phase clocking as specified in section 2.2.4
		// of this document:
		//   http://www.ftdichip.com/Support/Documents/AppNotes/AN_255_USB%20to%20I2C%20Example%20using%20the%20FT232H%20and%20FT201X%20devices.pdf
		if (three_phase)
			writeByte(0x8C);
		else
			writeByte(0x8D);

		// Compute divisor for requested clock.
		// Use equation from section 3.8.1 of:
		//  http://www.ftdichip.com/Support/Documents/AppNotes/AN_108_Command_Processor_for_MPSSE_and_MCU_Host_Bus_Emulation_Modes.pdf
		// Note equation is using 60mhz master clock instead of 12mhz.
		int divisor = int(ceil((30000000.0 - float(clock_hz)) / float(clock_hz))) & 0xFFFF;
		if (three_phase)
			divisor = int(divisor*(2.0 / 3.0));

		// Send command to set divisor from low and high byte values.
		writeByte(0x86);
		writeUInt16(uint16_t(divisor));
	}

	int FT232H::write(const uint8_t* data, size_t length)
	{
		return ftdi_write_data(m_ftdi, data, static_cast<int>(length));
	}

	int FT232H::read(uint8_t* data, int expected, int timeOutInMs)
	{
		int index = 0;
		auto start = std::chrono::high_resolution_clock::now();
		while ((std::chrono::high_resolution_clock::now() - start) < std::chrono::milliseconds(timeOutInMs))
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


	/// Read both GPIO bus states and return a 16 bit value with their state.
	/// D0 - D7 are the lower 8 bits and C0 - C7 are the upper 8 bits.
	///
	uint16_t FT232H::mpsse_read_gpio()
	{
		// Send command to read low byte and high byte.
		writeList({ 0x81, 0x83 });

		// Wait for 2 byte response.
		uint8_t data[2];
		read(data, 2);

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

		writeList({ 0x80, level_low, dir_low, 0x82, level_high, dir_high });
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
		writeByte(0xAB);

		// Keep reading until bad command response(0xFA 0xAB) is returned.
		// Fail if too many read attempts are made to prevent sticking in a loop.
		int tries = 0;
		bool sync = false;
		while (!sync)
		{
			uint8_t data[2];
			read(data, 2);
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

	void FT232H::setupPin(Pin pin, Direction dir)
	{
		if (dir == Direction::In)
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

	void FT232H::outputPin(Pin pin, bool value)
	{
		if (value)
			m_gpio_values |= pinToMask(pin);
		else
			m_gpio_values &= ~pinToMask(pin);
	}

	uint16_t FT232H::pinToMask(Pin pin)
	{
		return uint16_t(1 << static_cast<int>(pin));
	}
}