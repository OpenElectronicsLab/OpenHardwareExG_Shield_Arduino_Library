/* Library for accessing the ADS129x chip*/

#include <Arduino.h>
#include <SPI.h>

#include "ADS129x.h"
#include <math.h>
#include "ads1299.h"

#define SPI_SPEED_MAX 4000000

ADS129xChip ADS129x;
const SPISettings spiSettings(SPI_SPEED_MAX, MSBFIRST, SPI_MODE1);

ADS129xChip::ADS129xChip()
{
}

ADS129xChip::~ADS129xChip()
{
}

void ADS129xChip::init()
{
	ipinDRDY = 5;
	ipinSlaveCS = 6;
	ipinMasterCS = 7;
	setGPIOToOutput = false;
	leadOffSensingEnabled = false;
	sharedNegativeElectrode = false;

	using namespace ADS1299;

	// set up inputs and outpus
	pinMode(ipinMasterCS, OUTPUT);
	pinMode(ipinSlaveCS, OUTPUT);
	pinMode(ipinDRDY, INPUT);

	SPI.begin();

	// Wait for 33 milliseconds (we will use 100 millis)
	//  for Power-On Reset and Oscillator Start-Up
	delay(100);

	// Reset the device and exit read data continuous mode.
	// "If the device is in RDATAC mode, a SDATAC command must be issued
	// before any other commands can be sent to the device."
	// (page 36 of the ADS1299 data sheet)
	sendCommand(SDATAC);
	delay(1);
	sendCommand(RESET);
	delay(1);
	sendCommand(SDATAC);
	delay(1);

	cacheChipId = readRegister(ID);

	writeRegister(CONFIG1, CONFIG1_const | DR_250_SPS | CLK_EN_enabled |
		      DAISY_EN_enabled, master);

	delay(1);
	sendCommand(SDATAC, slave);
	delay(1);
	sendCommand(RESET, slave);
	delay(1);
	sendCommand(SDATAC, slave);
	delay(1);

	writeRegister(CONFIG1, CONFIG1_const | DR_250_SPS | DAISY_EN_enabled,
		      slave);

	if (setGPIOToOutput) {
		// All GPIO set to output 0x0000
		// (floating CMOS inputs can flicker on and off, creating noise)
		writeRegister(GPIO, 0x00);
	}
	//writeRegister(CONFIG2, INT_TEST);    // generate internal test signals

	// Power up the internal reference and wait for it to settle
	writeRegister(CONFIG3,
		      PD_REFBUF_reference_buffer_enabled |
		      PD_BIAS_bias_buffer_enabled |
		      BIAS_LOFF_SENS | CONFIG3_const, master);

	writeRegister(CONFIG3,
		      PD_REFBUF_reference_buffer_enabled |
		      PD_BIAS_bias_buffer_powered_down |
		      BIAS_LOFF_SENS | CONFIG3_const, slave);
	delay(150);

	if (leadOffSensingEnabled) {
		// Use lead-off sensing in all channels (but only drive one of the
		// negative leads if all of them are connected to one electrode)
		writeRegister(CONFIG4, CONFIG4_const | PD_LOFF_COMP);
		writeRegister(LOFF, COMP_TH_80 | ILEAD_OFF_24nA);
		writeRegister(LOFF_SENSP, 0xFF);
		writeRegister(LOFF_SENSN,
			      sharedNegativeElectrode ? 0x01 : 0xFF);
	}
	// If we want to share a single negative electrode, tie the negative
	// inputs together using the BIAS_IN line.
	uint8_t mux = sharedNegativeElectrode ? BIAS_DRN : ELECTRODE_INPUT;

	// connect the negative channel to the (shared) BIAS_IN line
	// Set the first liveChannelsNum channels to input signal
	//TODO change to accept a list of live channels
	//NOTE This is to master and all slaves
	for (size_t i = 1; i <= 8; ++i) {
		writeRegister(CHnSET + i, mux | GAIN_12X);
		// writeRegister(CHnSET + i, TEST_SIGNAL | GAIN_12X);
		// writeRegister(CHnSET + i, SHORTED | PDn);
	}

	// now start reading data continuously mode
	sendCommand(RDATAC);
	delay(1);
	sendCommand(START);
}

int ADS129xChip::chipId()
{
	return cacheChipId;
}

bool ADS129xChip::getFrames(ADS1299::Data_frame *frames, size_t len)
{

	if (digitalRead(ipinDRDY) == HIGH) {
		return false;
	}

	using namespace ADS1299;

	SPI.beginTransaction(spiSettings);
	digitalWrite(ipinMasterCS, LOW);
	digitalWrite(ipinSlaveCS, LOW);

	for (size_t i = 0; (i/8) < len; i += 8) {
		Data_frame &frame = frames[i/8];
		for (int j = 0; j < frame.size; ++j) {
			frame.data[j] = SPI.transfer(0);
		}
	}

	delayMicroseconds(1);
	digitalWrite(ipinSlaveCS, HIGH);
	digitalWrite(ipinMasterCS, HIGH);
	SPI.endTransaction();

	return true;
}

void ADS129xChip::sendCommand(int cmd, int target)
{
	SPI.beginTransaction(spiSettings);
	if (target & master) {
		digitalWrite(ipinMasterCS, LOW);
	}
	if (target & slave) {
		digitalWrite(ipinSlaveCS, LOW);
	}

	SPI.transfer(cmd);

	delayMicroseconds(1);
	if (target & master) {
		digitalWrite(ipinMasterCS, HIGH);
	}
	if (target & slave) {
		digitalWrite(ipinSlaveCS, HIGH);
	}
	SPI.endTransaction();
}

uint8_t ADS129xChip::readRegister(int reg)
{
	uint8_t val;

	SPI.beginTransaction(spiSettings);
	digitalWrite(ipinMasterCS, LOW);

	SPI.transfer(ADS1299::RREG | reg);
	SPI.transfer(0);	// number of registers to be read/written
	val = SPI.transfer(0);

	delayMicroseconds(1);
	digitalWrite(ipinMasterCS, HIGH);
	SPI.endTransaction();

	return val;
}

void ADS129xChip::writeRegister(int reg, int val, int target)
{
	SPI.beginTransaction(spiSettings);
	if (target & master) {
		digitalWrite(ipinMasterCS, LOW);
	}
	if (target & slave) {
		digitalWrite(ipinSlaveCS, LOW);
	}

	SPI.transfer(ADS1299::WREG | reg);
	SPI.transfer(0);	// number of registers to be read/written
	SPI.transfer(val);

	delayMicroseconds(1);
	if (target & master) {
		digitalWrite(ipinMasterCS, HIGH);
	}
	if (target & slave) {
		digitalWrite(ipinSlaveCS, HIGH);
	}
	SPI.endTransaction();
}
