#pragma once

#include "IDevice.h"
#include <stdint.h>
#include <initializer_list>

struct ftdi_context;

namespace hw
{
	class FT232H sealed : public IDevice
	{
	public:
		FT232H();
		FT232H(FT232H&& lhs);
		~FT232H();

		FT232H(const FT232H&) = delete;
		FT232H& operator=(const FT232H&) = delete;
		FT232H& operator=(FT232H&& lhs);

		bool open() override;
		void close() override;

		// GPIO access.
		void      setPinDirection(Pin pin, Direction dir) override;
		Direction getPinDirection(Pin pin) override;

		void      setPinValue(Pin pin, bool value) override;
		bool      getPinValue(Pin pin) override;
		uint16_t  readPins() override;

		// MPSSE access.
		void      setClock(int clock_hz, bool adaptive = false, bool three_phase = false) override;
		int       write(const uint8_t* data, size_t length) override;
		int       read(uint8_t* data, int expected, int timeOutInMs = 500) override;

	private:
		void      mpsse_enable();
		void      mpsse_sync(int max_retries = 10);
		void      setupPin(Pin pin, Direction dir);
		void      outputPin(Pin pin, bool value);
		uint16_t  mpsse_read_gpio();
		void      mpsse_write_gpio();
		static uint16_t pinToMask(Pin pin);

	private:
		ftdi_context* m_ftdi;
		uint16_t      m_gpio_direction;
		uint16_t      m_gpio_values;
	};
}