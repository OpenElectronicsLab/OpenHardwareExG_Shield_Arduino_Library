/* Single Sample takes a single sample from channel 1 of the OpenHardwareExG device. */

#include <SPI.h>
#include <ADS129x.h>

void setup() {
  // Open serial communications
  Serial.begin(57600);

  // Initialize OpenHardwareExG board and get potential in volts from Channel 1
  ADS129x.init();
  Serial.print(ADS129x.getVolts(1));
  Serial.println(" volts");
}
