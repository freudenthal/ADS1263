#include "ADS1263.h"

const ADS1263::ADC1SampeTimeForRate ADS1263::ADC1SampleTimeTable[] =
{
	{ADC1DataRateValue::SPS3, (int32_t)(1.0/3.0*1000000.0) },
	{ADC1DataRateValue::SPS5, (int32_t)(1.0/5.0*1000000.0) },
	{ADC1DataRateValue::SPS10, (int32_t)(1.0/10.0*1000000.0) },
	{ADC1DataRateValue::SPS17, (int32_t)(1.0/17.0*1000000.0) },
	{ADC1DataRateValue::SPS20, (int32_t)(1.0/20.0*1000000.0) },
	{ADC1DataRateValue::SPS50, (int32_t)(1.0/50.0*1000000.0) },
	{ADC1DataRateValue::SPS60, (int32_t)(1.0/60.0*1000000.0) },
	{ADC1DataRateValue::SPS100, (int32_t)(1.0/100.0*1000000.0) },
	{ADC1DataRateValue::SPS400, (int32_t)(1.0/400.0*1000000.0) },
	{ADC1DataRateValue::SPS1200, (int32_t)(1.0/1200.0*1000000.0) },
	{ADC1DataRateValue::SPS2400, (int32_t)(1.0/2400.0*1000000.0) },
	{ADC1DataRateValue::SPS4800, (int32_t)(1.0/4800.0*1000000.0) },
	{ADC1DataRateValue::SPS7200, (int32_t)(1.0/7200.0*1000000.0) },
	{ADC1DataRateValue::SPS14400, (int32_t)(1.0/14400.0*1000000.0) },
	{ADC1DataRateValue::SPS19200, (int32_t)(1.0/19200.0*1000000.0) },
	{ADC1DataRateValue::SPS38400, (int32_t)(1.0/38400.0*1000000.0) },
};

const ADS1263::ADC2SampeTimeForRate ADS1263::ADC2SampleTimeTable[] =
{
	{ADC2DataRateValue::SPS10, (int32_t)(1.0/10.0*1000000.0) },
	{ADC2DataRateValue::SPS100, (int32_t)(1.0/100.0*1000000.0) },
	{ADC2DataRateValue::SPS400, (int32_t)(1.0/400.0*1000000.0) },
	{ADC2DataRateValue::SPS800, (int32_t)(1.0/800.0*1000000.0) },
};

ADS1263::ADS1263(uint8_t InitCSSelectPin)
{
	CSSelectPin = InitCSSelectPin;
	pinMode(CSSelectPin, OUTPUT);
	digitalWrite(CSSelectPin, HIGH);
	ReturnData = new uint8_t[ReturnDataSize]();
	HasStatusByte = true;
	HasCheckSumByte = true;
	HasCRCByte = false;
	Verbose = false;
	ReferenceVoltage = 2.5;
	ConnectionSettings = SPISettings(1000000, MSBFIRST, SPI_MODE1);
}
ADS1263::ADS1263(uint8_t InitCSSelectPin, uint8_t InitDataReadyPin) : ADS1263(InitCSSelectPin)
{
	DataReadyPin = InitDataReadyPin;
	UseDataReadyPin = true;
	pinMode(DataReadyPin, INPUT);
}
ADS1263::ADS1263(uint8_t InitCSSelectPin, uint8_t InitDataReadyPin, uint8_t InitStartPin) : ADS1263(InitCSSelectPin, InitDataReadyPin)
{
	StartPin = InitStartPin;
	UseStartPin = true;
	pinMode(StartPin, OUTPUT);
	digitalWrite(StartPin, LOW);
}
bool ADS1263::Begin()
{
	return Begin(true);
}
bool ADS1263::Begin(uint8_t InitADC2)
{
	if (Verbose)
	{
		Serial.print("[ADS1263] Sending reset.\n");
		Send(OpCodesSimple::RESET);
	}
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial power setting.\n");
	}
	PowerRegister InitialPowerSettings;
	InitialPowerSettings.Reset = false;
	InitialPowerSettings.VBias = false;
	InitialPowerSettings.IntRefActive = true;
	SetPowerRegister(&InitialPowerSettings);
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial interface setting.\n");
	}
	InterfaceRegister InitialInterfaceSettings;
	InitialInterfaceSettings.TimeOutActive = false;
	InitialInterfaceSettings.ReportStatus = true;
	InitialInterfaceSettings.ReportCheckSum = false;
	InitialInterfaceSettings.ReportCheckCRC = false;
	SetInterfaceRegister(&InitialInterfaceSettings);
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial mode0 setting.\n");
	}
	Mode0Register InitialMode0Register;
	InitialMode0Register.RefReversal = false;
	InitialMode0Register.RunPulse = true;
	InitialMode0Register.ChopActive = false;
	InitialMode0Register.IDACRotation = false;
	InitialMode0Register.Delay = DelayValue::NODELAY;
	SetMode0(&InitialMode0Register);
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial mode1 setting.\n");
	}
	Mode1Register InitialMode1Register;
	InitialMode1Register.SensorBiasToADC2 = false;
	InitialMode1Register.SensorBiasPolarity = false;
	InitialMode1Register.Filter = FilterValue::FIR;
	InitialMode1Register.SensorBias = SensorBiasValue::NOBIAS;
	SetMode1(&InitialMode1Register);
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial mode2 setting.\n");
	}
	Mode2Register InitialMode2Register;
	InitialMode2Register.GainBypass = false;
	InitialMode2Register.ADC1Gain = ADC1GainValue::V1V;
	InitialMode2Register.ADC1DataRate = ADC1DataRateValue::SPS20;
	SetMode2(&InitialMode2Register);
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial input multiplexer setting.\n");
	}
	InputMultiplexerRegister InitialInputMultiplexerRegister;
	InitialInputMultiplexerRegister.PositiveChannel = InputMUXValue::AIN0;
	InitialInputMultiplexerRegister.NegativeChannel = InputMUXValue::AINCOM;
	SetInputMultiplexer(&InitialInputMultiplexerRegister);
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial IDAC multiplexer setting.\n");
	}
	IDACMultiplexerRegister InitialIDACMultiplexerRegister;
	InitialIDACMultiplexerRegister.Channel1 = IDACMUXValue::NOCONNECT;
	InitialIDACMultiplexerRegister.Channel2 = IDACMUXValue::NOCONNECT;
	SetIDACMultiplexer(&InitialIDACMultiplexerRegister);
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial IDAC magnitude setting.\n");
	}
	IDACMagnitudeRegister InitialIDACMagnitudeRegister;
	InitialIDACMagnitudeRegister.IDAC1Magnitude = IDACMAGValue::uA0;
	InitialIDACMagnitudeRegister.IDAC2Magnitude = IDACMAGValue::uA0;
	SetIDACMagnitudes(&InitialIDACMagnitudeRegister);
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial reference multiplexer setting.\n");
	}
	ReferenceMultiplexerRegister InitialReferenceMultiplexerRegister;
	InitialReferenceMultiplexerRegister.PositiveChannel = REFMUXValue::INT2V5;
	InitialReferenceMultiplexerRegister.NegativeChannel = REFMUXValue::INT2V5;
	SetReferenceChannelMultiplexer(&InitialReferenceMultiplexerRegister);
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial TDAC positive setting.\n");
	}
	TDACConfigurationRegister InitialTDACPositveRegister;
	InitialTDACPositveRegister.Active = false;
	InitialTDACPositveRegister.Magnitude = TDACValue::Vp00000;
	SetTDACPositive(&InitialTDACPositveRegister);
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial TDAC negative setting.\n");
	}
	TDACConfigurationRegister InitialTDACNegativeRegister;
	InitialTDACNegativeRegister.Active = false;
	InitialTDACNegativeRegister.Magnitude = TDACValue::Vp00000;
	SetTDACNegative(&InitialTDACNegativeRegister);
	if (InitADC2)
	{
		if (Verbose)
		{
			Serial.print("[ADS1263] Initial ADC2 configuration setting.\n");
		}
		ADC2ConfigurationRegister InitialADC2ConfigurationRegister;
		InitialADC2ConfigurationRegister.DataRate = ADC2DataRateValue::SPS400;
		InitialADC2ConfigurationRegister.Reference = ADC2Reference::INT2V5;
		InitialADC2ConfigurationRegister.Gain = ADC2GainValue::V1V;
		SetADC2Configuration(&InitialADC2ConfigurationRegister);
		if (Verbose)
		{
			Serial.print("[ADS1263] Initial ADC2 multiplexer setting.\n");
		}
		ADC2MultiplexerRegister InitialADC2MultiplexerRegister;
		InitialADC2MultiplexerRegister.PositiveChannel = InputMUXValue::TEMP;
		InitialADC2MultiplexerRegister.NegativeChannel = InputMUXValue::TEMP;
		SetADC2Multiplexer(&InitialADC2MultiplexerRegister);
	}
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial GPIO active setting.\n");
	}
	uint8_t GIOPinsToActive = 0;
	SetGPIOActive(GIOPinsToActive);
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial GPIO direction setting.\n");
	}
	uint8_t GIOPinsDirection = 0;
	SetGPIODirection(GIOPinsDirection);
	if (Verbose)
	{
		Serial.print("[ADS1263] Initial GPIO output setting.\n");
	}
	uint8_t GIOPinsOutput = 0;
	SetGPIOOutput(GIOPinsOutput);
	return true;
}
bool ADS1263::Check()
{
	int32_t TimeNow = micros();
	if (RunningADC1)
	{
		if (UseDataReadyPin)
		{
			bool DataReadyPinStatus = (bool)digitalRead(DataReadyPin);
			if (!DataReadyPinStatus)
			{
				digitalWrite(StartPin, LOW);
				if (Verbose)
				{
					Serial.print("[ADS1263] Complete.\n");
				}
				bool IsNewAndStatus = ReadADC1();
				if (IsNewAndStatus)
				{
					RunningADC1 = false;
				}
			}
		}
		else
		{
			if (TimeNow > ADC1CheckTime)
			{
				bool IsNewAndStatus = ReadADC1();
				if (IsNewAndStatus)
				{
					RunningADC1 = false;
				}
				else
				{
					ADC1CheckTime = ADC1CheckTime + 1000;
				}
			}
		}
	}
	if (RunningADC2)
	{
		if (TimeNow > ADC2CheckTime)
		{
			bool IsNewAndStatus = ReadADC2();
			if (IsNewAndStatus)
			{
				RunningADC2 = false;
			}
			else
			{
				ADC2CheckTime = ADC2CheckTime + 1000;
			}
		}
	}
	return true;
}
bool ADS1263::ReadADC1()
{
	uint8_t ExpectedReturnDataSize = 4;
	if (HasStatusByte)
	{
		ExpectedReturnDataSize++;
	}
	if (HasCheckSumByte || HasCRCByte)
	{
		ExpectedReturnDataSize++;
	}
	bool Status = SendRecieve(OpCodesSimple::RDATA1,ReturnData,ExpectedReturnDataSize);
	uint8_t DataStartByte = 0;
	bool NewADC1 = true;
	if (HasStatusByte)
	{
		uint8_t StatusByte = ReturnData[0];
		NewADC1 = (bool)bitRead(StatusByte, static_cast<uint8_t>(StatusCode::ADC1));
		DataStartByte = 1;
	}
	uint8_tToint DataConverter;
	DataConverter.array[3] = ReturnData[DataStartByte+0];
	DataConverter.array[2] = ReturnData[DataStartByte+1];
	DataConverter.array[1] = ReturnData[DataStartByte+2];
	DataConverter.array[0] = ReturnData[DataStartByte+3];
	ADC1Value = DataConverter.integer;
	if (Verbose)
	{
		Serial.print("[ADS1263] ADC1 ");
		Serial.print(DataConverter.integer);
		Serial.print(',');
		Serial.print(DataConverter.array[0],HEX);
		Serial.print(',');
		Serial.print(DataConverter.array[1],HEX);
		Serial.print(',');
		Serial.print(DataConverter.array[2],HEX);
		Serial.print(',');
		Serial.print(DataConverter.array[3],HEX);
		Serial.print('\n');
	}
	if (HasCRCByte)
	{
		Status = Status & CheckCRC(ReturnData + DataStartByte, 4, ReturnData[DataStartByte+4]);
	}
	if (HasCheckSumByte)
	{
		Status = Status & CheckSum(ReturnData + DataStartByte, 4, ReturnData[DataStartByte+4]);
	}
	if (NewADC1)
	{
		FireADC1Callback();
	}
	return Status && NewADC1;
}
bool ADS1263::ReadADC2()
{
	uint8_t ExpectedReturnDataSize = 4;
	if (HasStatusByte)
	{
		ExpectedReturnDataSize++;
	}
	if (HasCheckSumByte || HasCRCByte)
	{
		ExpectedReturnDataSize++;
	}
	bool Status = SendRecieve(OpCodesSimple::RDATA2,ReturnData,ExpectedReturnDataSize);
	uint8_t DataStartByte = 0;
	bool NewADC2 = true;
	if (HasStatusByte)
	{
		uint8_t StatusByte = ReturnData[0];
		NewADC2 = (bool)bitRead(StatusByte, static_cast<uint8_t>(StatusCode::ADC2));
		DataStartByte = 1;
	}
	uint8_tToint DataConverter;
	DataConverter.array[3] = ReturnData[DataStartByte+0];
	DataConverter.array[2] = ReturnData[DataStartByte+1];
	DataConverter.array[1] = ReturnData[DataStartByte+2];
	DataConverter.array[0] = ReturnData[DataStartByte+3];
	ADC2Value = DataConverter.integer / 256;
	if (Verbose)
	{
		Serial.print("[ADS1263] ADC2 ");
		Serial.print(ADC1Value);
		Serial.print('\n');
	}
	if (HasCRCByte)
	{
		Status = Status & CheckCRC(ReturnData + DataStartByte, 4, ReturnData[DataStartByte+4]);
	}
	if (HasCheckSumByte)
	{
		Status = Status & CheckSum(ReturnData + DataStartByte, 4, ReturnData[DataStartByte+4]);
	}
	if (NewADC2)
	{
		FireADC2Callback();
	}
	return Status && NewADC2;
}
bool ADS1263::StartADC1()
{
	bool Status = true;
	if (RunningADC1)
	{
		Status = StopADC1();
	}
	RunningADC1 = true;
	if (UseStartPin)
	{
		digitalWrite(StartPin, HIGH);
		if (Verbose)
		{
			Serial.print("[ADS1263] START PIN\n");
		}
	}
	else
	{
		Status = Status && Send(OpCodesSimple::START1);
	}
	ADC1CheckTime = micros() + ADC1SampleTimeTable[static_cast<uint8_t>(ADC1DataRate)].Micros;
	return Status;
}
bool ADS1263::StopADC1()
{
	RunningADC1 = false;
	if (UseStartPin)
	{
		digitalWrite(StartPin, LOW);
		return true;
	}
	else
	{
		return Send(OpCodesSimple::STOP1);
	}
}
bool ADS1263::StartADC2()
{
	bool Status = true;
	if (RunningADC2)
	{
		Status = StopADC2();
	}
	if (!RunningADC2)
	{
		RunningADC2 = true;
		Status = Status && Send(OpCodesSimple::START2);
	}
	ADC2CheckTime = micros() + ADC2SampleTimeTable[static_cast<uint8_t>(ADC2DataRate)].Micros;
	return Status;
}
bool ADS1263::StopADC2()
{
	RunningADC2 = false;
	bool Status = Send(OpCodesSimple::STOP2);
	return Status;
}
bool ADS1263::FireADC1Callback()
{
	if (NewADC1Callback)
	{
		NewADC1Callback();
		return true;
	}
	else
	{
		return false;
	}
}
bool ADS1263::FireADC2Callback()
{
	if (NewADC2Callback)
	{
		NewADC2Callback();
		return true;
	}
	else
	{
		return false;
	}
}
bool ADS1263::SetVerbose(bool VerboseToSet)
{
	Verbose = VerboseToSet;
	return true;
}
bool ADS1263::SetReferenceVoltage(double ReferenceVoltageToSet)
{
	ReferenceVoltage = ReferenceVoltageToSet;
	return true;
}
bool ADS1263::GetADC1Value(int32_t* DataToGet)
{
	*DataToGet = ADC1Value;
	return true;
}
bool ADS1263::GetADC1Value(double* DataToGet)
{
	//int32 max value 2147483647
	double GainAmplitude = (double)(1 << static_cast<uint8_t>(ADC1GainSetting));
	*DataToGet =  (double)(ADC1Value) * ReferenceVoltage / GainAmplitude / (double)(2147483647);
	if (Verbose)
	{
		Serial.print("[ADS1263] ADC1V ");
		Serial.print(*DataToGet);
		Serial.print(',');
		Serial.print(GainAmplitude);
		Serial.print(',');
		Serial.print(ReferenceVoltage);
		Serial.print('\n');
	}
	return true;
}
bool ADS1263::GetADC1Busy(bool* DataToGet)
{
	*DataToGet = RunningADC1;
	return true;
}
bool ADS1263::GetADC2Busy(bool* DataToGet)
{
	*DataToGet = RunningADC2;
	return true;
}
bool ADS1263::GetADCBusy(bool* DataToGet)
{
	*DataToGet = RunningADC1 || RunningADC1;
	return true;
}
bool ADS1263::GetADC1Temperature(double* DataToGet)
{
	double VoltageRecorded;
	bool Status = GetADC1Value(&VoltageRecorded);
	*DataToGet = ((VoltageRecorded * 0.000001)-122400.0)/420.0+25.0;
	return Status;
}
bool ADS1263::GetADC2Value(int32_t* DataToGet)
{
	*DataToGet = ADC2Value;
	return true;
}
bool ADS1263::GetADC2Value(double* DataToGet)
{
	//int24 max value 8388608
	*DataToGet =  (double)(ADC2Value) / (double)(8388608) * (double)(1 << static_cast<uint8_t>(ADC2GainSetting)) * ReferenceVoltage;
	return true;
}
bool ADS1263::GetADC2Temperature(double* DataToGet)
{
	double VoltageRecorded;
	bool Status = GetADC2Value(&VoltageRecorded);
	*DataToGet = ((VoltageRecorded * 0.000001)-122400.0)/420.0+25.0;
	return Status;
}
bool ADS1263::CheckSum(uint8_t* Data, uint8_t DataSize, uint8_t CheckValue)
{
	uint8_t Sum = 0;
	for (int DataIndex = 0; DataIndex < DataSize; ++DataIndex)
	{
		Sum = (Sum + Data[DataIndex]) & 0xFF;
	}
	Sum = (Sum + CheckSumConstant) & 0xFF;
	if (Sum != CheckValue)
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool ADS1263::CheckCRC(uint8_t* Data, uint8_t DataSize, uint8_t CheckValue)
{
    uint8_t crc = 0x00;
    while (DataSize--)
    {
        uint8_t extract = *Data++;
        for (uint8_t tempI = 8; tempI; tempI--)
        {
            uint8_t sum = (crc ^ extract) & 0x01;
            crc >>= 1;
            if (sum)
            {
                crc ^= 0x8C;
            }
            extract >>= 1;
        }
    }
    if (CheckValue != crc)
    {
    	return false;
    }
    else
    {
    	return true;
    }
}
bool ADS1263::SetNewADC1Callback(NewSampleListener ListenerToSet)
{
	NewADC1Callback = ListenerToSet;
	return true;
}
bool ADS1263::SetNewADC2Callback(NewSampleListener ListenerToSet)
{
	NewADC2Callback = ListenerToSet;
	return true;
}
//Pin controls
bool ADS1263::SetDataReadyPin(uint8_t DataReadyPinToSet)
{
	DataReadyPin = DataReadyPinToSet;
	return true;
}
bool ADS1263::SetUseDataReadyPin(bool ValueToSet)
{
	UseDataReadyPin = ValueToSet;
	return true;
}
bool ADS1263::SetStartPin(uint8_t StartPinToSet)
{
	StartPin = StartPinToSet;
	return true;
}
bool ADS1263::SetUseStartPin(uint8_t ValueToSet)
{
	UseStartPin = ValueToSet;
	return true;
}
bool ADS1263::GetChipID(uint8_t* IDByteToSet)
{
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::ID), IDByteToSet, 1);
	return Status;
}
//Setting power register
bool ADS1263::SetReset(bool ValueToSet)
{
	PowerRegister RegisterToSet;
	bool Status = GetPowerRegister(&RegisterToSet);
	RegisterToSet.Reset = ValueToSet;
	return Status & SetPowerRegister(&RegisterToSet);
}
bool ADS1263::SetVBias(bool ValueToSet)
{
	PowerRegister RegisterToSet;
	bool Status = GetPowerRegister(&RegisterToSet);
	RegisterToSet.VBias = ValueToSet;
	return Status & SetPowerRegister(&RegisterToSet);
}
bool ADS1263::SetIntRefActive(bool ValueToSet)
{
	PowerRegister RegisterToSet;
	bool Status = GetPowerRegister(&RegisterToSet);
	RegisterToSet.IntRefActive = ValueToSet;
	return Status & SetPowerRegister(&RegisterToSet);
}
bool ADS1263::SetPowerRegister(bool ResetToSet, bool VBiasToSet, bool IntRefActiveToSet)
{
	PowerRegister RegisterToSet;
	RegisterToSet.Reset = ResetToSet;
	RegisterToSet.VBias = VBiasToSet;
	RegisterToSet.IntRefActive = IntRefActiveToSet;
	return SetPowerRegister(&RegisterToSet);
}
bool ADS1263::SetPowerRegister(PowerRegister* RegisterToSet)
{
	uint8_t RegisterToSend = 0;
	bitWrite(RegisterToSend, static_cast<uint8_t>(PowerRegMap::RESET), RegisterToSet->Reset);
	bitWrite(RegisterToSend, static_cast<uint8_t>(PowerRegMap::VBIAS), RegisterToSet->VBias);
	bitWrite(RegisterToSend, static_cast<uint8_t>(PowerRegMap::INTREF), RegisterToSet->IntRefActive);
	//if (Verbose)
	//{
	//	Serial.print("[ADS1263] Power reg: ");
	//	Serial.print(RegisterToSend, HEX);
	//	Serial.print('\n');
	//}
	return WriteRegister(static_cast<uint8_t>(RegisterMap::POWER), &RegisterToSend, 1);
}
bool ADS1263::GetPowerRegister(PowerRegister* RegisterToGet)
{
	const uint8_t ResetBitToRead = static_cast<uint8_t>( PowerRegMap::RESET );
	const uint8_t VBiasBitToRead = static_cast<uint8_t>( PowerRegMap::VBIAS );
	const uint8_t IntRefBitToRead = static_cast<uint8_t>( PowerRegMap::INTREF );
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::POWER), &ReturnByte, 1);
	RegisterToGet->Reset = (bool)bitRead(ReturnByte, ResetBitToRead);
	RegisterToGet->VBias = (bool)bitRead(ReturnByte, VBiasBitToRead);
	RegisterToGet->IntRefActive = (bool)bitRead(ReturnByte, IntRefBitToRead);
	return Status;
}
//Setting interface register
bool ADS1263::SetTimeOut(bool ValueToSet)
{
	InterfaceRegister RegisterToSet;
	bool Status = GetInterfaceRegister(&RegisterToSet);
	RegisterToSet.TimeOutActive = ValueToSet;
	return Status & SetInterfaceRegister(&RegisterToSet);
}
bool ADS1263::SetReportStatus(bool ValueToSet)
{
	InterfaceRegister RegisterToSet;
	bool Status = GetInterfaceRegister(&RegisterToSet);
	RegisterToSet.ReportStatus = ValueToSet;
	return Status & SetInterfaceRegister(&RegisterToSet);
}
bool ADS1263::SetReportCheckSum(bool ValueToSet)
{
	InterfaceRegister RegisterToSet;
	bool Status = GetInterfaceRegister(&RegisterToSet);
	RegisterToSet.ReportCheckSum = ValueToSet;
	return Status & SetInterfaceRegister(&RegisterToSet);
}
bool ADS1263::SetReportCheckCRC(bool ValueToSet)
{
	InterfaceRegister RegisterToSet;
	bool Status = GetInterfaceRegister(&RegisterToSet);
	RegisterToSet.ReportCheckCRC = ValueToSet;
	return Status & SetInterfaceRegister(&RegisterToSet);
}
bool ADS1263::SetInterfaceRegister(bool TimeOutActiveToSet, bool ReportStatusToSet, bool ReportCheckSumToSet, bool ReportCheckCRCToSet)
{
	InterfaceRegister RegisterToSet;
	RegisterToSet.TimeOutActive = TimeOutActiveToSet;
	RegisterToSet.ReportStatus = ReportStatusToSet;
	RegisterToSet.ReportCheckCRC = ReportCheckSumToSet;
	RegisterToSet.ReportCheckSum = ReportCheckCRCToSet;
	return SetInterfaceRegister(&RegisterToSet);
}
bool ADS1263::SetInterfaceRegister(InterfaceRegister* RegisterToSet)
{
	HasStatusByte = RegisterToSet->ReportStatus;
	HasCheckSumByte = RegisterToSet->ReportCheckSum;
	HasCRCByte = RegisterToSet->ReportCheckCRC;
	uint8_t RegisterToSend = 0;
	bitWrite(RegisterToSend, static_cast<uint8_t>(InterfaceRegMap::TIMEOUT), RegisterToSet->TimeOutActive);
	bitWrite(RegisterToSend, static_cast<uint8_t>(InterfaceRegMap::STATUS), RegisterToSet->ReportStatus);
	bitWrite(RegisterToSend, static_cast<uint8_t>(InterfaceRegMap::CHECKCRC), RegisterToSet->ReportCheckCRC);
	bitWrite(RegisterToSend, static_cast<uint8_t>(InterfaceRegMap::CHECKSUM), RegisterToSet->ReportCheckSum);
	return WriteRegister(static_cast<uint8_t>(RegisterMap::INTERFACE), &RegisterToSend, 1);
}
bool ADS1263::GetInterfaceRegister(InterfaceRegister* RegisterToGet)
{
	const uint8_t TimeOutBitToRead = static_cast<uint8_t>( InterfaceRegMap::TIMEOUT );
	const uint8_t StatusBitToRead = static_cast<uint8_t>( InterfaceRegMap::STATUS );
	const uint8_t CheckCRCBitToRead = static_cast<uint8_t>( InterfaceRegMap::CHECKCRC );
	const uint8_t CheckSumToRead = static_cast<uint8_t>( InterfaceRegMap::CHECKSUM );
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::INTERFACE), &ReturnByte, 1);
	RegisterToGet->TimeOutActive = (bool)bitRead(ReturnByte, TimeOutBitToRead);
	RegisterToGet->ReportStatus = (bool)bitRead(ReturnByte, StatusBitToRead);
	RegisterToGet->ReportCheckCRC = (bool)bitRead(ReturnByte, CheckCRCBitToRead);
	RegisterToGet->ReportCheckSum = (bool)bitRead(ReturnByte, CheckSumToRead);
	HasStatusByte = RegisterToGet->ReportStatus;
	HasCheckSumByte = RegisterToGet->ReportCheckSum;
	HasCRCByte = RegisterToGet->ReportCheckCRC;
	return Status;
}
//Setting mode 0 register
bool ADS1263::SetRefReversal(bool ValueToSet)
{
	Mode0Register RegisterToSet;
	bool Status = GetMode0(&RegisterToSet);
	RegisterToSet.RefReversal = ValueToSet;
	return Status & SetMode0(&RegisterToSet);
}
bool ADS1263::SetRunPulse(bool ValueToSet)
{
	Mode0Register RegisterToSet;
	bool Status = GetMode0(&RegisterToSet);
	RegisterToSet.RunPulse = ValueToSet;
	return Status & SetMode0(&RegisterToSet);
}
bool ADS1263::SetChopActive(bool ValueToSet)
{
	Mode0Register RegisterToSet;
	bool Status = GetMode0(&RegisterToSet);
	RegisterToSet.ChopActive = ValueToSet;
	return Status & SetMode0(&RegisterToSet);
}
bool ADS1263::SetIDACRotation(bool ValueToSet)
{
	Mode0Register RegisterToSet;
	bool Status = GetMode0(&RegisterToSet);
	RegisterToSet.IDACRotation = ValueToSet;
	return Status & SetMode0(&RegisterToSet);
}
bool ADS1263::SetConversionDelay(DelayValue ValueToSet)
{
	Mode0Register RegisterToSet;
	bool Status = GetMode0(&RegisterToSet);
	RegisterToSet.Delay = ValueToSet;
	return Status & SetMode0(&RegisterToSet);
}
bool ADS1263::SetMode0(bool RefReversalToSet, bool RunPulseToSet, bool ChopActiveToSet, bool IDACRotationToSet, DelayValue DelayValueToSet)
{
	Mode0Register RegisterToSet;
	RegisterToSet.Delay = DelayValueToSet;
	RegisterToSet.RefReversal = RefReversalToSet;
	RegisterToSet.RunPulse = RunPulseToSet;
	RegisterToSet.IDACRotation = IDACRotationToSet;
	RegisterToSet.ChopActive = ChopActiveToSet;
	return SetMode0(&RegisterToSet);
}
bool ADS1263::SetMode0(Mode0Register* RegisterToSet)
{
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->Delay);
	bitWrite(RegisterToSend, static_cast<uint8_t>(Mode0RegMap::REFREV), RegisterToSet->RefReversal);
	bitWrite(RegisterToSend, static_cast<uint8_t>(Mode0RegMap::RUNMODE), RegisterToSet->RunPulse);
	bitWrite(RegisterToSend, static_cast<uint8_t>(Mode0RegMap::IDAC), RegisterToSet->IDACRotation);
	bitWrite(RegisterToSend, static_cast<uint8_t>(Mode0RegMap::CHOP), RegisterToSet->ChopActive);
	return WriteRegister(static_cast<uint8_t>(RegisterMap::MODE0), &RegisterToSend, 1);
}
bool ADS1263::GetMode0(Mode0Register* RegisterToGet)
{
	const uint8_t RefReversalBitToRead = static_cast<uint8_t>( Mode0RegMap::REFREV );
	const uint8_t RunPulseBitToRead = static_cast<uint8_t>( Mode0RegMap::RUNMODE );
	const uint8_t IDACRotationBitToRead = static_cast<uint8_t>( Mode0RegMap::IDAC );
	const uint8_t ChopBitToRead = static_cast<uint8_t>( Mode0RegMap::CHOP );
	const uint8_t DelayMask = (1 << static_cast<uint8_t>( Mode0RegMap::CHOP )) - 1;
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::MODE0), &ReturnByte, 1);
	RegisterToGet->RefReversal = (bool)bitRead(ReturnByte, RefReversalBitToRead);
	RegisterToGet->RunPulse = (bool)bitRead(ReturnByte, RunPulseBitToRead);
	RegisterToGet->IDACRotation = (bool)bitRead(ReturnByte, IDACRotationBitToRead);
	RegisterToGet->ChopActive = (bool)bitRead(ReturnByte, ChopBitToRead);
	RegisterToGet->Delay = static_cast<DelayValue>(ReturnByte & DelayMask);
	return Status;
}
//Setting mode 1 register
bool ADS1263::SetFilter(FilterValue ValueToSet)
{
	Mode1Register RegisterToSet;
	bool Status = GetMode1(&RegisterToSet);
	RegisterToSet.Filter = ValueToSet;
	return Status & SetMode1(&RegisterToSet);
}
bool ADS1263::SetSensorBiasToADC2(bool ValueToSet)
{
	Mode1Register RegisterToSet;
	bool Status = GetMode1(&RegisterToSet);
	RegisterToSet.SensorBiasToADC2 = ValueToSet;
	return Status & SetMode1(&RegisterToSet);
}
bool ADS1263::SetSensorBiasPolarity(bool ValueToSet)
{
	Mode1Register RegisterToSet;
	bool Status = GetMode1(&RegisterToSet);
	RegisterToSet.SensorBiasPolarity = ValueToSet;
	return Status & SetMode1(&RegisterToSet);
}
bool ADS1263::SetSensorBias(SensorBiasValue ValueToSet)
{
	Mode1Register RegisterToSet;
	bool Status = GetMode1(&RegisterToSet);
	RegisterToSet.SensorBias = ValueToSet;
	return Status & SetMode1(&RegisterToSet);
}
bool ADS1263::SetMode1(bool SensorBiasToADC2ToSet, bool SensorBiasPolarityToSet, FilterValue FilterValueToSet, SensorBiasValue SensorBiasValueToSet)
{
	Mode1Register RegisterToSet;
	RegisterToSet.Filter = FilterValueToSet;
	RegisterToSet.SensorBias = SensorBiasValueToSet;
	RegisterToSet.SensorBiasToADC2 = SensorBiasToADC2ToSet;
	RegisterToSet.SensorBiasPolarity = SensorBiasPolarityToSet;
	return SetMode1(&RegisterToSet);
}
bool ADS1263::SetMode1(Mode1Register* RegisterToSet)
{
	const uint8_t FilterShift = static_cast<uint8_t>(Mode1RegMap::FILTER);
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->SensorBias);
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToSet->Filter) << FilterShift);
	bitWrite(RegisterToSend, static_cast<uint8_t>(Mode1RegMap::SBADC), RegisterToSet->SensorBiasToADC2);
	bitWrite(RegisterToSend, static_cast<uint8_t>(Mode1RegMap::SBPOL), RegisterToSet->SensorBiasPolarity);
	return WriteRegister(static_cast<uint8_t>(RegisterMap::MODE1), &RegisterToSend, 1);
}
bool ADS1263::GetMode1(Mode1Register* RegisterToGet)
{
	const uint8_t FilterMask = ((1 << 8) - ( 1 << static_cast<uint8_t>(Mode1RegMap::FILTER)));
	const uint8_t FilterShift = static_cast<uint8_t>(Mode1RegMap::FILTER);
	const uint8_t SensorBiasBitToRead = static_cast<uint8_t>(Mode1RegMap::SBADC);
	const uint8_t SensorPolarityBitToRead = static_cast<uint8_t>(Mode1RegMap::SBPOL);
	const uint8_t SensorMagnitudeMask = (1<<static_cast<uint8_t>(Mode1RegMap::SBPOL)) - 1;
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::MODE1), &ReturnByte, 1);
	RegisterToGet->Filter = static_cast<FilterValue>( (ReturnByte & FilterMask) >> FilterShift );
	RegisterToGet->SensorBiasToADC2 = (bool)bitRead(ReturnByte, SensorBiasBitToRead);
	RegisterToGet->SensorBiasPolarity = (bool)bitRead(ReturnByte, SensorPolarityBitToRead);
	RegisterToGet->SensorBias = static_cast<SensorBiasValue>( ReturnByte & SensorMagnitudeMask );
	return Status;
}
//Setting mode 2 register
bool ADS1263::SetGainBypass(bool ValueToSet)
{
	Mode2Register RegisterToSet;
	bool Status = GetMode2(&RegisterToSet);
	RegisterToSet.GainBypass = ValueToSet;
	return Status & SetMode2(&RegisterToSet);
}
bool ADS1263::SetADC1Gain(ADC1GainValue ValueToSet)
{
	Mode2Register RegisterToSet;
	bool Status = GetMode2(&RegisterToSet);
	RegisterToSet.ADC1Gain = ValueToSet;
	return Status & SetMode2(&RegisterToSet);
}
bool ADS1263::SetADC1DataRate(ADC1DataRateValue ValueToSet)
{
	Mode2Register RegisterToSet;
	bool Status = GetMode2(&RegisterToSet);
	RegisterToSet.ADC1DataRate = ValueToSet;
	return Status & SetMode2(&RegisterToSet);
}
bool ADS1263::SetMode2(bool GainBypassToSet, ADC1GainValue ADC1GainValueToSet, ADC1DataRateValue ADC1DataRateValueToSet)
{
	Mode2Register RegisterToSet;
	RegisterToSet.GainBypass = GainBypassToSet;
	RegisterToSet.ADC1Gain = ADC1GainValueToSet;
	RegisterToSet.ADC1DataRate = ADC1DataRateValueToSet;
	return SetMode2(&RegisterToSet);
}
bool ADS1263::SetMode2(Mode2Register* RegisterToSet)
{
	const uint8_t GainShift = static_cast<uint8_t>(Mode2RegMap::GAIN);
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->ADC1DataRate);
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToSet->ADC1Gain) << GainShift);
	bitWrite(RegisterToSend, static_cast<uint8_t>(Mode2RegMap::BYPASS), RegisterToSet->GainBypass);
	ADC1GainSetting = RegisterToSet->ADC1Gain;
	ADC1DataRate = RegisterToSet->ADC1DataRate;
	return WriteRegister(static_cast<uint8_t>(RegisterMap::MODE2), &RegisterToSend, 1);
}
bool ADS1263::GetMode2(Mode2Register* RegisterToGet)
{
	const uint8_t BypassBitToRead = static_cast<uint8_t>(Mode2RegMap::BYPASS);
	const uint8_t GainMask = (1 << static_cast<uint8_t>(Mode2RegMap::BYPASS) ) - ( 1 << static_cast<uint8_t>(Mode2RegMap::GAIN) );
	const uint8_t GainShift = static_cast<uint8_t>(Mode2RegMap::GAIN);
	const uint8_t DataRateMask = ( 1 << static_cast<uint8_t>(Mode2RegMap::GAIN) ) - 1;
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::MODE2), &ReturnByte, 1);
	RegisterToGet->GainBypass = (bool)bitRead(ReturnByte, BypassBitToRead);
	RegisterToGet->ADC1Gain = static_cast<ADC1GainValue>( (ReturnByte & GainMask) >> GainShift );
	RegisterToGet->ADC1DataRate = static_cast<ADC1DataRateValue>( ReturnByte & DataRateMask );
	ADC1GainSetting = RegisterToGet->ADC1Gain;
	ADC1DataRate = RegisterToGet->ADC1DataRate;
	return Status;
}
//Setting input multiplexer
bool ADS1263::SetADC1PositiveChannel(InputMUXValue ValueToSet)
{
	InputMultiplexerRegister RegisterToSet;
	bool Status = GetInputMultiplexer(&RegisterToSet);
	RegisterToSet.PositiveChannel = ValueToSet;
	if (Verbose)
	{
		Serial.print("[ADS1263] ADC1MUX PSET ");
		Serial.print(static_cast<uint8_t>(RegisterToSet.PositiveChannel), HEX);
		Serial.print(',');
		Serial.print(static_cast<uint8_t>(RegisterToSet.NegativeChannel), HEX);
		Serial.print('\n');
	}
	return Status & SetInputMultiplexer(&RegisterToSet);
}
bool ADS1263::SetADC1NegativeChannel(InputMUXValue ValueToSet)
{
	InputMultiplexerRegister RegisterToSet;
	bool Status = GetInputMultiplexer(&RegisterToSet);
	RegisterToSet.NegativeChannel = ValueToSet;
	return Status & SetInputMultiplexer(&RegisterToSet);
}
bool ADS1263::SetInputMultiplexer(InputMUXValue PositiveChannelToSet, InputMUXValue NegativeChannelToSet)
{
	InputMultiplexerRegister RegisterToSet;
	RegisterToSet.PositiveChannel = PositiveChannelToSet;
	RegisterToSet.NegativeChannel = NegativeChannelToSet;
	return SetInputMultiplexer(&RegisterToSet);
}
bool ADS1263::SetInputMultiplexer(InputMultiplexerRegister* RegisterToSet)
{
	const uint8_t PositiveChannelShift = static_cast<uint8_t>(InputMUXRegMap::MUXP);
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->NegativeChannel);
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToSet->PositiveChannel) << PositiveChannelShift);
	return WriteRegister(static_cast<uint8_t>(RegisterMap::INPMUX), &RegisterToSend, 1);
}
bool ADS1263::GetInputMultiplexer(InputMultiplexerRegister* RegisterToGet)
{
	const uint8_t PositiveChannelShift = static_cast<uint8_t>(InputMUXRegMap::MUXP);
	const uint8_t NegativeChannelMask = ( (1 << static_cast<uint8_t>(InputMUXRegMap::MUXP)) - 1);
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::INPMUX), &ReturnByte, 1);
	RegisterToGet->PositiveChannel = static_cast<InputMUXValue>( ReturnByte >> PositiveChannelShift );
	RegisterToGet->NegativeChannel = static_cast<InputMUXValue>( ReturnByte & NegativeChannelMask );
	if (Verbose)
	{
		Serial.print("[ADS1263] ADC1MUX GET ");
		Serial.print(ReturnByte, HEX);
		Serial.print(',');
		Serial.print(static_cast<uint8_t>(RegisterToGet->PositiveChannel), HEX);
		//Serial.print(',');
		//Serial.print(PositiveChannelShift, HEX);
		Serial.print(',');
		Serial.print(static_cast<uint8_t>(RegisterToGet->NegativeChannel), HEX);
		//Serial.print(',');
		//Serial.print(NegativeChannelMask, HEX);
		Serial.print('\n');
	}
	return Status;
}
//Setting IDAC multiplexer
bool ADS1263::SetIDAC1Channel(IDACMUXValue ValueToSet)
{
	IDACMultiplexerRegister RegisterToSet;
	bool Status = GetIDACMultiplexer(&RegisterToSet);
	RegisterToSet.Channel1 = ValueToSet;
	return Status & SetIDACMultiplexer(&RegisterToSet);
}
bool ADS1263::SetIDAC2Channel(IDACMUXValue ValueToSet)
{
	IDACMultiplexerRegister RegisterToSet;
	bool Status = GetIDACMultiplexer(&RegisterToSet);
	RegisterToSet.Channel2 = ValueToSet;
	return Status & SetIDACMultiplexer(&RegisterToSet);
}
bool ADS1263::SetIDACMultiplexer(IDACMUXValue IDAC1ChannelToSet, IDACMUXValue IDAC2ChannelToSet)
{
	IDACMultiplexerRegister RegisterToSet;
	RegisterToSet.Channel1 = IDAC1ChannelToSet;
	RegisterToSet.Channel2 = IDAC2ChannelToSet;
	return SetIDACMultiplexer(&RegisterToSet);
}
bool ADS1263::SetIDACMultiplexer(IDACMultiplexerRegister* RegisterToSet)
{
	const uint8_t Channel2Shift = static_cast<uint8_t>(IDACMUXRegMap::MUX2);
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->Channel1);
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToSet->Channel2) << Channel2Shift);
	return WriteRegister(static_cast<uint8_t>(RegisterMap::IDACMUX), &RegisterToSend, 1);
}
bool ADS1263::GetIDACMultiplexer(IDACMultiplexerRegister* RegisterToGet)
{
	const uint8_t Channel2Shift = static_cast<uint8_t>(IDACMUXRegMap::MUX2);
	const uint8_t Channel1Mask = ( (1 << static_cast<uint8_t>(IDACMUXRegMap::MUX2)) - 1);
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::IDACMUX), &ReturnByte, 1);
	RegisterToGet->Channel2 = static_cast<IDACMUXValue>( ReturnByte >> Channel2Shift );
	RegisterToGet->Channel1 = static_cast<IDACMUXValue>( ReturnByte & Channel1Mask );
	return Status;
}
//Setting IDAC magnitude
bool ADS1263::SetIDAC1Magnitude(IDACMAGValue ValueToSet)
{
	IDACMagnitudeRegister RegisterToSet;
	bool Status = GetIDACMagnitudes(&RegisterToSet);
	RegisterToSet.IDAC1Magnitude = ValueToSet;
	return Status & SetIDACMagnitudes(&RegisterToSet);
}
bool ADS1263::SetIDAC2Magnitude(IDACMAGValue ValueToSet)
{
	IDACMagnitudeRegister RegisterToSet;
	bool Status = GetIDACMagnitudes(&RegisterToSet);
	RegisterToSet.IDAC2Magnitude = ValueToSet;
	return Status & SetIDACMagnitudes(&RegisterToSet);
}
bool ADS1263::SetIDACMagnitudes(IDACMAGValue IDAC1MagnitudeToSet, IDACMAGValue IDAC2MagnitudeValueToSet)
{
	IDACMagnitudeRegister RegisterToSet;
	RegisterToSet.IDAC1Magnitude = IDAC1MagnitudeToSet;
	RegisterToSet.IDAC2Magnitude = IDAC2MagnitudeValueToSet;
	return SetIDACMagnitudes(&RegisterToSet);
}
bool ADS1263::SetIDACMagnitudes(IDACMagnitudeRegister* RegisterToSet)
{
	const uint8_t Channel2Shift = static_cast<uint8_t>(IDACMAGRegMap::MAG2);
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->IDAC1Magnitude);
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToSet->IDAC2Magnitude) << Channel2Shift);
	return WriteRegister(static_cast<uint8_t>(RegisterMap::IDACMAG), &RegisterToSend, 1);
}
bool ADS1263::GetIDACMagnitudes(IDACMagnitudeRegister* RegisterToGet)
{
	const uint8_t IDAC2Shift = static_cast<uint8_t>(IDACMAGRegMap::MAG2);
	const uint8_t IDAC1Mask = ( (1 << static_cast<uint8_t>(IDACMAGRegMap::MAG2)) - 1);
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::IDACMAG), &ReturnByte, 1);
	RegisterToGet->IDAC2Magnitude = static_cast<IDACMAGValue>( ReturnByte >> IDAC2Shift );
	RegisterToGet->IDAC1Magnitude = static_cast<IDACMAGValue>( ReturnByte & IDAC1Mask );
	return Status;
}
//Setting reference multiplexer
bool ADS1263::SetReferenceChannelPositive(REFMUXValue ValueToSet)
{
	ReferenceMultiplexerRegister RegisterToSet;
	bool Status = GetReferenceChannelMultiplexer(&RegisterToSet);
	RegisterToSet.PositiveChannel = ValueToSet;
	return Status & SetReferenceChannelMultiplexer(&RegisterToSet);
}
bool ADS1263::SetReferenceChannelNegative(REFMUXValue ValueToSet)
{
	ReferenceMultiplexerRegister RegisterToSet;
	bool Status = GetReferenceChannelMultiplexer(&RegisterToSet);
	RegisterToSet.NegativeChannel = ValueToSet;
	return Status & SetReferenceChannelMultiplexer(&RegisterToSet);
}
bool ADS1263::SetReferenceChannelMultiplexer(REFMUXValue PositiveChannelToSet, REFMUXValue NegativeChannelToSet)
{
	ReferenceMultiplexerRegister RegisterToSet;
	RegisterToSet.PositiveChannel = PositiveChannelToSet;
	RegisterToSet.NegativeChannel = NegativeChannelToSet;
	return SetReferenceChannelMultiplexer(&RegisterToSet);
}
bool ADS1263::SetReferenceChannelMultiplexer(ReferenceMultiplexerRegister* RegisterToSet)
{
	const uint8_t PositiveChannelShift = static_cast<uint8_t>(REFMUXRegMap::RMUXP);
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->NegativeChannel);
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToSet->PositiveChannel) << PositiveChannelShift);
	return WriteRegister(static_cast<uint8_t>(RegisterMap::REFMUX), &RegisterToSend, 1);
}
bool ADS1263::GetReferenceChannelMultiplexer(ReferenceMultiplexerRegister* RegisterToGet)
{
	const uint8_t PostitiveChannelMask = (1 << 6) - 1;
	const uint8_t PostitiveChannelShift = static_cast<uint8_t>(REFMUXRegMap::RMUXP);
	const uint8_t NegativeChannelMask = ( (1 << static_cast<uint8_t>(REFMUXRegMap::RMUXP)) - 1);
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::REFMUX), &ReturnByte, 1);
	RegisterToGet->PositiveChannel = static_cast<REFMUXValue>( (ReturnByte & PostitiveChannelMask) >> PostitiveChannelShift );
	RegisterToGet->NegativeChannel = static_cast<REFMUXValue>( ReturnByte & NegativeChannelMask );
	return Status;
}
//Setting test DAC positive
bool ADS1263::SetTDACPositiveOutput(bool ValueToSet)
{
	TDACConfigurationRegister RegisterToSet;
	bool Status = GetTDACPositive(&RegisterToSet);
	RegisterToSet.Active = ValueToSet;
	return Status & SetTDACPositive(&RegisterToSet);
}
bool ADS1263::SetTDACPositiveMagnitude(TDACValue ValueToSet)
{
	TDACConfigurationRegister RegisterToSet;
	bool Status = GetTDACPositive(&RegisterToSet);
	RegisterToSet.Magnitude = ValueToSet;
	return Status & SetTDACPositive(&RegisterToSet);
}
bool ADS1263::SetTDACPositive(bool PositiveActiveToSet, TDACValue PositiveMagnitudeToSet)
{
	TDACConfigurationRegister RegisterToSet;
	RegisterToSet.Active = PositiveActiveToSet;
	RegisterToSet.Magnitude = PositiveMagnitudeToSet;
	return SetTDACPositive(&RegisterToSet);
}
bool ADS1263::SetTDACPositive(TDACConfigurationRegister* RegisterToSet)
{
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->Magnitude);
	bitWrite(RegisterToSend, static_cast<uint8_t>(TDACPNRegMap::OUT), RegisterToSet->Active);
	return WriteRegister(static_cast<uint8_t>(RegisterMap::TDACP), &RegisterToSend, 1);
}
bool ADS1263::GetTDACPositive(TDACConfigurationRegister* RegisterToGet)
{
	const uint8_t MagnitudeShift = 5;
	const uint8_t ActiveBitToRead = static_cast<uint8_t>(TDACPNRegMap::OUT);
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::TDACP), &ReturnByte, 1);
	RegisterToGet->Active = (bool)bitRead(ReturnByte, ActiveBitToRead);
	RegisterToGet->Magnitude = static_cast<TDACValue>( ReturnByte >> MagnitudeShift );
	return Status;
}
//Setting test DAC negative
bool ADS1263::SetTDACNegativeOutput(bool ValueToSet)
{
	TDACConfigurationRegister RegisterToSet;
	bool Status = GetTDACNegative(&RegisterToSet);
	RegisterToSet.Active = ValueToSet;
	return Status & SetTDACNegative(&RegisterToSet);
}
bool ADS1263::SetTDACNegativeMagnitude(TDACValue ValueToSet)
{
	TDACConfigurationRegister RegisterToSet;
	bool Status = GetTDACNegative(&RegisterToSet);
	RegisterToSet.Magnitude = ValueToSet;
	return Status & SetTDACNegative(&RegisterToSet);
}
bool ADS1263::SetTDACNegative(bool NegativeActiveValueToSet, TDACValue NegativeMagnitudeToSet)
{
	TDACConfigurationRegister RegisterToSet;
	RegisterToSet.Active = NegativeActiveValueToSet;
	RegisterToSet.Magnitude = NegativeMagnitudeToSet;
	return SetTDACNegative(&RegisterToSet);
}
bool ADS1263::SetTDACNegative(TDACConfigurationRegister* RegisterToSet)
{
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->Magnitude);
	bitWrite(RegisterToSend, static_cast<uint8_t>(TDACPNRegMap::OUT), RegisterToSet->Active);
	return WriteRegister(static_cast<uint8_t>(RegisterMap::TDACN), &RegisterToSend, 1);
}
bool ADS1263::GetTDACNegative(TDACConfigurationRegister* RegisterToGet)
{
	const uint8_t MagnitudeShift = 5;
	const uint8_t ActiveBitToRead = static_cast<uint8_t>(TDACPNRegMap::OUT);
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::TDACN), &ReturnByte, 1);
	RegisterToGet->Active = (bool)bitRead(ReturnByte, ActiveBitToRead);
	RegisterToGet->Magnitude = static_cast<TDACValue>( ReturnByte >> MagnitudeShift );
	return Status;
}
//Setting GPIO connection registers
bool ADS1263::SetGPIOActive(uint8_t ChannelsToActive)
{
	return WriteRegister(static_cast<uint8_t>(RegisterMap::GPIOCON), &ChannelsToActive, 1);
}
bool ADS1263::SetGPIOActive(uint8_t Channel, bool ValueToSet)
{
	uint8_t ChannelsToActive;
	bool Status = ReadRegister(static_cast<uint8_t>(RegisterMap::GPIOCON), &ChannelsToActive, 1);
	bitWrite(ChannelsToActive, Channel, ValueToSet);
	return Status & WriteRegister(static_cast<uint8_t>(RegisterMap::GPIOCON), &ChannelsToActive, 1);
}
//Setting GPIO direction register
bool ADS1263::SetGPIODirection(uint8_t ChannelAsOutput)
{
	return WriteRegister(static_cast<uint8_t>(RegisterMap::GPIODIR), &ChannelAsOutput, 1);
}
bool ADS1263::SetGPIODirection(uint8_t Channel, bool ValueToSet)
{
	uint8_t ChannelAsOutput;
	bool Status = ReadRegister(static_cast<uint8_t>(RegisterMap::GPIODIR), &ChannelAsOutput, 1);
	bitWrite(ChannelAsOutput, Channel, ValueToSet);
	return Status & WriteRegister(static_cast<uint8_t>(RegisterMap::GPIODIR), &ChannelAsOutput, 1);
}
//Set GPIO output
bool ADS1263::SetGPIOOutput(uint8_t ChannelsHigh)
{
	return WriteRegister(static_cast<uint8_t>(RegisterMap::GPIODAT), &ChannelsHigh, 1);
}
bool ADS1263::SetGPIOOutput(uint8_t Channel, bool ValueToSet)
{
	uint8_t ChannelsHigh;
	bool Status = ReadRegister(static_cast<uint8_t>(RegisterMap::GPIODAT), &ChannelsHigh, 1);
	bitWrite(ChannelsHigh, Channel, ValueToSet);
	return Status & WriteRegister(static_cast<uint8_t>(RegisterMap::GPIODAT), &ChannelsHigh, 1);
}
//Set ADC2 configuration
bool ADS1263::SetADC2DataRate(ADC2DataRateValue ValueToSet)
{
	ADC2ConfigurationRegister RegisterToSet;
	bool Status = GetADC2Configuration(&RegisterToSet);
	RegisterToSet.DataRate = ValueToSet;
	return Status & SetADC2Configuration(&RegisterToSet);
}
bool ADS1263::SetADC2Reference(ADC2Reference ValueToSet)
{
	ADC2ConfigurationRegister RegisterToSet;
	bool Status = GetADC2Configuration(&RegisterToSet);
	RegisterToSet.Reference = ValueToSet;
	return Status & SetADC2Configuration(&RegisterToSet);
}
bool ADS1263::SetADC2Gain(ADC2GainValue ValueToSet)
{
	ADC2ConfigurationRegister RegisterToSet;
	bool Status = GetADC2Configuration(&RegisterToSet);
	RegisterToSet.Gain = ValueToSet;
	return Status & SetADC2Configuration(&RegisterToSet);
}
bool ADS1263::SetADC2Configuration(ADC2DataRateValue DataRateToSet, ADC2Reference ReferenceToSet, ADC2GainValue GainToSet)
{
	ADC2ConfigurationRegister RegisterToSet;
	RegisterToSet.DataRate = DataRateToSet;
	RegisterToSet.Reference = ReferenceToSet;
	RegisterToSet.Gain = GainToSet;
	return SetADC2Configuration(&RegisterToSet);
}
bool ADS1263::SetADC2Configuration(ADC2ConfigurationRegister* RegisterToSet)
{
	const uint8_t DataRateShift = static_cast<uint8_t>(ADC2CFGRegMap::DR2);
	const uint8_t ReferenceInputShift = static_cast<uint8_t>(ADC2CFGRegMap::REF2);
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->Gain);
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToSet->Reference) << ReferenceInputShift );
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToSet->DataRate) << DataRateShift );
	ADC2GainSetting = RegisterToSet->Gain;
	ADC2DataRate = RegisterToSet->DataRate;
	return WriteRegister(static_cast<uint8_t>(RegisterMap::ADC2CFG), &RegisterToSend, 1);
}
bool ADS1263::GetADC2Configuration(ADC2ConfigurationRegister* RegisterToGet)
{
	const uint8_t DataRateShift = static_cast<uint8_t>(ADC2CFGRegMap::DR2);
	const uint8_t ReferenceInputMask = (1 << static_cast<uint8_t>(ADC2CFGRegMap::DR2) ) - (1 << static_cast<uint8_t>(ADC2CFGRegMap::REF2));
	const uint8_t ReferenceInputShift = static_cast<uint8_t>(ADC2CFGRegMap::REF2);
	const uint8_t GainMask = (1 << static_cast<uint8_t>(ADC2CFGRegMap::REF2)) - 1;
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::ADC2CFG), &ReturnByte, 1);
	RegisterToGet->DataRate = static_cast<ADC2DataRateValue>(ReturnByte >> DataRateShift);
	RegisterToGet->Reference = static_cast<ADC2Reference>( (ReturnByte & ReferenceInputMask) >> ReferenceInputShift );
	RegisterToGet->Gain = static_cast<ADC2GainValue>(ReturnByte & GainMask);
	ADC2GainSetting = RegisterToGet->Gain;
	ADC2DataRate = RegisterToGet->DataRate;
	return Status;
}
//Set ADC multiplexer
bool ADS1263::SetADC2PositiveChannel(InputMUXValue ValueToSet)
{
	ADC2MultiplexerRegister RegisterToSet;
	bool Status = GetADC2Multiplexer(&RegisterToSet);
	RegisterToSet.PositiveChannel = ValueToSet;
	return Status & SetADC2Multiplexer(&RegisterToSet);
}
bool ADS1263::SetADC2NegativeChannel(InputMUXValue ValueToSet)
{
	ADC2MultiplexerRegister RegisterToSet;
	bool Status = false;
	Status = GetADC2Multiplexer(&RegisterToSet);
	RegisterToSet.NegativeChannel = ValueToSet;
	return Status & SetADC2Multiplexer(&RegisterToSet);
}
bool ADS1263::SetADC2Multiplexer(InputMUXValue PositiveChannelToSet, InputMUXValue NegativeChannelToSet)
{
	ADC2MultiplexerRegister RegisterToSet;
	RegisterToSet.PositiveChannel = PositiveChannelToSet;
	RegisterToSet.NegativeChannel = NegativeChannelToSet;
	return SetADC2Multiplexer(&RegisterToSet);
}
bool ADS1263::SetADC2Multiplexer(ADC2MultiplexerRegister* RegisterToSet)
{
	const uint8_t PositiveChannelShift = static_cast<uint8_t>(ADC2MUX::MUXP2);
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->NegativeChannel);
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToSet->PositiveChannel) << PositiveChannelShift);
	return WriteRegister(static_cast<uint8_t>(RegisterMap::ADC2MUX), &RegisterToSend, 1);
}
bool ADS1263::GetADC2Multiplexer(ADC2MultiplexerRegister* RegisterToGet)
{
	const uint8_t PositiveChannelShift = static_cast<uint8_t>(ADC2MUX::MUXP2);
	const uint8_t NegativeChannelMask = ( (1 << static_cast<uint8_t>(ADC2MUX::MUXP2) ) - 1);
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::ADC2MUX), &ReturnByte, 1);
	RegisterToGet->PositiveChannel = static_cast<InputMUXValue>(ReturnByte >> PositiveChannelShift );
	RegisterToGet->NegativeChannel = static_cast<InputMUXValue>(ReturnByte & NegativeChannelMask);
	return Status;
}
bool ADS1263::Send(OpCodesSimple OpCodeToSend)
{
	uint8_t ByteToSend = static_cast<uint8_t>(OpCodeToSend);
	SPI.beginTransaction(ConnectionSettings);
	digitalWrite(CSSelectPin,LOW);
	SPI.transfer(ByteToSend);
	digitalWrite(CSSelectPin, HIGH);
	SPI.endTransaction();
	if (Verbose)
	{
		Serial.print("[ADS1263] out: ");
		Serial.print(ByteToSend, HEX);
		Serial.print('\n');
	}
	return true;
}
bool ADS1263::SendRecieve(OpCodesSimple OpCodeToSend, uint8_t* ReceivedData, uint8_t BytesToRecieve)
{
	SPI.beginTransaction(ConnectionSettings);
	digitalWrite(CSSelectPin,LOW);
	uint8_t ByteToSend = static_cast<uint8_t>(OpCodeToSend);
	SPI.transfer(ByteToSend);
	for (uint8_t Index = 0; Index < BytesToRecieve; ++Index)
	{
		ReceivedData[Index] = SPI.transfer(0);
	}
	digitalWrite(CSSelectPin, HIGH);
	SPI.endTransaction();
	if (Verbose)
	{
		Serial.print("[ADS1263] out: ");
		Serial.print(ByteToSend, HEX);
		Serial.print('\n');
		Serial.print("[ADS1263] in: ");
		for (uint8_t Index = 0; Index < BytesToRecieve; ++Index)
		{
			Serial.print(ReceivedData[Index], HEX);
			Serial.print(',');
		}
		Serial.print('\n');
	}
	return true;
}
bool ADS1263::ReadRegister(uint8_t RegisterAddress, uint8_t* ReceivedData, uint8_t BytesToRecieve)
{
	if (BytesToRecieve > 0)
	{
		uint8_t OpCodeToSend = static_cast<uint8_t>(OpCodesComplex::RREG) | RegisterAddress;
		uint8_t ByteCountToSend = BytesToRecieve - 1;
		SPI.beginTransaction(ConnectionSettings);
		digitalWrite(CSSelectPin,LOW);
		SPI.transfer(OpCodeToSend);
		SPI.transfer(ByteCountToSend);
		for (uint8_t Index = 0; Index < BytesToRecieve; ++Index)
		{
			ReceivedData[Index] = SPI.transfer(0);
		}
		digitalWrite(CSSelectPin, HIGH);
		SPI.endTransaction();
		if (Verbose)
		{
			Serial.print("[ADS1263] out: ");
			Serial.print(OpCodeToSend, HEX);
			Serial.print(',');
			Serial.print(ByteCountToSend, HEX);
			Serial.print('\n');
			Serial.print("[ADS1263] in: ");
			for (uint8_t Index = 0; Index < BytesToRecieve; ++Index)
			{
				Serial.print(ReceivedData[Index], HEX);
				Serial.print(',');
			}
			Serial.print('\n');
		}
		return true;
	}
	else
	{
		return true;
	}
}
bool ADS1263::WriteRegister(uint8_t RegisterAddress, uint8_t* WriteData, uint8_t BytesToSend)
{
	if (BytesToSend > 0)
	{
		uint8_t OpCodeToSend = static_cast<uint8_t>(OpCodesComplex::WREG) | RegisterAddress;
		uint8_t ByteCountToSend = BytesToSend - 1;
		//Serial.print("[ADS1263] out: ");
		//Serial.print(OpCodeToSend, HEX);
		//Serial.print(',');
		//Serial.print(ByteCountToSend, HEX);
		//Serial.print(',');
		//for (uint8_t Index = 0; Index < BytesToSend; ++Index)
		//{
		//	Serial.print(WriteData[Index], HEX);
		//	Serial.print(',');
		//}
		//Serial.print('\n');
		SPI.beginTransaction(ConnectionSettings);
		digitalWrite(CSSelectPin,LOW);
		SPI.transfer(OpCodeToSend);
		SPI.transfer(ByteCountToSend);
		for (uint8_t Index = 0; Index < BytesToSend; ++Index)
		{
			SPI.transfer(WriteData[Index]);
		}
		digitalWrite(CSSelectPin, HIGH);
		SPI.endTransaction();
		if (Verbose)
		{
			Serial.print("[ADS1263] out: ");
			Serial.print(OpCodeToSend, HEX);
			Serial.print(',');
			Serial.print(ByteCountToSend, HEX);
			Serial.print(',');
			for (uint8_t Index = 0; Index < BytesToSend; ++Index)
			{
				Serial.print(WriteData[Index], HEX);
				Serial.print(',');
			}
			Serial.print('\n');
		}
		return true;
	}
	else
	{
		return true;
	}
}
SPISettings* ADS1263::GetSPISettings()
{
	return &ConnectionSettings;
}