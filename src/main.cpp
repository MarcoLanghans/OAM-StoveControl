#include "Logic.h"
#include "StoveControl.h"
#include "OpenKNX.h"
#include "SensorDevices.h"
#include "SensorModule.h"
#include "Sensor.h"

#ifdef ARDUINO_ARCH_RP2040
    #include "FileTransferModule.h"
    #include "UsbExchangeModule.h"
#endif


void setup()
{
    const uint8_t firmwareRevision = 0;
    openknx.init(firmwareRevision);
    openknx.addModule(10, openknxLogic);
    openknx.addModule(20, openknxStoveControlModule);
    openknx.addModule(70, openknxSensorDevicesModule);
    openknx.addModule(60, openknxSensorModule);
 
#ifdef ARDUINO_ARCH_RP2040
    openknx.addModule(80, openknxUsbExchangeModule);
    openknx.addModule(90, openknxFileTransferModule);
#endif
    openknx.setup();
}


#ifdef OPENKNX_DUALCORE
void setup1()
{
    openknx.setup1();
}
#endif


void loop()
{   
    openknx.loop();   
}

#ifdef OPENKNX_DUALCORE
void loop1()
{
    openknx.loop1();
}
#endif