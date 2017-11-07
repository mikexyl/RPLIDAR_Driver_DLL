#include "librplidar_global.h"
#include <stdio.h>
#include <stdlib.h>

#include "rplidar.h" //RPLIDAR standard sdk, all-in-one header
using namespace rp::standalone::rplidar;

bool checkRPLIDARHealth(RPlidarDriver *drv);
bool dispose(void);

RPlidarDriver * drv;
int lidarInit(const char *portname, unsigned int baudrate)
{
    u_result op_result;
    drv = RPlidarDriver::CreateDriver(RPlidarDriver::DRIVER_TYPE_SERIALPORT);

    if (!drv) {
        fprintf(stderr, "insufficent memory, exit\n");
        return (-2);
    }

    // make connection...
    if (IS_FAIL(drv->connect(portname, baudrate))) {
        fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n"
            , baudrate);
        lidarStop();
    }

    rplidar_response_device_info_t devinfo;

    // retrieving the device info
    ////////////////////////////////////////
    op_result = drv->getDeviceInfo(devinfo);

    if (IS_FAIL(op_result)) {
        fprintf(stderr, "Error, cannot get device info.\n");
        lidarStop();
    }

    // print out the device serial number, firmware and hardware version number..
    printf("RPLIDAR S/N: ");
    for (int pos = 0; pos < 16 ;++pos) {
        printf("%02X", devinfo.serialnum[pos]);
    }

    printf("\n"
            "Firmware Ver: %d.%02d\n"
            "Hardware Rev: %d\n"
            , devinfo.firmware_version>>8
            , devinfo.firmware_version & 0xFF
            , (int)devinfo.hardware_version);

    // check health...
    if (!checkRPLIDARHealth(drv)) {
        lidarStop();
    }
}

bool checkRPLIDARHealth(RPlidarDriver *drv)
{
    u_result     op_result;
    rplidar_response_device_health_t healthinfo;


    op_result = drv->getHealth(healthinfo);
    if (IS_OK(op_result)) { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
        printf("RPLidar health status : %d\n", healthinfo.status);
        if (healthinfo.status == RPLIDAR_STATUS_ERROR) {
            fprintf(stderr, "Error, rplidar internal error detected. Please reboot the device to retry.\n");
            // enable the following code if you want rplidar to be reboot by software
            // drv->reset();
            return false;
        } else {
            return true;
        }

    } else {
        fprintf(stderr, "Error, cannot retrieve the lidar health code: %x\n", op_result);
        return false;
    }
}

int lidarStart()
{
    drv->startMotor();
    // start scan...
    drv->startScan();
}

int lidarStop()
{
    drv->stop();
    drv->stopMotor();
    dispose();
}

bool dispose()
{
    RPlidarDriver::DisposeDriver(drv);
    return (0);
}

int lidarGetScanData(rplidar_measurement_node_t* nodes, size_t count, _u32 timeout)
{
    _u32 op_result;

    op_result = drv->grabScanData((rplidar_response_measurement_node_t*)nodes, count,timeout);
    if (IS_OK(op_result)){
        drv->ascendScanData((rplidar_response_measurement_node_t*)nodes, count);
        return 0;
    }
    else return -1;
}
