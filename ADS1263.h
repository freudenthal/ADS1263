#ifndef __ARDUINO_ADS1263
#define __ARDUINO_ADS1263

#include <Arduino.h>
#include <SPI.h>

class ADS1263
{
	public:
		enum class StatusCode : uint8_t
		{
			RESET = 0, //High => Device reset has occured
			PGAD_ALM = 1, //High => PGA differential alarm
			PGAH_ALM = 2, //High => PGA output high alarm
			PGAL_ALM = 3, //High => PGA output low alarm
			REF_ALM = 4, //High => Reference voltage low alarm
			EXTCLK = 5, //High => Using external clock
			ADC1 = 6, //High => New data read in ADC1
			ADC2 = 7, //High => New data read in ADC2
		};
		enum class OpCodesSimple : uint8_t
		{
			NOP = 0x00, //No operation
			RESET = 0x06, //Reset ADC
			START1 = 0x08, //Start conversion on ADC1
			STOP1 = 0x0A, //Stop conversion on ADC1
			START2 = 0x0C, //Start conversion on ADC2
			STOP2 = 0x0E, //Stop conversion on ADC2
			RDATA1 = 0x12, //Continous conversion on ADC1
			RDATA2 = 0x14, //Continous conversion on ADC2
			SYOCAL1 = 0x16, //ADC1 system offset calibration
			SYGCAL1 = 0x17, //ADC1 system gain calibration
			SFOCAL1 = 0x18, //ADC1 self offset calibration
			SYOCAL2 = 0x1B, //ADC2 system offset calibration
			SYGCAL2 = 0x1C, //ADC2 system gain calibration
			SFOCAL2 = 0x1E, //ADC2 self offset calibration
		};
		enum class OpCodesComplex : uint8_t
		{
			RREG = 0x20, //Read register (B 001r rrrr) where rrrrr are the read register, second opcode byte is N-1, N is number of bytes
			WREG = 0x40, //Write register (B 010r rrrr) where rrrrr are the write register, second opcode byte is N-1, N is number of bytes
		};
		enum class RegisterMap : uint8_t
		{
			ID = 0x00, //Identification register, bit 5 is 0 for ADS1262 or 1 for ADS1263, bits 0 to 4 are revision numbers
			POWER = 0x01,
			INTERFACE = 0x02,
			MODE0 = 0x03,
			MODE1 = 0x04,
			MODE2 = 0x05,
			INPMUX = 0x06,
			OFCAL0 = 0x07,
			OFCAL1 = 0x08,
			OFCAL2 = 0x09,
			FSCAL0 = 0x0A,
			FSCAL1 = 0x0B,
			FSCAL2 = 0x0C,
			IDACMUX = 0x0D,
			IDACMAG = 0x0E,
			REFMUX = 0x0F,
			TDACP = 0x10,
			TDACN = 0x11,
			GPIOCON = 0x12,
			GPIODIR = 0x13,
			GPIODAT = 0x14,
			ADC2CFG = 0x15,
			ADC2MUX = 0x16,
			ADC2OFC0 = 0x17,
			ADC2OFC1 = 0x18,
			ADC2FSC0 = 0x19,
			ADC2FSC1 = 0x1A,
		};
		enum class PowerRegMap : uint8_t
		{
			INTREF = 0, //R/W Internal reference enabled, enabled by default
			VBIAS = 1, //R/W Level shift voltage enabled, disabled by default
			RESET = 4, //R Indicates a reset has occured
		};
		enum class InterfaceRegMap : uint8_t
		{
			CHECKSUM = 0, //R/W Use checksum in Checksum mode, enabled by default. Do not enable both CRC and CHECKSUM.
			CHECKCRC = 1, //R/W Use CRC in Checksum mode, disabled by default. Do not enable both CRC and CHECKSUM.
			STATUS = 2, //R/W Use Status byte, enabled by default
			TIMEOUT = 3, //R/W Enable SPI timeout, disabled by default
		};
		enum class Mode0RegMap : uint8_t
		{
			DELAY = 0, //R/W 4 bits of conversion delay, no delay by default (B 0000)
			CHOP = 4, //R/W Input chop enabled, disabled by default
			IDAC = 5, //R/W IDAC rotation enabled, disabled by default
			RUNMODE = 6, //R/W High for pulse conversion, low for continous, continous by default
			REFREV = 7, //R/W Normal polarity of reference multiplexer, high to reverse, normal by default
		}
		enum class DelayValue : uint8_t //Delay settings
		{
			NODELAY = 0,
			uS9 = 1, //8.9uS delay
			uS17 = 2, //17uS delay
			uS35 = 3,
			uS69 = 4,
			uS139 = 5,
			uS278 = 6,
			uS555 = 7,
			uS1100 = 8,
			uS2200 = 9,
			uS4400 = 10,
			uS8800 = 11,
		};
		enum class Mode1RegMap : uint8_t
		{
			SBMAG = 0, //R/W 3 bits to select sensor bias magnitude, 000 for no bias, no bias as default
			SBPOL = 3, //R/W Low for pull-up mode, high for pull-down, default to pull-up mode
			SBADC = 4, //R/W Low for sensor bias to ADC1 mux out, high for ADC2, default to ADC1
			FILTER = 5, //R/W 3 bits to select filter. 100 for FIR filter, lower for SINC folters, deafult to FIR
		};
		enum class SensorBiasValue : uint8_t //Sensor bias magnitude settings
		{
			NOBIAS = 0,
			uA0p5 = 1, //0.5uA
			uA2 = 2, //2.0uA
			uA10 = 3, //10uA
			uA50 = 4, //50uA
			uA200 = 5, //200uA
			MOhm10 = 6, //10Mohm resistor
		};
		enum class FilterValue : uint8_t //Filter values for Mode1 FILTER register
		{
			SINC1 = 0,
			SINC2 = 1,
			SINC3 = 2,
			SINC4 = 3,
			FIR = 4,
		};
		enum class Mode2RegMap : uint8_t
		{
			DATARATE = 0, //R/W 4 bits to select data rate, default to 20 samples per second
			GAIN = 4, //R/W 3 bits to select gain, defaults to 1 V/V
			BYPASS = 7, //R/W High to bypass programmable gain amplifier, default to PGA enabled
		};
		enum class ADC1DataRateValue : uint8_t //Data rate settings for Mode2 DATARATE
		{
			SPS3 = 0, //2.5 samples per second
			SPS5 = 1, //5 samples per second
			SPS10 = 2,
			SPS17 = 3,
			SPS20 = 4, //20 samples per second, highest allowed for FIR filter
			SPS50 = 5,
			SPS60 = 6,
			SPS100 = 7,
			SPS400 = 8,
			SPS1200 = 9,
			SPS2400 = 10,
			SPS4800 = 11,
			SPS7200 = 12,
			SPS14400 = 13,
			SPS19200 = 14,
			SPS38400 = 15,
		};
		enum class ADC1GainValue: uint8_t //Gain settigns for ADC1
		{
			V1V = 0, // 1V/V, unity gain
			V2V = 1, // 2V/V, 2x gain
			V4V = 2, // 4V/V, 4x gain
			V8V = 3, // 8V/V, 8x gain
			V16V = 4,
			V32V = 5,
		};
		enum class InputMUXRegMap : uint8_t
		{
			MUXN = 0, //R/W, 4 bits, negative input multiplexer, default to AIN1
			MUXP = 4, //R/W, 4 bits, positive input multiplecer, default to AIN0
		};
		enum class InputMUXValue : uint8_t //4 bit input mux values for InputMUXRegMap MUXN, MUXP, MUXN2, and MUXP2
		{
			AIN0 = 0,
			AIN1 = 1,
			AIN2 = 2,
			AIN3 = 3,
			AIN4 = 4,
			AIN5 = 5,
			AIN6 = 6,
			AIN7 = 7,
			AIN8 = 8,
			AIN9 = 9,
			AINCOM = 10,
			TEMP = 11, //Temperature monitor negative/positive
			APOWER = 12, //Analog power supply rail negative/positive
			DPOWER = 13, //Digital power supply rail negative/positive
			TDAC = 14, //TDAC test signal negative/positive
			OPEN = 15, //Float open connection
		};
		enum class IDACMUXRegMap : uint8_t
		{
			MUX1 = 0, //R/W, 4 bits, Selects the analogue input to connect to IDAC1, default no connection
			MUX2 = 4, //R/W, 4 bits, Selects the analogue input to connect to IDAC2, default no connection
		};
		enum class IDACMUXValue : uint8_t //4 bit values for IDACMUXRegMap MUX1 and MUX2
		{
			AIN0 = 0,
			AIN1 = 1,
			AIN2 = 2,
			AIN3 = 3,
			AIN4 = 4,
			AIN5 = 5,
			AIN6 = 6,
			AIN7 = 7,
			AIN8 = 8,
			AIN9 = 9,
			AINCOM = 10,
			NOCONNECT = 11,
		};
		enum class IDACMAGRegMap : uint8_t
		{
			MAG1 = 0, //R/W, 4 bits, Selects the current magnitude for IDAC1, defaults to zero
			MAG2 = 4, //R/W, 4 bits, Selects the current magnitude for IDAC2, defaults to zero
		}
		enum class IDACMAGValue : uint8_t //4 bit values for IDACMAGRegMap MAG1 and MAG2
		{
			uA0 = 0,
			uA50 = 1,
			uA100 = 2,
			uA250 = 3,
			uA500 = 4,
			uA750 = 5,
			uA1000 = 6,
			uA1500 = 7,
			uA2000 = 8,
			uA2500 = 9,
			uA3000 = 10,
		};
		enum class REFMUXRegMap : uint8_t
		{
			RMUXN = 0, //R/W, 3 bits, reference negative input, default to internal 2.5V negative
			RMUXP = 3, //R/W, 3 bits, reference positive input, default to internal 2.5V positive
		}
		enum class REFMUXValue : uint8_t //3 bit values for REFMUXRegMap RMUXN and RMUXP
		{
			INTERNAL = 0, //Internal 2.5V positive/negative reference
			EXTAIN01 = 1, //External, use AIN0 for positive, AIN1 for negative
			EXTAIN23 = 2, //External, use AIN2 for positive, AIN3 for negative
			EXTAIN45 = 3, //External, use AIN4 for positive, AIN5 for negative
			ANALOG = 4, //Internal analog supply, either positive or negative (Vavdd or Vavss)
		};
		enum class TDACPNRegMap : uint8_t //Registry map for TDACP and TCACN
		{
			MAG = 0, //R/W, 5 bits, set the positive or negative magnitude of the DAC, default 2.5V
			OUT = 7, //R/W, low for no connection, high for connection to AIN6(P) or AIN7(N), default to no connection
		};
		enum class TDACValue : uint8_t //Acceptable values for TDACP and TDACN, assume +/-2.5V
		{
			Vp20000 = 0x09, //+2.00000V
			Vp10000 = 0x08,
			Vp05000 = 0x07,
			Vp02500 = 0x06,
			Vp01250 = 0x05,
			Vp00625 = 0x04,
			Vp00313 = 0x03,
			Vp00156 = 0x02,
			Vp00079 = 0x01, //+0.00078125V
			Vp00000 = 0x00, //0V
			Vn00079 = 0x11, //-0.00078125V
			Vn00156 = 0x12,
			Vn00313 = 0x13,
			Vn00625 = 0x14,
			Vn01250 = 0x15,
			Vn02500 = 0x16,
			Vn05000 = 0x17,
			Vn10000 = 0x18,
			Vn20000 = 0x19, //-2.00000V
		};
		enum class GPIOCONRegMap : uint8_t
		{
			CON7 = 0, //R/W, low for not connected, high for GPIO to AINCOM, default to not connected
			CON6 = 1, //R/W, low for not connected, high for GPIO to AIN9, default to not connected
			CON5 = 2, //R/W, low for not connected, high for GPIO to AIN8, default to not connected
			CON4 = 3, //R/W, low for not connected, high for GPIO to AIN7, default to not connected
			CON3 = 4, //R/W, low for not connected, high for GPIO to AIN6, default to not connected
			CON2 = 5, //R/W, low for not connected, high for GPIO to AIN5, default to not connected
			CON1 = 6, //R/W, low for not connected, high for GPIO to AIN4, default to not connected
			CON0 = 7, //R/W, low for not connected, high for GPIO to AIN3, default to not connected
		};
		enum class GPIODIRRegMap : uint8_t
		{
			DIR7 = 0, //R/W, low for output, high for input, default to output
			DIR6 = 1, //R/W, low for output, high for input, default to output
			DIR5 = 2, //R/W, low for output, high for input, default to output
			DIR4 = 3, //R/W, low for output, high for input, default to output
			DIR3 = 4, //R/W, low for output, high for input, default to output
			DIR2 = 5, //R/W, low for output, high for input, default to output
			DIR1 = 6, //R/W, low for output, high for input, default to output
			DIR0 = 7, //R/W, low for output, high for input, default to output
		};
		//GPIODATRegMap is pretty obvious, high to write high, defaults to low if output
		enum class ADC2CFGRegMap : uint8_t
		{
			GAIN2 = 0, //R/W, 3 bits, set gain for ADC2, defaults to 1 V/V
			REF2 = 3, //R/W, 3 bits, set voltage reference for ADC2, defaults to internal 2.5V reference
			DR2 = 6, //R/W, 2 bits, set sample rate for ADC2, defaults to 10 samples per second
		};
		enum class ADC2GainValue : uint8_t
		{
			V1V = 0, // 1V/V, unity gain
			V2V = 1, // 2V/V, 2x gain
			V4V = 2, // 4V/V, 4x gain
			V8V = 3,
			V16V = 4,
			V32V = 5,
			V64V = 6,
			V128V = 7,
		};
		enum class ADC2DataRateValue : uint8_t
		{
			SPS10 = 0,
			SPS100 = 1,
			SPS400 = 2,
			SPS800 = 3,
		};
		enum class ADC2Reference : uint8_t
		{
			INTERNAL = 0, //Set to 2.5V reference
			AIN01 = 1, //Set positive reference to AIN0 and negative to AIN1
			AIN23 = 2, //Same as above for AIN2 and AIN3
			AIN45 = 3, //Same as above for AIN4 and AIN5
			RAIL = 4, //Set reference to positive and negative analogue rails
		};
		enum class ADC2MUX : uint8_t
		{
			MUXN2 = 0, //R/W, 4 bits, select negative input for ADC2, defaults to AIN1
			MUXP2 = 4, //R/W, 4 bits, select positive input for ADC2, defaults to AIN0
		};
		struct PowerRegister
		{
			bool VBias,
			bool IntRefActive,
			bool Reset,
		}
		struct InterfaceRegister
		{
			bool TimeOutActive,
			bool ReportStatus,
			bool ReportCheckSum,
			bool ReportCheckCRC,
		}
		struct Mode0Register
		{
			bool RefReversal,
			bool RunPulse,
			bool ChopActive,
			bool IDACRotation,
			DelayValue Delay,
		};
		struct Mode1Register
		{
			bool SensorBiasToADC2,
			bool SensorBiasPolarity,
			FilterValue Filter,
			SensorBiasValue SensorBias,
		};
		struct Mode2Register
		{
			bool GainBypass,
			ADC1GainValue ADC1Gain,
			ADC1DataRateValue ADC1DataRate,
		};
		struct InputMultiplexerRegister
		{
			InputMUXValue PositiveChannel,
			InputMUXValue NegativeChannel,
		};
		struct IDACMultiplexerRegister
		{
			InputMUXValue Channel1,
			InputMUXValue Channel2,
		};
		struct IDACMagnitudeRegister
		{
			IDACMAGValue IDAC1Magnitude,
			IDACMAGValue IDAC2Magnitude,
		};
		struct ReferenceMultiplexerRegister
		{
			REFMUXValue PostitiveChannel,
			REFMUXValue NegativeChannel,
		};
		struct TDACConfigurationRegister
		{
			bool Active,
			TDACValue Magnitude,
		};
		struct ADC2ConfigurationRegister
		{
			ADC2DataRateValue DataRate,
			ADC2Reference Reference,
			ADC2GainValue Gain,
		};
		struct ADC2MultiplexerRegister
		{
			InputMUXValue PositiveChannel,
			InputMUXValue NegativeChannel,
		}
		//Values for ADC2MUX MUXN2 and MUXP2 are InputMUXValue
		ADS1263(uint8_t InitCSSelectPin);
		ADS1263(uint8_t InitCSSelectPin, uint8_t InitDataReadyPin);
		ADS1263(uint8_t InitCSSelectPin, uint8_t InitDataReadyPin, uint8_t InitStartPin);
		bool begin();
		bool check();
		//Pin controls
		bool SetDataReadyPin(uint8_t DataReadyPinToSet);
		bool SetUseDataReadyPin(bool ValueToSet);
		bool SetStartPin(uint8_t StartPinToSet);
		bool SetUseStartPin(uint8_t ValueToSet);
		//Setting power register
		bool SetReset(bool ValueToSet);
		bool SetVBias(bool ValueToSet);
		bool SetIntRefActive(bool ValueToSet);
		bool SetPowerRegister(bool Reset, bool VBiasToSet, bool IntRefActiveToSet);
		bool SetPowerRegister(PowerRegister* RegisterToSet);
		bool GetPowerRegister(PowerRegister* RegisterToGet);
		//Setting interface register
		bool SetTimeOut(bool ValueToSet);
		bool SetReportStatus(bool ValueToSet);
		bool SetReportCheckSum(bool ValueToSet);
		bool SetReportCheckCRC(bool ValueToSet);
		bool SetInterfaceRegister(bool TimeOutActiveToSet, bool ReportStatusToSet, bool ReportCheckSumToSet, bool ReportCheckCRCToSet);
		bool SetInterfaceRegister(InterfaceRegister* RegisterToSet);
		bool GetInterfaceRegister(InterfaceRegister* RegisterToGet);
		//Setting mode 0 register
		bool SetRefReversal(bool ValueToSet);
		bool SetRunPulse(bool ValueToSet);
		bool SetChopActive(bool ValueToSet);
		bool SetIDACRotation(bool ValueToSet);
		bool SetConversionDelay(DelayValue ValueToSet);
		bool SetMode0(bool RefReversalToSet, bool RunPulseToSet, bool ChopActiveToSet, bool IDACRotationToSet, DelayValue DelayValueToSet);
		bool SetMode0(Mode0Register* RegisterToSet);
		bool GetMode0(Mode0Register* RegisterToGet);
		//Setting mode 1 register
		bool SetFilter(FilterValue ValueToSet);
		bool SetSensorBiasToADC2(bool ValueToSet);
		bool SetSensorBiasPolarity(bool ValueToSet);
		bool SetSensorBias(SensorBiasValue ValueToSet);
		bool SetMode1(bool SensorBiasToADC2ToSet, bool SensorBiasPolarityToSet, FilterValue FilterValueToSet, SensorBiasValue SensorBiasValueToSet);
		bool SetMode1(Mode1Register* RegisterToSet);
		bool GetMode1(Mode1Register* RegisterToGet);
		//Setting mode 2 register
		bool SetGainBypass(bool ValueToSet);
		bool SetADC1Gain(ADC1GainValue ValueToSet);
		bool SetADC1DataRate(ADC1DataRateValue ValueToSet);
		bool SetMode2(bool GainBypassToSet, ADC1GainValue ADC1GainValueToSet, ADC1DataRateValue ADC1DataRateValueToSet);
		bool SetMode2(Mode2Register* RegisterToSet);
		bool GetMode2(Mode2Register* RegisterToGet);
		//Setting input multiplexer
		bool SetADC1PositiveChannel(InputMUXValue ValueToSet);
		bool SetADC1NegativeChannel(InputMUXValue ValueToSet);
		bool SetInputMultiplexer(InputMUXValue PositiveChannelToSet, InputMUXValue NegativeChannelToSet);
		bool SetInputMultiplexer(InputMultiplexerRegister* RegisterToSet);
		bool GetInputMultiplexer(InputMultiplexerRegister* RegisterToGet);
		//Setting IDAC multiplexer
		bool SetIDAC1Channel(IDACMUXValue ValueToSet);
		bool SetIDAC2Channel(IDACMUXValue ValueToSet);
		bool SetIDACMultiplexer(IDACMUXValue IDAC1ChannelToSet, IDACMUXValue IDAC2ChannelToSet);
		bool SetIDACMultiplexer(IDACMultiplexerRegister* RegisterToSet);
		bool GetIDACMultiplexer(IDACMultiplexerRegister* RegisterToGet);
		//Setting IDAC magnitude
		bool SetIDAC1Magnitude(IDACMAGValue ValueToSet);
		bool SetIDAC2Magnitude(IDACMAGValue ValueToSet);
		bool SetIDACMagnitudes(IDACMAGValue IDAC1MagnitudeToSet, IDACMAGValue IDAC2MagnitudeValueToSet);
		bool SetIDACMagnitudes(IDACMagnitudeRegister* RegisterToSet);
		bool GetIDACMagnitudes(IDACMagnitudeRegister* RegisterToSet);
		//Setting reference multiplexer
		bool SetReferenceChannelPositive(REFMUXValue ValueToSet);
		bool SetReferenceChannelNegative(REFMUXValue ValueToSet);
		bool SetReferenceChannelMultiplexer(REFMUXValue PostitiveChannelToSet, REFMUXValue NegativeChannelToSet);
		bool SetReferenceChannelMultiplexer(ReferenceMultiplexerRegister* RegisterToSet);
		bool GetReferenceChannelMultiplexer(ReferenceMultiplexerRegister* RegisterToGet);
		//Setting test DAC positive
		bool SetTDACPositiveOutput(bool ValueToSet);
		bool SetTDACPositiveMagnitude(TDACValue ValueToSet);
		bool SetTDACPositive(bool PositiveActiveValueToSet, TDACValue PositiveMagnitudeToSet);
		bool SetTDACPositive(TDACConfigurationRegister* RegisterToSet);
		bool GetTDACPositive(TDACConfigurationRegister* RegisterToGet);
		//Setting test DAC negative
		bool SetTDACNegativeOutput(bool ValueToSet);
		bool SetTDACNegativeMagnitude(TDACValue ValueToSet);
		bool SetTDACNegative(bool NegativeActiveValueToSet, TDACValue NegativeMagnitudeToSet);
		bool SetTDACNegative(TDACConfigurationRegister* RegisterToSet);
		bool GetTDACNegative(TDACConfigurationRegister* RegisterToGet);
		//Setting GPIO connection registers
		bool SetGPIOActive(uint8_t ChannelsToActive);
		bool SetGPIOActive(uint8_t Channel, bool ValueToSet);
		//Setting GPIO direction register
		bool SetGPIODirection(uint8_t ChannelAsOutput);
		bool SetGPIODirection(uint8_t Channel, bool ValueToSet);
		//Set GPIO output
		bool SetGPIOOutput(uint8_t ChannelsHigh);
		bool SetGPIOOutput(uint8_t Channel, bool ValueToSet);
		//Set ADC2 configuration
		bool SetADC2DataRate(ADC2DataRateValue ValueToSet);
		bool SetADC2Reference(ADC2Reference ValueToSet);
		bool SetADC2Gain(ADC2GainValue ValueToSet);
		bool SetADC2Configuration(ADC2DataRateValue DataRateToSet, ADC2Reference ReferenceToSet, ADC2GainValue GainToSet);
		bool SetADC2Configuration(ADC2ConfigurationRegister* RegisterToSet);
		bool GetADC2Configuration(ADC2ConfigurationRegister* RegisterToGet);
		//Set ADC multiplexer
		bool SetADC2Positive(InputMUXValue ValueToSet);
		bool SetADC2Negative(InputMUXValue ValueToSet);
		bool SetADC2Multiplexer(InputMUXValue PositiveChannelToSet, InputMUXValue NegativeChannelToSet);
		bool SetADC2Multiplexer(ADC2MultiplexerRegister* RegisterToSet);
		bool GetADC2Multiplexer(ADC2MultiplexerRegister* RegisterToGet);
	private:
		bool StatusByteActive;
		bool UseDataReadyPin;
		bool UseStartPin;
		uint8_t CSSelectPin;
		uint8_t DataReadyPin;
		uint8_t StartPin;
		SPISettings ConnectionSettings;
		bool SendRecieve(OpCodesSimple OpCodeToSend, uint8_t* RecievedData, uint8_t BytesToRecieve);
		bool ReadRegister(uint8_t RegisterAddress, uint8_t* ReceivedData, uint8_t BytesToRecieve);
		bool WriteRegister(uint8_t RegisterAddress, uint8_t* WriteData, uint8_t BytesToSend);
};
#endif