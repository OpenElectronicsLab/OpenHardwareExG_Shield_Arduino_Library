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

  int cacheChipId;

  unsigned long sampleCounter;

  unsigned long lastSampleMicros;

  float *lastSample;

  float *gain;

  enum { master = 1, slave = 2 };
  void sendCommand(int cmd, int target = master | slave);

  uint8_t readRegister(int reg);

  void writeRegister(int reg, int val, int target = master | slave);

  bool updateData();

  public:

  ADS129xChip();

  ~ADS129xChip();

  void init();

  float getVolts(int channel);

  unsigned long timeOfSample();

  unsigned long sampleCount();

  int chipId();

};

extern ADS129xChip ADS129x;
