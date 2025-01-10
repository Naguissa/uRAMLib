/**
 * I2C RAM/FRAM/MRAM library, derived from uEEPROMLib
 *
 * This library controls any I2C RAM/FRAM/MRAM. Based on FM24CL16 FRAM.
 *
 * @copyright Naguissa
 * @author Naguissa
 * @email naguissa@foroelectro.net
 * @url https://www.foroelectro.net/
 * @url https://github.com/Naguissa/uRAMLib
 * @version 1.0.0
 */
#include "Arduino.h"
#include "uRAMLib.h"

#if defined(ARDUINO_attiny) || defined(ARDUINO_AVR_ATTINYX4) || defined(ARDUINO_AVR_ATTINYX5) || defined(ARDUINO_AVR_ATTINYX7) || defined(ARDUINO_AVR_ATTINYX8) || defined(ARDUINO_AVR_ATTINYX61) || defined(ARDUINO_AVR_ATTINY43) || defined(ARDUINO_AVR_ATTINY828) || defined(ARDUINO_AVR_ATTINY1634) || defined(ARDUINO_AVR_ATTINYX313)
    #include <SoftwareUsedSerial.h>
    const int rx=3;
    const int tx=4;
    SoftwareSerial UsedSerial(rx,tx);
    #include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI
    #define WIRE TinyWireM
#else
    #define UsedSerial Serial
    #include <Wire.h>
    #define WIRE Wire
#endif


// uRAMLib object
uRAMLib ram((uint8_t) 0x50);


void setup() {
	delay (1500);
	UsedSerial.begin(115200);
	UsedSerial.println("Serial OK");
	delay (1500);


	// +256 bytes
	char longMessage[350] = "I2C RAM/FRAM/MRAM/SRAM library example. Derived from uEEPROMLib\n\nThis library controls any I2C RAM. Based on FM24CL16 FRAM uses banked I2C adresses and fast access. Accepts any address+size combination controlling memory banks transparently.\n\nhttps://github.com/Naguissa/uRAMLib\nhttps://www.foroelectro.net\0";
	int longMessageLen = strlen(longMessage);

	#ifdef ARDUINO_ARCH_ESP8266
		WIRE.begin(4, 5); // D3 and D4 on ESP8266
	#else
		WIRE.begin();
	#endif

	byte 	    random_numbers[300]; // write 300 bytes of random data
	uint16_t 	random_number = 0;
	uint16_t 	running_total = 0;
    int       start_address = 123; // random
	UsedSerial.println("Performing write test and verify with library - banked writes (256 bytes per bank)");

	for (int i = 0; i < 300; i++)
	{
		random_number = rand() % 10 + 1; // range from 1 to 10
		random_numbers[i] = random_number;
		running_total += random_number;
	}

	UsedSerial.print("Magic number: "); UsedSerial.println(running_total, DEC);

	UsedSerial.println("Writing to RAM.");
	if (!ram.write(start_address, (byte *) random_numbers, 300)) {
		UsedSerial.println("Failed to write to RAM!");
	}

	delay(1000);
	
	random_number = 0;
	memset(random_numbers,0, 300);	// Flush back to zero
	UsedSerial.println("Reading from RAM....");
	ram.read(start_address, (byte *) random_numbers, 300);

	for (int i = 0; i < 300; i++) {
        // UsedSerial.print("Read from magic number array value: ");
        // UsedSerial.println (random_numbers[i], DEC);
		random_number += random_numbers[i];
	}

	UsedSerial.print("Calculated magic number: ");
	UsedSerial.println(random_number, DEC);

	if ( random_number == running_total) {
    	UsedSerial.println("Magic numbers match. RAM read successful.");
	} else {
		UsedSerial.println("==> VALIDATION ERROR <==");
	}



	// Bank aligned String vs Unaligned
	
	// Bank aligned String
	if (!ram.write(0, (byte *) longMessage, longMessageLen)) {
		UsedSerial.println("Failed to store aligned STRING");
	} else {
		UsedSerial.println("Aligned STRING correctly stored");
	}

    memset(longMessage,0, sizeof(longMessage)); // Flush back to zero
    UsedSerial.println("-------------------------------------");
    UsedSerial.println();

    UsedSerial.println("== Bank Aligned string (offset: 0):");
    ram.read(0, (byte *) longMessage, longMessageLen);
    UsedSerial.println(longMessage);


	// Bank unaligned String - This will read junk if previous test failed.
	if (!ram.write(123, (byte *) longMessage, longMessageLen)) {
		UsedSerial.println("Failed to store unaligned STRING");
	} else {
		UsedSerial.println("Unaligned STRING correctly stored");
	}

    memset(longMessage,0, longMessageLen); // Flush back to zero
    UsedSerial.println("-------------------------------------");
    UsedSerial.println();

    UsedSerial.println("== Bank Unaligned string (offset: 123):");
    ram.read(123, (byte *) longMessage, longMessageLen);
    UsedSerial.println(longMessage);

    UsedSerial.println();

    UsedSerial.println("-------------------------------------");
    UsedSerial.println("-------         END         ---------");
    UsedSerial.println("-------------------------------------");
}

void loop() {


}
