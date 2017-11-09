#include "librplidar_global.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
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

int lidarGetScanData(uint8_t* nodes,size_t* count, uint32_t timeout)
{
    _u32 result;
    size_t _count;
    memcpy(&_count,count,sizeof(size_t));
    rplidar_response_measurement_node_t _nodes[_count];

    result = drv->grabScanData(_nodes, *count,timeout);
    if (IS_OK(result)){
        drv->ascendScanData(_nodes, *count);
        //memcpy(nodes,_nodes,_count*sizeof(rplidar_response_measurement_node_t));
        for(int i=0;i<_count;i++){
            nodes[i*5]=_nodes[i].sync_quality;
            nodes[i*5+1]=_nodes[i].angle_q6_checkbit>>8;
            nodes[i*5+2]=(uint8_t)_nodes[i].angle_q6_checkbit;
            nodes[i*5+3]=_nodes[i].distance_q2>>8;
            nodes[i*5+4]=(uint8_t)_nodes[i].distance_q2;

//            memcpy(&nodes[i*5+1],&_nodes[i].angle_q6_checkbit,sizeof(uint16_t));
//            memcpy(&nodes[i*5+3],&_nodes[i].distance_q2,sizeof(uint16_t));
        }

        typedef struct _tmp {
            uint8_t    sync_quality;      // syncbit:1;syncbit_inverse:1;quality:6;
            uint16_t   angle_q6_checkbit; // check_bit:1;angle_q6:15;
            uint16_t   distance_q2;
        }  _tmp;

        _tmp _node[_count];
//        memcpy(_node,nodes,_count*sizeof(rplidar_response_measurement_node_t));

        for (int pos = 0; pos < *count ; ++pos) {
            _node[pos].sync_quality=nodes[pos*5];
            _node[pos].angle_q6_checkbit=nodes[pos*5+1]<<8|nodes[pos*5+2];
            _node[pos].distance_q2=nodes[pos*5+3]<<8|nodes[pos*5+4];
            printf("%s theta: %03.2f Dist: %08.2f Q: %d ",
                (_node[pos].sync_quality & RPLIDAR_RESP_MEASUREMENT_SYNCBIT) ?"S ":"  ",
                (_node[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT)/64.0f,
                _node[pos].distance_q2/4.0f,
                _node[pos].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT);
//            uint8_t nodesU8[5];
//            memcpy(nodesU8,&_nodes[pos],5*sizeof(uint8_t));
//            cout<<(int)_nodes[pos].angle_q6_checkbit<<ends;
//            for(int i=0;i<5;i++)
//                cout<<(int)nodesU8[i]<<ends;
            cout<<endl;
        }

        return 0;
    }
    else return -1;
}
