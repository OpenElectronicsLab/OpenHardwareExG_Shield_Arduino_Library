/* Library for accessing the ADS129x chip */

class ADS129xChip{
  public:

  void init();

  float getVolts(int channel);
};

extern ADS129xChip ADS129x;
