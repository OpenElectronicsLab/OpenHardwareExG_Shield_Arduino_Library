/* reads samples from the OpenHardwareExG device */

#include <SPI.h>
#include <SdFat.h>
#include "ADS129x.h"
#include "util.h"

using namespace ADS1299;

// plan to start with 10s ECG as the basis:
// record from 2 leads-on for one second tot-en-met all leads-off
const int LED_PIN = 13;
const unsigned Max_samples_no_leads = 250 * 5;
const unsigned Max_samples_per_file = 250 * (60 * 5);
const unsigned maxChannels = 8;
const size_t num_ads_chips = (maxChannels / 8);

Data_frame frames[num_ads_chips];
unsigned long sampleCount;
unsigned int samplesNoLeads;
int loopCounter;
bool recording;

SdFat sd;
SdFile file;
const size_t bufLen = 80;
char fileName[bufLen];
int sdChipSelect = 2;

void setup()
{
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);

	// Open serial communications
	Serial.begin(115200);

	// Initialize OpenHardwareExG board
	ADS129x.init();

	// init sd card
	sampleCount = 0;
	recording = 0;
	samplesNoLeads = Max_samples_no_leads;
	loopCounter = 0;

	// Initialize at the highest speed supported by the board that is
	// not over 50 MHz. Try a lower speed if SPI errors occur.
	//  sd.begin(chipSelect, SPI_SPEED);
	if (!sd.begin(sdChipSelect, SD_SCK_MHZ(50))) {
		Serial.println("initialization failed. Things to check:");
		Serial.println("* is a card inserted?");
		Serial.println("* is your wiring correct?");
		Serial.println("* is sdChipSelect pin correct?");
	} else {
		Serial.println("Wiring is correct and a card is present.");
	}
}

bool leads_on(Data_frame * frames, size_t len)
{
	bool pos_found = false;
	bool neg_found = false;

	for (size_t i = 0; i < len; ++i) {
		Data_frame frame = frames[i];
		for (size_t j = 0; j < 8; ++j) {
			pos_found = pos_found || frame.loff_statp(j);
			neg_found = neg_found || frame.loff_statn(j);
			if (pos_found && neg_found) {
				return true;
			}
		}
	}

	return false;
}

size_t sprint_hex(char *buf, size_t blen, void *data, size_t len)
{
	size_t i;
	unsigned char *cp = (unsigned char *)data;
	for (i = 0; i < len && (i * 2) < blen; ++i) {
		to_hex(*(cp + i), buf + (i * 2));
	}
	if ((i * 2) < blen) {
		buf[i * 2] = '\0';
		return i * 2;
	} else {
		buf[blen - 1] = '\0';
		return blen - 1;
	}
}

void error(const char *msg1, const char *msg2)
{
	Serial.println(msg1);
	if (msg2) {
		Serial.println(msg2);
	}
}

void open_file()
{
	unsigned long time = micros();
	size_t i = 0;

/*
	fileName[i++] = 'e';
	fileName[i++] = 'x';
	fileName[i++] = 'g';
	fileName[i++] = '_';
*/
	i += sprint_hex(fileName + i, bufLen - i, &time, sizeof(time));
	fileName[i++] = '.';
	fileName[i++] = 'd';
	fileName[i++] = 'a';
	fileName[i++] = 't';
	fileName[i++] = '\0';

	if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
		error("file.open", fileName);
		recording = false;
		digitalWrite(LED_PIN, LOW);
	} else {
		Serial.print("opened ");
		Serial.println(fileName);
		recording = true;
		digitalWrite(LED_PIN, HIGH);
	}
}

void close_file()
{
	file.close();
	Serial.print("closed ");
	Serial.println(fileName);
	recording = false;
	digitalWrite(LED_PIN, LOW);
	sampleCount = 0;
}

void log_frames_sd()
{
	char buf[bufLen];
	file.print(sampleCount);

	for (size_t i = 0; i < num_ads_chips; ++i) {
		file.write(':');
		Data_frame frame = frames[i];
		sprint_hex(buf, bufLen, frame.data, frame.size);
		file.print(buf);
	}
	file.println();
}

void notify_frames_arrival()
{
}

void loop()
{
	if ((loopCounter % 1000000) == 0) {
		Serial.print("loop count: ");
		Serial.print(loopCounter);
		if (recording) {
			Serial.println(" (recording)");
		} else {
			Serial.print(" ADS Chip ID: ");
			Serial.println(ADS129x.chipId());
		}
	}
	loopCounter++;

	if (ADS129x.getFrames(frames, num_ads_chips)) {
		if (leads_on(frames, num_ads_chips)) {
			samplesNoLeads = 0;
		} else if (samplesNoLeads < Max_samples_no_leads) {
			samplesNoLeads++;
		}

		if (!recording && samplesNoLeads == 0) {
			open_file();
		}

		if (recording) {
			sampleCount++;
			log_frames_sd();
			// notify_frames_arrival();
		}

		if (recording && (samplesNoLeads >= Max_samples_no_leads)
		    || (sampleCount >= Max_samples_per_file)) {
			close_file();
		}
	}
	// do_blue_tooth_stuff();
}
