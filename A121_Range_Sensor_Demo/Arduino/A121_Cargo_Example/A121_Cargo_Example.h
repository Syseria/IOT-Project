#ifndef A121_DISTANCE_DETECTOR_H
#define A121_DISTANCE_DETECTOR_H

#include "Arduino.h"
#include "Wire.h"

#define A121_WAKEUP_PIN 9
#define A121_BUSY_PIN 8

#define USE_ACTUVATE_PRESENCE true


#define A121_DEV_ADDR 0x52

#define REG_VERSION                         0x0000
#define REG_PROTOCOL_STATUS                 0x0001
#define REG_MEASURE_COUNTER                 0x0002
#define REG_ACTUAL_PRESENCE_UPDATE_RATE     0x0003
#define REG_APPLICATION_STATUS              0x0004

#define REG_CONTAINER_SIZE                  0x0010
#define REG_ACTIVATE_UTILIZATION_LEVEL      0x0011
#define REG_UTILIZATION_SIGNAL_QUALITY      0x0012
#define REG_UTILIZATION_THRESHOLD_SENS      0x0013
#define REG_ACTIVATE_PRESENCE               0x0014
#define REG_PRESENCE_UPDATE_RATE            0x0015
#define REG_PRESENCE_SWEEPS_PER_FRAME       0x0016
#define REG_PRESENCE_SIGNAL_QUALITY         0x0017
#define REG_PRESENCE_INTER_THRESHOLD        0x0018
#define REG_PRESENCE_INTRA_THRESHOLD        0x0019

#define REG_RESULT_HEADER                   0x0020
#define REG_UTILIZATION_DISTANCE            0x0021
#define REG_UTILIZATION_LEVEL_MM            0x0022
#define REG_UTILIZATION_LEVEL_PERCENT       0x0023
#define REG_PRESENCE_DETECTED               0x0024
#define REG_MAX_INTER_PRESENCE_SCORE        0x0025
#define REG_MAX_INTRA_PRESENCE_SCORE        0x0026

#define REG_COMMAND                         0x0030

#define REG_APPLICATION_ID                  0xFFFF


typedef enum
{
    PROTOCOL_STATE_ERROR   = 0x00000001,   // Pos 0
    PACKET_LENGTH_ERROR    = 0x00000002,   // Pos 1
    ADDRESS_ERROR          = 0x00000004,   // Pos 2
    WRITE_FAILED           = 0x00000008,   // Pos 3
    WRITE_TO_READ_ONLY     = 0x00000010    // Pos 4
} sensor_protocol_error_t;

typedef enum
{
    RSS_REGISTER_OK          = (1UL << 0),
    SENSOR_CREATE_OK         = (1UL << 1),
    SENSOR_CALIBRATE_OK      = (1UL << 2),
    CARGO_CREATE_OK          = (1UL << 3),
    CARGO_CALIBRATE_OK       = (1UL << 4),
    SENSOR_BUFFER_OK         = (1UL << 5),
    CARGO_BUFFER_OK          = (1UL << 6),
    CONFIG_APPLY_OK          = (1UL << 7),

    RSS_REGISTER_ERROR       = (1UL << 8),
    SENSOR_CREATE_ERROR      = (1UL << 10),
    SENSOR_CALIBRATE_ERROR   = (1UL << 11),
    CARGO_CREATE_ERROR       = (1UL << 12),
    CARGO_CALIBRATE_ERROR    = (1UL << 13),
    SENSOR_BUFFER_ERROR      = (1UL << 14),
    CARGO_BUFFER_ERROR       = (1UL << 15),
    CONFIG_APPLY_ERROR       = (1UL << 16),
    APPLICATION_ERROR        = (1UL << 17),

    APP_BUSY                 = (1UL << 18),

} sensor_status_t;


#define ALL_ERROR ( RSS_REGISTER_ERROR     | \
                    SENSOR_CREATE_ERROR    | \
                    SENSOR_CALIBRATE_ERROR | \
                    CARGO_CREATE_ERROR     | \
                    CARGO_CALIBRATE_ERROR  | \
                    SENSOR_BUFFER_ERROR    | \
                    CARGO_BUFFER_ERROR     | \
                    CONFIG_APPLY_ERROR     | \
                    APPLICATION_ERROR      | \
                    APP_BUSY )


typedef enum
{
    UTILIZATION_LEVEL_VALID     = (1UL << 17),
    PRESENCE_VALID              = (1UL << 18),
    TEMPERATURE                 = 0x0000FFFF,
} sensor_result_header_t;

typedef enum
{
    INIT                        = 0x00,
    NO_PRESENCE                 ,
    INTRA_PRESENCE              ,
    DETERMINE_DISTANCE          ,
    ESTIMATE_BREATHING_RATE     ,
} sensor_app_state_t;


typedef enum
{
    PROFILE1          = (1),
    PROFILE2,
    PROFILE3,
    PROFILE4,
    PROFILE5,

} sensor_profile_t;

typedef enum
{
    APPLY_CONFIGURATION        = 1,
    MEASURE_UTILIZATION_LEVEL  = 4,
    MEASURE_PRESENCE           = 5,

    ENABLE_UART_LOGS           = 32,
    DISABLE_UART_LOGS          = 33,
    LOG_CONFIGURATION          = 34,

    RESET_MODULE               = 1381192737,
} sensor_command_t;


typedef enum
{
    DISTANCE_DETECTOR          = (1),
    PRESENCE_DETECTOR,
    REF_APP_BREATHING,
    EXAMPLE_CARGO,

} sensor_mode_t;

void A121_init();
void A121_Get_Cargo_application();

#endif /* A121_DISTANCE_DETECTOR_H */
