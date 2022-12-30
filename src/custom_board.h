#ifndef CUSTOM_BOARD
#define CUSTOM_BOARD

// Two LED outputs are defined
#define LEDS_NUMBER  2
#define LED_1    3
#define LED_2    20
#define LEDS_LIST   { LED_1, LED_2 }
#define BSP_LED_0  LED_1
#define BSP_LED_1  LED_2
#define LEDS_ACTIVE_STATE  0

// No buttons are defined
#define BUTTONS_NUMBER  0
#define BUTTON_PULL  NRF_GPIO_PIN_PULLUP
#define BUTTONS_LIST { }
#define BUTTONS_ACTIVE_STATE 0

// I2C lines
#define TWI_SCL_PIN    13
#define TWI_SDA_PIN    17

// Sensor power switch control (output)
#define SENSOR_PWR      28
#define SENSOR_PWR_ACTIVE_STATE 1

#endif // CUSTOM_BOARD
