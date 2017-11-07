#ifndef LIBRPLIDAR_H
#define LIBRPLIDAR_H

#include "librplidar_global.h"
#include <stdio.h>
#include <stdlib.h>

#include "rplidar.h" //RPLIDAR standard sdk, all-in-one header
using namespace rp::standalone::rplidar;

typedef struct _rplidar_measurement_node_t {
    _u8    sync_quality;      // syncbit:1;syncbit_inverse:1;quality:6;
    _u16   angle_q6_checkbit; // check_bit:1;angle_q6:15;
    _u16   distance_q2;
} __attribute__((packed)) rplidar_measurement_node_t;

bool checkRPLIDARHealth(RPlidarDriver *drv);
bool dispose(void);

LIBRPLIDARSHARED_EXPORT int lidarInit(const char* portname="////.//com3",_u32 baudrate=115200);
LIBRPLIDARSHARED_EXPORT int lidarStart(void);
LIBRPLIDARSHARED_EXPORT int lidarGetScanData(rplidar_response_measurement_node_t * nodes,size_t count, _u32 timeout = 2000);
LIBRPLIDARSHARED_EXPORT int lidarStop(void);

#endif // LIBRPLIDAR_H
