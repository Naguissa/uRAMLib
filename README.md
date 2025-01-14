# Arduino, ESP8266, STM32, ESP32 and others uRAMLib


## What is this repository for?

I2C RAM/FRAM/MRAM library. Derived from uEEPROMLib https://github.com/Naguissa/uEEPROMLib , it maintains same operation principle.

This library controls any I2C RAM. It's based on [FM24CL16 FRAM](https://www.foroelectro.net/arduino/link/I2C-FRAM) but should work with any.

*Note: On ATTiny, you need TnyWireM library in order to compile. It's available on Library manager.


## uEEPROMLib vs uRAMLib comparation

Item | uEEPROMLib | uRAMLib
--- | --- | ---
Internal delay in I2C operation | 6ms | 1ms (may be 0)
Reading / Writing operation mode | Paged | Banked
Adressing | Direct (2 bytes) | Base + remaining (1+1 bytes)
  


## How do I get set up?

You can get it from Arduino libraries directly, searching by uRAMLib.

For manual installation:

 * Get the ZIP.
 * Rename to uRAMLib.zip
 * Install library on Arduino



## Documentation and extras

You can find all documentation and extras in this repository: https://github.com/Naguissa/uRAMLib_doc_and_extras

You can read documentation online here: https://naguissa.github.io/uRAMLib_doc_and_extras/



## Examples

Included on example folder, available on Arduino IDE.




## Who do I talk to?

 * [Naguissa](https://github.com/Naguissa)
 * https://www.foroelectro.net/
 * https://www.naguissa.com



## Contribute

Any code contribution, report or comment are always welcome. Don't hesitate to use GitHub for that.


 * You can sponsor this project using GitHub's Sponsor button: https://github.com/Naguissa/uRAMLib
 * You can make a donation via PayPal: https://paypal.me/foroelectro


Thanks for your support.


Contributors hall of fame: https://www.foroelectro.net/hall-of-fame-f32/contributors-contribuyentes-t271.html

