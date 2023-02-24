#include <SPI.h>
#include <ADS1263.h>

const uint8_t ADCSPICSPin = 2;

bool EnableADC2 = false;
double ADC1Value = 0.0;
double ADC2Value = 0.0;

ADS1263 ADC(ADCSPICSPin);

void ADC1Callback()
{
	ADC.GetADC1Value(&ADC1Value);
	Serial.print("{\"ADC1\":");
	Serial.print(ADC1Value);
	Serial.println("}");
}

void ADC2Callback()
{
	ADC.GetADC2Value(&ADC2Value);
	Serial.print("{\"ADC2\":");
	Serial.print(ADC2Value);
	Serial.println("}");
}

void setup()
{
	ADC.SetNewADC1Callback(ADC1Callback);
	ADC.SetNewADC2Callback(ADC1Callback);
	ADC.Begin(EnableADC2);
	ADC.StartADC1();
	if (EnableADC2)
	{
		ADC.StartADC2();
	}
}

void loop()
{
	ADC.Check();
}
