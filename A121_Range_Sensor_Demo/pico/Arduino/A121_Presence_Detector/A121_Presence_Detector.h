#ifndef A121_DISTANCE_DETECTOR_H
#define A121_DISTANCE_DETECTOR_H

#include "Arduino.h"
#include "Wire.h"

#define A121_SDA_PIN 6
#define A121_SCL_PIN 7
#define A121_WAKEUP_PIN 9
#define A121_BUSY_PIN 8

#define AUTOMATIC_SUBSWEEPS true
#define OVERRIDE true
#define INTRA_MOTION_DETECTION true
#define INTER_MOTION_DETECTION true

#define A121_DEV_ADDR 0x52

#define REG_VERSION                          0x0000
#define REG_PROTOCOL_STATUS                  0x0001
#define REG_MEASURE_COUNTER                  0x0002
#define REG_DETECTOR_STATUS                  0x0003

#define REG_PRESENCE_RESULT                  0x0010
#define REG_PRESENCE_DISTANCE                0x0011
#define REG_INTRA_PRESENCE_SCORE             0x0012
#define REG_INTER_PRESENCE_SCORE             0x0013
#define REG_PRESENCE_ACTUAL_FRAME_RATE       0x0020

#define REG_SWEEPS_PER_FRAME                 0x0040
#define REG_INTER_FRAME_PRESENCE_TIMEOUT     0x0041
#define REG_INTRA_DETECTION_ENABLED          0x0043
#define REG_INTER_DETECTION_ENABLED          0x0044
#define REG_FRAME_RATE                       0x0045
#define REG_INTRA_DETECTION_THRESHOLD        0x0046
#define REG_INTER_DETECTION_THRESHOLD        0x0047
#define REG_INTER_FRAME_DEVIATION_TIME_CONST 0x0048
#define REG_INTER_FRAME_FAST_CUTOFF          0x0049
#define REG_INTER_FRAME_SLOW_CUTOFF          0x004A
#define REG_INTRA_FRAME_TIME_CONST           0x004B
#define REG_INTRA_OUTPUT_TIME_CONST          0x004C
#define REG_INTER_OUTPUT_TIME_CONST          0x004D
#define REG_AUTO_PROFILE_ENABLED             0x004E
#define REG_AUTO_STEP_LENGTH_ENABLED         0x004F

#define REG_MANUAL_PROFILE                   0x0050
#define REG_MANUAL_STEP_LENGTH               0x0051
#define REG_START                            0x0052
#define REG_END                              0x0053
#define REG_RESET_FILTERS_ON_PREPARE         0x0054
#define REG_HWAAS                            0x0055
#define REG_AUTOMATIC_SUBSWEEPS              0x0056
#define REG_SIGNAL_QUALITY                   0x0057

#define REG_DETECTION_ON_GPIO                0x0080

#define REG_COMMAND                          0x0100
#define REG_APPLICATION_ID                   0xFFFF


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
    RSS_REGISTER_OK           = (1U << 0),
    CONFIG_CREATE_OK          = (1U << 1),
    SENSOR_CREATE_OK          = (1U << 2),
    DETECTOR_CREATE_OK        = (1U << 3),
    DETECTOR_BUFFER_OK        = (1U << 4),
    SENSOR_BUFFER_OK          = (1U << 5),
    CALIBRATION_BUFFER_OK     = (1U << 6),
    CONFIG_APPLY_OK           = (1U << 7),
    SENSOR_CALIBRATE_OK       = (1U << 8),
    DETECTOR_CALIBRATE_OK     = (1U << 9),

    RSS_REGISTER_ERROR        = (1U << 16),
    CONFIG_CREATE_ERROR       = (1U << 17),
    SENSOR_CREATE_ERROR       = (1U << 18),
    DETECTOR_CREATE_ERROR     = (1U << 19),
    DETECTOR_BUFFER_ERROR     = (1U << 20),
    SENSOR_BUFFER_ERROR       = (1U << 21),
    CALIBRATION_BUFFER_ERROR  = (1U << 22),
    CONFIG_APPLY_ERROR        = (1U << 23),
    SENSOR_CALIBRATE_ERROR    = (1U << 24),
    DETECTOR_CALIBRATE_ERROR  = (1U << 25),
    DETECTOR_ERROR            = (1U << 28),

    DETECTOR_BUSY             = (1U << 31),
} sensor_status_t;

#define ALL_ERROR ( RSS_REGISTER_ERROR | CONFIG_CREATE_ERROR | \
                  SENSOR_CREATE_ERROR | DETECTOR_CREATE_ERROR | \
                  DETECTOR_BUFFER_ERROR | SENSOR_BUFFER_ERROR | \
                  CALIBRATION_BUFFER_ERROR | CONFIG_APPLY_ERROR | \
                  SENSOR_CALIBRATE_ERROR | DETECTOR_CALIBRATE_ERROR | \
                  DETECTOR_ERROR | DETECTOR_BUSY ) 

typedef enum
{
    PRESENCE_DETECTED         = 0x01,
    PRESENCE_DETECTED_STICKY  = 0x02,
    RESULT_DETECTOR_ERROR     = (1U << 15),
    TEMPERATURE               = 0xFFFF0000,
} sensor_presence_result_t;

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
    START_DETECTOR             = 2,
    STOP_DETECTOR              = 3,

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
void A121_Get_Presence_Detector();

#endif /* A121_DISTANCE_DETECTOR_H */
