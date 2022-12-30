#include <stdio.h>

#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "app_timer.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#include "sensor.h"

#define TWI_INSTANCE_ID     0

static void twi_init(void)
{
}

static void set_mode(void)
{
}


int sensor_init(void)
{
    // Turn on power to the sensor
    #ifdef SENSOR_PWR
        #if SENSOR_PWR_ACTIVE_STATE
            nrf_gpio_pin_set(SENSOR_PWR);
        #else
            nrf_gpio_pin_clear(SENSOR_PWR);
        #endif
        nrf_gpio_cfg_output(SENSOR_PWR);
    #endif
    
    twi_init();
    set_mode();
    return 0;
}

uint32_t sensor_read(void)
{
    return 0;
}
