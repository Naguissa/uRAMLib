/**
 * \mainpage
 * \class uRAMLib
 * \brief I2C RAM/FRAM/MRAM library, derived from uEEPROMLib
 *
 * This library controls any I2C RAM/FRAM/MRAM. Based on FM24CL16 FRAM.
 *
 * @see <a href="https://github.com/Naguissa/uRAMLib">https://github.com/Naguissa/uRAMLib</a>
 * @see <a href="mailto:naguissa@foroelectro.net">naguissa@foroelectro.net</a>
 * @see <a href="https://github.com/Naguissa/uEEPROMLib">https://github.com/Naguissa/uEEPROMLib</a>
 *
 * @version 1.0.0
 * \file uRAMLib.h
 */
#ifndef uRAMLIB
	/**
	 * \brief Prevent multiple inclussion
	 */
	#define uRAMLIB
	#include "Arduino.h"
	#ifndef uRAMLIB_WIRE
		#if defined(ARDUINO_attiny) || defined(ARDUINO_AVR_ATTINYX4) || defined(ARDUINO_AVR_ATTINYX5) || defined(ARDUINO_AVR_ATTINYX7) || defined(ARDUINO_AVR_ATTINYX8) || defined(ARDUINO_AVR_ATTINYX61) || defined(ARDUINO_AVR_ATTINY43) || defined(ARDUINO_AVR_ATTINY828) || defined(ARDUINO_AVR_ATTINY1634) || defined(ARDUINO_AVR_ATTINYX313)
			#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI
			#define uRAMLIB_WIRE TinyWireM
		#else
			#include <Wire.h>
			#define uRAMLIB_WIRE Wire
		#endif
	#endif

	/**
	 * \brief Default (F)RAM address
	 *
	 * Base address for RAM module. FM24CL16 address range is 0x50 .. 0x57
	 */
	#define uRAMLIB_ADDRESS 0x50

	// ESP yield function (ESP32 has no need for that on dual core, but it has on single core version)
	#if ARDUINO_ARCH_ESP8266
		/**
		 * \brief ESP8266, yield to don't block ESP functionality.
		 *
		 * When this library is used in other MCUs this is simply removed by the preprocessor
		 */
		#define uRAMLIB_YIELD yield();
	#else
		#if ARDUINO_ARCH_ESP32
		/**
		 * \brief ESP32, yield to don't block ESP functionality.
		 *
		 * When this library is used in other MCUs this is simply removed by the preprocessor
		 */
			#define uRAMLIB_YIELD yield();
		#else
			#define uRAMLIB_YIELD
		#endif
	#endif

	/**
	 * \brief Very short delay to let I2C bus to process data
	 */
    #define uRAMLIB_WIRE_SHORT_DELAY 1

	// BUFFER_LENGTH should come from Wire library
	 #ifdef BUFFER_LENGTH
	/**
	 * \brief Max buffer length, taken from Wire library if defined
	 */
		#define uRAMLIB_WIRE_MAX_RBUFFER (BUFFER_LENGTH - 2)
	#else
	/**
	 * \brief Max buffer length, failsafe if not defined in Wire library
	 */
		#define uRAMLIB_WIRE_MAX_RBUFFER 32
	#endif
		#define uRAMLIB_WIRE_MAX_WBUFFER (uRAMLIB_WIRE_MAX_RBUFFER - 2)


	class uRAMLib {
		public:
			// Constructors
			uRAMLib();
			uRAMLib(const uint8_t);
			uRAMLib(bool);
			uRAMLib(bool, const uint8_t);

			void set_address(const uint8_t);
			
			// (F)RAM read functions
			byte read(const uint16_t);
			bool read(const uint16_t, byte *, const uint16_t);
			template <typename TR> void read(const uint16_t, TR *);

			// (F)RAM write functions
			bool write(const uint16_t, const byte);
			bool write(const uint16_t, const byte *, const uint16_t);
 			template <typename TW> bool write(const uint16_t, const TW);

		private:
			// Addresses
			int _ram_address = uRAMLIB_ADDRESS;

			// Fix for 1st write error
			bool init = false;
			#define uRAMLIB_STM32_INIT_FIX() { if (!init) { init = true;  read(0); delay(10); } }
	};


	// Templates must be here because Arduino compiler incompatibility to declare them on .cpp file


	/**
	 * \brief Write any datatype to (F)RAM address
	 *
	 * @param address Address inside (F)RAM to write to
	 * @param data data to write (any type)
	 * @return false on error
	 */
	template <typename TW> bool uRAMLib::write(const uint16_t address, const TW data) {
		return write(address, (byte *) &data, (uint16_t) sizeof(TW));
	}


	/**
	 * \brief Read any datatype from (F)RAM address
	 *
	 * @param address Address inside (F)RAM to read from
	 * @return read data (any type)
	 */
	template <typename TR> void uRAMLib::read(const uint16_t address, TR *data) {
		read(address, (byte *) data, (uint16_t) sizeof(TR));
	}
#endif


