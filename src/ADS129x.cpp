/* Library for accessing the ADS129x chip*/

#include "ADS129x.h"
#include <math.h>

const int maxChannels = 8; //set the maximum nr of channels here
ADS129xChip ADS129x;

void ADS129xChip::init(){
}

float ADS129xChip::getVolts(int channel){
    if(channel < 1 || channel > maxChannels){
         return NAN;}
    return 0;
}
