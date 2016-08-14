/* Single Sample takes a single sample from channel 1 of the OpenHardwareExG device. */

#include <ADS129x.h>

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println(hello());
}
