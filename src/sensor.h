#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

int         sensor_init(void);
uint32_t    sensor_read(void);

#endif // SENSOR_H
