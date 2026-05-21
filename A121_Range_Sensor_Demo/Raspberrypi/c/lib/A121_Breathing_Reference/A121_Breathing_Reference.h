#ifndef A121_DISTANCE_DETECTOR_H
#define A121_DISTANCE_DETECTOR_H

#include <stdbool.h>
#include "A121_Dev_Config.h"

#ifdef A121_BREATHING_REFERENCE


#define USE_PRESENCE_PROCESSOR true

// Read Only
#define REG_VERSION                        0x0000
#define REG_PROTOCOL_STATUS                0x0001
#define REG_MEASURE_COUNTER                0x0002
#define REG_APP_STATUS                     0x0003
#define REG_BREATHING_RESULT               0x0010
#define REG_BREATHING_RATE                 0x0011
#define REG_APP_STATE                      0x0012

// Read / Write
#define REG_START                          0x0040
#define REG_END                            0x0041
#define REG_NUM_DISTANCES_TO_ANALYZE       0x0042
#define REG_DISTANCE_DETERMINATION_DURATION_S  0x0043
#define REG_USE_PRESENCE_PROCESSOR         0x0044
#define REG_LOWEST_BREATHING_RATE          0x0045
#define REG_HIGHEST_BREATHING_RATE         0x0046
#define REG_TIME_SERIES_LENGTH_S           0x0047
#define REG_FRAME_RATE                     0x0048
#define REG_SWEEPS_PER_FRAME               0x0049
#define REG_HWAAS                          0x004A
#define REG_PROFILE                        0x004B
#define REG_INTRA_DETECTION_THRESHOLD      0x004C

// Write Only
#define REG_COMMAND                        0x0100

// Read Only
#define REG_APPLICATION_ID                 0xFFFF


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
    RSS_REGISTER_OK          = (1U << 0),
    CONFIG_CREATE_OK         = (1U << 1),
    SENSOR_CREATE_OK         = (1U << 2),
    SENSOR_CALIBRATE_OK      = (1U << 3),
    APP_CREATE_OK            = (1U << 4),
    APP_BUFFER_OK            = (1U << 5),
    SENSOR_BUFFER_OK         = (1U << 6),
    CONFIG_APPLY_OK          = (1U << 7),

    RSS_REGISTER_ERROR       = (1U << 16),
    CONFIG_CREATE_ERROR      = (1U << 17),
    SENSOR_CREATE_ERROR      = (1U << 18),
    SENSOR_CALIBRATE_ERROR   = (1U << 19),
    APP_CREATE_ERROR         = (1U << 20),
    APP_BUFFER_ERROR         = (1U << 21),
    SENSOR_BUFFER_ERROR      = (1U << 22),
    CONFIG_APPLY_ERROR       = (1U << 23),
    APP_ERROR                = (1U << 28),

    APP_BUSY                     = (1U << 31),

} sensor_status_t;


#define ALL_ERROR ( RSS_REGISTER_ERROR | CONFIG_CREATE_ERROR | \
                  SENSOR_CREATE_ERROR  | APP_CREATE_ERROR | \
                  SENSOR_BUFFER_ERROR  | APP_BUFFER_ERROR | \
                  CONFIG_APPLY_ERROR   | SENSOR_CALIBRATE_ERROR | \
                  APP_ERROR | APP_BUSY ) 

typedef enum
{
    RESULT_READY               = 0x01,
    RESULT_READY_STICKY        = 0x02,
    TEMPERATURE                = 0xFFFF0000,
} sensor_breathing_result_t;

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
    START_APP                  = 2,
    STOP_APP                   = 3,

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
void A121_Get_Breathing_application();

#endif /* A121_DISTANCE_DETECTOR_H */
#endif