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

/* I2C address for the specific device  */
#define SHT40_ADDR       (0x44U)

/* Indicate if operation on TWI has succeeded/failed */
static volatile bool m_xfer_done = false;
static volatile bool m_xfer_nak = false;

/* TWI instance */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Buffer for samples read */
static uint64_t m_sample;

__STATIC_INLINE void data_handler(uint64_t temp)
{
    NRF_LOG_INFO("Got data: %X", temp);
}

void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
            {
                data_handler(m_sample);
            }
            
            m_xfer_done = true;
            break;
        default:
            m_xfer_nak = true;
    }
}


static void twi_init(void)
{
    ret_code_t err_code;
    
    const nrf_drv_twi_config_t twi_config = {
        .scl                = TWI_SCL_PIN,
        .sda                = TWI_SDA_PIN,
        .frequency          = NRF_DRV_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_MID,
        .clear_bus_init     = false
    };
    
    err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_twi_enable(&m_twi);
}

static uint32_t read_data_SHT40()
{
    // Write 1-byte command
    m_xfer_done = false;
    
    ((uint8_t *)&m_sample)[0] = 0xE0;   // Read temperature & humidity
    ret_code_t err_code = nrf_drv_twi_tx(&m_twi, SHT40_ADDR, (uint8_t *) &m_sample, 1, false);
    APP_ERROR_CHECK(err_code);
    
    while (!m_xfer_done) ;
    
    // Delay 10ms before reading. If too short, the SHT40 will respond with a NAK,
    // and we should really wait before trying again. Not done here.
    
    nrf_delay_ms(10);
    
    
    // Read 6 bytes of data.
    m_xfer_done = false;
    m_xfer_nak = false;
    
    err_code = nrf_drv_twi_rx(&m_twi, SHT40_ADDR, (uint8_t *) &m_sample, 6);
    APP_ERROR_CHECK(err_code);
    
    while (!m_xfer_done && !m_xfer_nak) ;


    if (m_xfer_nak)
    {
        // Read didn't complete in the time allowed
        return 0xFFFFFFFF;
    }

    // Re-arrange the bytes for output
    uint32_t res;
    
    ((uint8_t *) &res)[0] = ((uint8_t *) &m_sample)[0];
    ((uint8_t *) &res)[1] = ((uint8_t *) &m_sample)[1];
    ((uint8_t *) &res)[2] = ((uint8_t *) &m_sample)[3];
    ((uint8_t *) &res)[3] = ((uint8_t *) &m_sample)[4];
    
    return res;
}


static void set_mode_SHT40(void)
{
    // Nothing to do
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
    set_mode_SHT40();
    return 0;
}

uint32_t sensor_read(void)
{
    return read_data_SHT40();
}
