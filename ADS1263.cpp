#include "ADS1263.h"

ADS1263::ADS1263(uint8_t InitCSSelectPin)
{
	CSSelectPin = InitCSSelectPin;
}
ADS1263::ADS1263(uint8_t InitCSSelectPin, uint8_t InitDataReadyPin) : ADS1263(InitCSSelectPin)
{
	DataReadyPin = InitDataReadyPin;
	UseDataReadyPin = true;
}
ADS1263::ADS1263(uint8_t InitCSSelectPin, uint8_t InitDataReadyPin, uint8_t InitStartPin) : ADS1263(InitCSSelectPin, InitDataReadyPin)
{
	StartPin = InitStartPin;
	UseStartPin = true;
}
bool ADS1263::begin()
{
	
}
bool ADS1263::check()
{
	
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
	return SetMode2($RegisterToSet);
}
bool ADS1263::SetMode2(Mode2Register* RegisterToSet)
{
	const uint8_t GainShift = static_cast<uint8_t>(Mode2RegMap::GAIN);
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->ADC1DataRate);
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToSet->ADC1Gain) << GainShift);
	bitWrite(RegisterToSend, static_cast<uint8_t>(Mode2RegMap::BYPASS), RegisterToSet->GainBypass);
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
	return Status;
}
//Setting input multiplexer
bool ADS1263::SetADC1PositiveChannel(InputMUXValue ValueToSet)
{
	InputMultiplexerRegister RegisterToSet;
	bool Status = GetInputMultiplexer(&RegisterToSet);
	RegisterToSet.PositiveChannel = ValueToSet;
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
	return SetInputMultiplexer($RegisterToSet);
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
	const uint8_t NegativeChannelMask = ( (1 << static_cast<uint8_t>(InputMUXRegMap::MUXN)) - 1);
	uint8_t ReturnByte;
	bool Status = false;
	Status = ReadRegister(static_cast<uint8_t>(RegisterMap::INPMUX), &ReturnByte, 1);
	RegisterToGet->PositiveChannel = static_cast<InputMUXValue>( ReturnByte >> PositiveChannelShift );
	RegisterToGet->NegativeChannel = static_cast<InputMUXValue>( ReturnByte & NegativeChannelMask );
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
	return SetIDACMultiplexer($RegisterToSet);
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
	RegisterToGet->Channel2 = static_cast<InputMUXValue>( ReturnByte >> Channel2Shift );
	RegisterToGet->Channel1 = static_cast<InputMUXValue>( ReturnByte & Channel1Mask );
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
	return SetIDACMagnitudes($RegisterToSet);
}
bool ADS1263::SetIDACMagnitudes(IDACMagnitudeRegister* RegisterToSet)
{
	const uint8_t Channel2Shift = static_cast<uint8_t>(IDACMAGRegMap::MAG2);
	uint8_t RegisterToSend = static_cast<uint8_t>(RegisterToSet->IDAC1Magnitude);
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToSet->IDAC2Magnitude) << Channel2Shift);
	return WriteRegister(static_cast<uint8_t>(RegisterMap::IDACMAG), &RegisterToSend, 1);
}
bool ADS1263::GetIDACMagnitudes(IDACMagnitudeRegister* RegisterToSet)
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
	return Status & SetReferenceChannelNegative(&RegisterToSet);
}
bool ADS1263::SetReferenceChannelNegative(REFMUXValue ValueToSet)
{
	ReferenceMultiplexerRegister RegisterToSet;
	bool Status = GetReferenceChannelMultiplexer(&RegisterToSet);
	RegisterToSet.NegativeChannel = ValueToSet;
	return Status & SetReferenceChannelNegative(&RegisterToSet);
}
bool ADS1263::SetReferenceChannelMultiplexer(REFMUXValue PostitiveChannelToSet, REFMUXValue NegativeChannelToSet)
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
bool ADS1263::SetTDACPositive(bool PositiveActiveValueToSet, TDACValue PositiveMagnitudeToSet)
{
	TDACConfigurationRegister RegisterToSet;
	RegisterToSet.Active = NegativeActiveValueToSet;
	RegisterToSet.Magnitude = NegativeMagnitudeToSet;
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
	RegisterToGet->Active = (bool)BitRead(ReturnByte, ActiveBitToRead);
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
	RegisterToGet->Active = (bool)BitRead(ReturnByte, ActiveBitToRead);
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
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToGet->Reference) << ReferenceInputShift );
	RegisterToSend |= ( static_cast<uint8_t>(RegisterToGet->DataRate) << DataRateShift );
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
	return Status;
}
//Set ADC multiplexer
bool ADS1263::SetADC2Positive(InputMUXValue ValueToSet)
{
	ADC2MultiplexerRegister RegisterToSet;
	bool Status = GetADC2Multiplexer(&RegisterToSet);
	RegisterToSet.PositiveChannel = ValueToSet;
	return Status & SetADC2Multiplexer(&RegisterToSet);
}
bool ADS1263::SetADC2Negative(InputMUXValue ValueToSet)
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
	RegisterToGet->NegativeChannel = static_cast<InputMUXValue>(ReturnByte & NegativeChannel);
	return Status;
}
//
bool ADS1263::SendRecieve(OpCodesSimple OpCodeToSend, uint8_t* RecievedData, uint8_t BytesToRecieve)
{
	SPI.beginTransaction(ConnectionSettings);
	digitalWrite(CSSelectPin,LOW);
	SPI.transfer(OpCodeToSend);
	for (uint8_t Index = 0; Index < BytesToSend; ++Index)
	{
		ReceivedData[Index] = SPI.transfer(0);
	}
	digitalWrite(SelectPin, HIGH);
	SPI.endTransaction();
	return true;
}
bool ADS1263::ReadRegister(uint8_t RegisterAddress, uint8_t* ReceivedData, uint8_t BytesToRecieve)
{
	if (BytesToSend > 0)
	{
		uint8_t OpCodeToSend = static_cast<uint8_t>(OpCodesComplex::RREG) | RegisterAddress;
		SPI.beginTransaction(ConnectionSettings);
		digitalWrite(CSSelectPin,LOW);
		SPI.transfer(OpCodeToSend);
		SPI.transfer(BytesToSend - 1);
		for (uint8_t Index = 0; Index < BytesToSend; ++Index)
		{
			ReceivedData[Index] = SPI.transfer(0);
		}
		digitalWrite(SelectPin, HIGH);
		SPI.endTransaction();
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
		SPI.beginTransaction(ConnectionSettings);
		digitalWrite(CSSelectPin,LOW);
		SPI.transfer(OpCodeToSend);
		SPI.transfer(BytesToSend - 1);
		for (uint8_t Index = 0; Index < BytesToSend; ++Index)
		{
			SPI.transfer(WriteData[Index]);
		}
		digitalWrite(SelectPin, HIGH);
		SPI.endTransaction();
		return true;
	}
	else
	{
		return true;
	}
}