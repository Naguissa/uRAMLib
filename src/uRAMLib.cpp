/**
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
 * \file uRAMLib.c
 */
#include <Arduino.h>
	#ifndef uRAMLIB_WIRE
		#if defined(ARDUINO_attiny) || defined(ARDUINO_AVR_ATTINYX4) || defined(ARDUINO_AVR_ATTINYX5) || defined(ARDUINO_AVR_ATTINYX7) || defined(ARDUINO_AVR_ATTINYX8) || defined(ARDUINO_AVR_ATTINYX61) || defined(ARDUINO_AVR_ATTINY43) || defined(ARDUINO_AVR_ATTINY828) || defined(ARDUINO_AVR_ATTINY1634) || defined(ARDUINO_AVR_ATTINYX313)
			#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI
			#define uRAMLIB_WIRE TinyWireM
		#else
			#include <Wire.h>
			#define uRAMLIB_WIRE Wire
		#endif
	#endif
#include "uRAMLib.h"

/**
 * \brief Constructor
 */
uRAMLib::uRAMLib() { }


/**
 * \brief Constructor
 *
 * @param skipInit Set true to skip Wire.init (needed for STM32, SAM and Arduino, at least)
 */
uRAMLib::uRAMLib(bool skipInit) {
	init = skipInit;
}




/**
 * \brief Constructor
 *
 * @param addr I2C address of (F)RAM
 */
uRAMLib::uRAMLib(const uint8_t addr) {
	_ram_address = addr;
}


/**
 * \brief Constructor
 *
 * @param skipInit Set true to skip Wire.init (needed for STM32, SAM and Arduino, at least)
 * @param addr I2C address of (F)RAM
 */
uRAMLib::uRAMLib(bool skipInit, const uint8_t addr) {
	init = skipInit;
	_ram_address = addr;
}


/**
 * \brief Sets (F)RAM i2c addres
 *
 * @param skipInit Set true to skip Wire.init (needed for STM32, SAM and Arduino, at least)
 * @param addr I2C address of (F)RAM
 */
void uRAMLib::set_address(const uint8_t addr) {
	_ram_address = addr;
}


/**
 * \brief Read one byte
 *
 * @param address Address inside (F)RAM to read from
 * @return read byte
 */
byte uRAMLib::read(const uint16_t address) {
    int ram_effective_address = _ram_address + (address >> 8); // address + MSB
	uRAMLIB_STM32_INIT_FIX()
	uRAMLIB_YIELD
	byte rdata = 0xFF;
	uRAMLIB_WIRE.beginTransmission(ram_effective_address);
	uRAMLIB_WIRE.write((int)(address & 0xFF)); // LSB
	if (uRAMLIB_WIRE.endTransmission() == 0) {
		uRAMLIB_WIRE.requestFrom(_ram_address, 1);
        delay(uRAMLIB_WIRE_SHORT_DELAY);
		if(uRAMLIB_WIRE.available()) {
			rdata = (byte) uRAMLIB_WIRE.read();
            delay(uRAMLIB_WIRE_SHORT_DELAY);
		}
	}
	uRAMLIB_YIELD
	return rdata;
}

/**
 * \brief Read sequence of n bytes.
 *
 * Public function, can read any arbitrary data length
 *
 * @param address Address inside (F)RAM to read from
 * @param data Pointer to where read data to
 * @param number of bytes to read
 * @return true if bytes read are the same as requested
 */
bool uRAMLib::read(const uint16_t address, byte *data, const uint16_t n) {
    uint32_t end = address + n;
    uint32_t act = (uint32_t) address;
    int ram_effective_address; // adress + MSB
    int n_remaining; // Length
    int current_address;
    int buffer_pos = 0;    
	byte temp = 0;
	uRAMLIB_STM32_INIT_FIX()

    while (act < end) {
        ram_effective_address = (int) (_ram_address + ((act >> 8) & 0xFF)); // adress + MSB
        current_address = (int) (act & 0xFF); // LSB
        n_remaining = (int) (((end - act - 1) % 256) + 1 - act % 256); // Align to 256bytes banks
	    uRAMLIB_YIELD
	    uRAMLIB_WIRE.beginTransmission(ram_effective_address);
	    uRAMLIB_WIRE.write(current_address); // LSB
        delay(uRAMLIB_WIRE_SHORT_DELAY);
	    if (uRAMLIB_WIRE.endTransmission() == 0) {
		    uRAMLIB_WIRE.requestFrom(ram_effective_address, n_remaining);
            delay(uRAMLIB_WIRE_SHORT_DELAY);
		    if(uRAMLIB_WIRE.available()) {
                for (int i = 0; i < n_remaining && uRAMLIB_WIRE.available(); i++, buffer_pos++) {
				    temp = (byte) uRAMLIB_WIRE.read();
                    *(data + buffer_pos) = temp;
                    delay(uRAMLIB_WIRE_SHORT_DELAY);
                	uRAMLIB_YIELD
				    // Added to wait if needed but cut after a failure (timeout)
				    if (i < n_remaining) {
    				    uint8_t j = 0;
	                	for (; j < 255 && !uRAMLIB_WIRE.available(); j++) {
                            delay(uRAMLIB_WIRE_SHORT_DELAY);
			            	uRAMLIB_YIELD
					    }
					    if (j == 255) {
					        return false;
				        }
				    }
                }
		    }
	    }
	    act += n_remaining;
    }
	uRAMLIB_YIELD
	return end == act;
}


/**
 * \brief Write a byte
 *
 * @param address initial addesss to write to
 * @param data byte to write (without offset)
 * @param n number of bytes to write
 * @return true if successful
 */
bool uRAMLib::write(const uint16_t address, const byte data) {
    int ram_effective_address = _ram_address + (address >> 8); // address + MSB
	uRAMLIB_YIELD
	uRAMLIB_WIRE.beginTransmission(ram_effective_address);
	uRAMLIB_WIRE.write((int)(address & 0xFF)); // LSB
	uRAMLIB_WIRE.write(data);
	uRAMLIB_YIELD
    delay(uRAMLIB_WIRE_SHORT_DELAY);
	return uRAMLIB_WIRE.endTransmission() == 0;
}


/**
 * \brief Write sequence of n bytes
 *
 * @param address initial addesss to write to
 * @param data pointer to data to write (without offset)
 * @param n number of bytes to write
 * @return true if successful
 */
bool uRAMLib::write(const uint16_t address, const byte *data, const uint16_t n) {
    uint32_t end = address + n;
    uint32_t act = (uint32_t) address;
    int ram_effective_address; // adress + MSB
    int n_remaining; // Length
    int current_address;
    int buffer_pos = 0;    
	uRAMLIB_STM32_INIT_FIX()

    while (act < end) {
        ram_effective_address = (int) (_ram_address + ((act >> 8) & 0xFF)); // adress + MSB
        current_address = (int) (act & 0xFF); // LSB
        n_remaining = (int) (((end - act - 1) % 256) + 1 - act % 256); // Align to 256bytes banks
	    uRAMLIB_YIELD
	    uRAMLIB_WIRE.beginTransmission(ram_effective_address);
	    uRAMLIB_WIRE.write(current_address); // LSB
        delay(uRAMLIB_WIRE_SHORT_DELAY);

        for (int i = 0; i < n_remaining && uRAMLIB_WIRE.available(); i++, buffer_pos++) {
		    uRAMLIB_WIRE.write(*(data + buffer_pos));
		    uRAMLIB_YIELD
        }
	    uRAMLIB_YIELD
	    if (uRAMLIB_WIRE.endTransmission() != 0) {
	        return false;
        }
	    act += n_remaining;
    }
	uRAMLIB_YIELD
	return end == act;
}



