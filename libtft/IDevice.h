#pragma once

#include <cstdint>
#include <initializer_list>

namespace hw
{
	enum class Pin
	{
		D0, D1, D2, D3, D4, D5, D6, D7,
		C0, C1, C2, C3, C4, C5, C6, C7,
	};

	enum class Direction
	{
		In, Out,
	};

	class IDevice
	{
	public:
		virtual ~IDevice() {}

		virtual bool open() = 0;
		virtual void close() = 0;

		// GPIO access.
		virtual void      setPinDirection(Pin pin, Direction dir) = 0;
		virtual Direction getPinDirection(Pin pin) = 0;

		virtual void      setPinValue(Pin pin, bool value) = 0;
		virtual bool      getPinValue(Pin pin) = 0;
		virtual uint16_t  readPins() = 0;

		inline void       setHigh(Pin pin) { setPinValue(pin, true); }
		inline void       setLow(Pin pin)  { setPinValue(pin, false); }
		inline bool       isHigh(Pin pin)  { return getPinValue(pin); }
		inline bool       isLow(Pin pin)   { return !getPinValue(pin); }

		// MPSSE access.
		virtual void      setClock(int clock_hz, bool adaptive = false, bool three_phase = false) = 0;
		virtual int       write(const uint8_t* data, size_t length) = 0;
		virtual int       read(uint8_t* data, int expected, int timeOutInMs = 500) = 0;

		int               writeByte(uint8_t data);
		int               writeUInt16(uint16_t data);
		int               writeList(const std::initializer_list<uint8_t>& list);
	};
}

