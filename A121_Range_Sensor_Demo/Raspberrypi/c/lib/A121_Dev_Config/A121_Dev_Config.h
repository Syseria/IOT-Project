#ifndef _A121_DEV_CONFIG_H_
#define _A121_DEV_CONFIG_H_

#include "DEV_Config.h"

// #define A121_DISTANCE_DETECTOR
// #define A121_PRESENCE_DETECTOR
// #define A121_CARGO_EXAMPLE
// #define A121_BREATHING_REFERENCE

#define DEV_I2C 1
#define DEV_UART 0

#define A121_WAKEUP_PIN 17
#define A121_BUSY_PIN 4

#define A121_DEV_ADDR 0x52

void A121_I2C_Write_Byte(uint16_t reg, uint32_t data);
void A121_I2C_Read_Byte(uint16_t reg, uint32_t *data);

#endif
