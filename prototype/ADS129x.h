/* Library for accessing the ADS129x chip */

#include <stdint.h>
#include "ads1299.h"

class ADS129xChip{
  private:

  int ipinDRDY;

  int ipinSlaveCS;

  int ipinMasterCS;

  bool setGPIOToOutput;

  bool leadOffSensingEnabled;

  bool sharedNegativeElectrode;

  int cacheChipId;

  enum { master = 1, slave = 2 };
  void sendCommand(int cmd, int target = master | slave);

  uint8_t readRegister(int reg);

  void writeRegister(int reg, int val, int target = master | slave);

  public:

  ADS129xChip();

  ~ADS129xChip();

  void init();

  bool getFrames(ADS1299::Data_frame *frames, size_t len);

  int chipId();
};

extern ADS129xChip ADS129x;
