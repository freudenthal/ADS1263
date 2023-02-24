# An Arduino library for ADS1263

This library is for the [ADS1263 32-Bit 38-kSPS SPI analog-to-digital converter](https://www.ti.com/product/ADS1263).

## Compability

This library will likely work on most anything with the standard SPI.h includes. Was tested with [Teensy](https://www.pjrc.com/teensy/).

```
#include <SPI.h>
```

## Operation

The header breaks out the registers into classes with additional get/set functions for individual settings. Each function's return is a status bool with true for success, and registers are returned via setting the values of a pointer.

The intended use is to place the Begin() call in the start(), and run Check() in the loop(). Begin() sets all the register values to reasonable defaults, and can optionally enable the second ADC with Begin(true).

The library has a SetVerbose(bool) for full verbose output over Serial.

A callback can be set for either ADC with SetNewADC1Callback( void ( \*NewSampleListener )() ) or SetNewADC2Callback( void ( \*NewSampleListener )() ). The callback will fire with each successful read of the respective ADC. Data can then be retreived with bool GetADC1Value(int32_t\* DataToGet) or bool GetADC2Value(int32_t\* DataToGet) using a pointer to an int.
