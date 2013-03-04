#ifndef __HWACCELMANAGER_H__
#define __HWACCELMANAGER_H__

//#include "global.h"

class AVGRABBER_EXPORT HwAccelManager
{
public:
   HwAccelManager();
   ~HwAccelManager();

   /*
   The acceleration level may be one of the following:
    0x0 - full acceleration
    0x1 - ...
    ...
    0x5 - no acceleration.
   */
   bool SetAccelerationLevel(long ulMode);

   /*
   Returns the acceleration level noted in the registry.
   */
   long GetAccelerationLevel();

   bool IsEnabled();

private:
   bool OpenAccelerationLevel(PHKEY phkDevice, LPDWORD pdwAccLevel);

   bool isEnabled_;
};

#endif
