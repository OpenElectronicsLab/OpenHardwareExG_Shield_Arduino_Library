/* Library for accessing the ADS129x chip */

#include <stdint.h>

class ADS129xChip{
  private:

  int ipinDRDY;

  int ipinSlaveCS;

  int ipinMasterCS;

  bool setGPIOToOutput;

  bool leadOffSensingEnabled;

  bool sharedNegativeElectrode;

  int liveChannelsNum;

  void sendCommand(int cmd);

  uint8_t readRegister(int reg);

  void writeRegister(int reg, int val);

  public:

  void init();

  float getVolts(int channel);

  unsigned long timeOfSample();
};

extern ADS129xChip ADS129x;
