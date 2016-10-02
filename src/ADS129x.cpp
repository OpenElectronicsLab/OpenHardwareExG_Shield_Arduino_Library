/* Library for accessing the ADS129x chip*/

#include <Arduino.h>
#include <SPI.h>

#include "ADS129x.h"
#include <math.h>
#include "ads1299.h"

#ifdef  _VARIANT_ARDUINO_DUE_X_
#define SPI_CLOCK_DIVIDER_VAL 21
#else
// #define SPI_CLOCK_DIVIDER_VAL SPI_CLOCK_DIV4
#define SPI_CLOCK_DIVIDER_VAL SPI_CLOCK_DIV8
#endif

const int maxChannels = 8;	//set the maximum nr of channels here
ADS129xChip ADS129x;

void ADS129xChip::init()
{
	ipinDRDY = 5;
	ipinSlaveCS = 6;
	ipinMasterCS = 7;
	setGPIOToOutput = false;
	leadOffSensingEnabled = false;
	sharedNegativeElectrode = false;
	liveChannelsNum = 8;

	using namespace ADS1299;
	int i;

	// set up inputs and outpus
	pinMode(ipinMasterCS, OUTPUT);
	pinMode(ipinDRDY, INPUT);

	SPI.begin();

	// setup SPI (note that if you have multiple SPI devices
	// you may need different settings)
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIVIDER_VAL);
	SPI.setDataMode(SPI_MODE1);

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

	if (setGPIOToOutput) {
		// All GPIO set to output 0x0000
		// (floating CMOS inputs can flicker on and off, creating noise)
		writeRegister(GPIO, 0x00);
	}
	// Power up the internal reference and wait for it to settle
	writeRegister(CONFIG3,
		      PD_REFBUF_reference_buffer_enabled |
		      PD_BIAS_bias_buffer_enabled |
		      BIAS_LOFF_SENS |
		      CONFIG3_const);
	delay(150);

	if (leadOffSensingEnabled) {
		// Use lead-off sensing in all channels (but only drive one of the
		// negative leads if all of them are connected to one electrode)
		writeRegister(CONFIG4, PD_LOFF_COMP);
		writeRegister(LOFF, COMP_TH_80 | ILEAD_OFF_24nA);
		writeRegister(LOFF_SENSP, 0xFF);
		writeRegister(LOFF_SENSN,
			      sharedNegativeElectrode ? 0x01 : 0xFF);
	}

	uint8_t reserved = (0x01 << 4) | (0x01 << 7);
	writeRegister(CONFIG1, reserved | 0x6);	// 250 SPS
	//writeRegister(CONFIG1, reserved | 0x5); // 500 SPS
	//writeRegister(CONFIG1, reserved | 0x4); // 1k SPS
	//writeRegister(CONFIG1, reserved | 0x3); // 2k SPS
	//writeRegister(CONFIG1, reserved | 0x2); // 4k SPS

	//writeRegister(CONFIG2, INT_TEST);    // generate internal test signals

	// If we want to share a single negative electrode, tie the negative
	// inputs together using the BIAS_IN line.
	uint8_t mux = sharedNegativeElectrode ? BIAS_DRN : ELECTRODE_INPUT;

	// connect the negative channel to the (shared) BIAS_IN line
	// Set the first liveChannelsNum channels to input signal
	//TODO change to accept a list of live channels
	for (i = 1; i <= liveChannelsNum; ++i) {
		writeRegister(CHnSET + i, mux | GAIN_12X);
		// writeRegister(CHnSET + i, TEST_SIGNAL | GAIN_12X);
	}
	// Set all remaining channels to shorted inputs
	for (; i <= 8; ++i) {
		writeRegister(CHnSET + i, SHORTED | PDn);
	}

}

float ADS129xChip::getVolts(int channel)
{
	if (channel < 1 || channel > maxChannels) {
		return NAN;
	}
	return 0;
}

unsigned long ADS129xChip::timeOfSample(){
	return micros();
}


void ADS129xChip::sendCommand(int cmd)
{
	//ipinMasterCS:
	digitalWrite(ipinMasterCS, LOW);
	SPI.transfer(cmd);
	delayMicroseconds(1);
	digitalWrite(ipinMasterCS, HIGH);
}

uint8_t ADS129xChip::readRegister(int reg)
{
	uint8_t val;

	digitalWrite(ipinMasterCS, LOW);

	SPI.transfer(ADS1299::RREG | reg);
	SPI.transfer(0);	// number of registers to be read/written
	val = SPI.transfer(0);

	delayMicroseconds(1);
	digitalWrite(ipinMasterCS, HIGH);

	return val;
}

void ADS129xChip::writeRegister(int reg, int val)
{
	// ipinMasterCS
	digitalWrite(ipinMasterCS, LOW);

	// ADS1299::WREG
	SPI.transfer(ADS1299::WREG | reg);
	SPI.transfer(0);	// number of registers to be read/written
	SPI.transfer(val);
	delayMicroseconds(1);
	digitalWrite(ipinMasterCS, HIGH);
}
