#ifndef A121_DISTANCE_DETECTOR_H
#define A121_DISTANCE_DETECTOR_H

#include "Arduino.h"
#include "Wire.h"

#define A121_WAKEUP_PIN 9
#define A121_BUSY_PIN 8

#define A121_DEV_ADDR 0x52

#define REG_VERSION                          0x0000
#define REG_PROTOCOL_STATUS                  0x0001
#define REG_MEASURE_COUNTER                  0x0002
#define REG_DETECTOR_STATUS                  0x0003

#define REG_DISTANCE_RESULT                  0x0010

#define REG_PEAK0_DISTANCE                   0x0011
#define REG_PEAK1_DISTANCE                   0x0012
#define REG_PEAK2_DISTANCE                   0x0013
#define REG_PEAK3_DISTANCE                   0x0014
#define REG_PEAK4_DISTANCE                   0x0015
#define REG_PEAK5_DISTANCE                   0x0016
#define REG_PEAK6_DISTANCE                   0x0017
#define REG_PEAK7_DISTANCE                   0x0018
#define REG_PEAK8_DISTANCE                   0x0019
#define REG_PEAK9_DISTANCE                   0x001A

#define REG_PEAK0_STRENGTH                   0x001B
#define REG_PEAK1_STRENGTH                   0x001C
#define REG_PEAK2_STRENGTH                   0x001D
#define REG_PEAK3_STRENGTH                   0x001E
#define REG_PEAK4_STRENGTH                   0x001F
#define REG_PEAK5_STRENGTH                   0x0020
#define REG_PEAK6_STRENGTH                   0x0021
#define REG_PEAK7_STRENGTH                   0x0022
#define REG_PEAK8_STRENGTH                   0x0023
#define REG_PEAK9_STRENGTH                   0x0024

#define REG_START                            0x0040
#define REG_END                              0x0041
#define REG_MAX_STEP_LENGTH                  0x0042
#define REG_CLOSE_RANGE_LEAKAGE_CANCEL       0x0043
#define REG_SIGNAL_QUALITY                   0x0044
#define REG_MAX_PROFILE                      0x0045
#define REG_THRESHOLD_METHOD                 0x0046
#define REG_PEAK_SORTING                     0x0047
#define REG_NUM_FRAMES_RECORDED_THRESHOLD    0x0048
#define REG_FIXED_AMPLITUDE_THRESHOLD_VALUE  0x0049
#define REG_THRESHOLD_SENSITIVITY            0x004A
#define REG_REFLECTOR_SHAPE                  0x004B
#define REG_FIXED_STRENGTH_THRESHOLD_VALUE   0x004C

#define REG_MEASURE_ON_WAKEUP                0x0080

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
    PROFILE1          = (1),
    PROFILE2,
    PROFILE3,
    PROFILE4,
    PROFILE5,

} sensor_profile_t;

typedef enum
{
    FIXED_AMPLITUDE          = (1),
    RECORDED,
    CFAR,
    FIXED_STRENGTH,
} sensor_threshold_method_t;

typedef enum
{
    CLOSEST          = (1),
    STRONGEST,
} sensor_peak_sorting_t;

typedef enum
{
    GENERIC          = (1),
    PLANAR,
} sensor_reflector_shape_t;


typedef enum
{
    RSS_REGISTER_OK           = (1UL << 0),
    CONFIG_CREATE_OK          = (1UL << 1),
    SENSOR_CREATE_OK          = (1UL << 2),
    DETECTOR_CREATE_OK        = (1UL << 3),
    DETECTOR_BUFFER_OK        = (1UL << 4),
    SENSOR_BUFFER_OK          = (1UL << 5),
    CALIBRATION_BUFFER_OK     = (1UL << 6),
    CONFIG_APPLY_OK           = (1UL << 7),
    SENSOR_CALIBRATE_OK       = (1UL << 8),
    DETECTOR_CALIBRATE_OK     = (1UL << 9),

    RSS_REGISTER_ERROR        = (1UL << 16),
    CONFIG_CREATE_ERROR       = (1UL << 17),
    SENSOR_CREATE_ERROR       = (1UL << 18),
    DETECTOR_CREATE_ERROR     = (1UL << 19),
    DETECTOR_BUFFER_ERROR     = (1UL << 20),
    SENSOR_BUFFER_ERROR       = (1UL << 21),
    CALIBRATION_BUFFER_ERROR  = (1UL << 22),
    CONFIG_APPLY_ERROR        = (1UL << 23),
    SENSOR_CALIBRATE_ERROR    = (1UL << 24),
    DETECTOR_CALIBRATE_ERROR  = (1UL << 25),
    DETECTOR_ERROR            = (1UL << 28),

    DETECTOR_BUSY             = (1UL << 31),
} sensor_status_t;

#define ALL_ERROR ( RSS_REGISTER_ERROR | CONFIG_CREATE_ERROR | \
                  SENSOR_CREATE_ERROR | DETECTOR_CREATE_ERROR | \
                  DETECTOR_BUFFER_ERROR | SENSOR_BUFFER_ERROR | \
                  CALIBRATION_BUFFER_ERROR | CONFIG_APPLY_ERROR | \
                  SENSOR_CALIBRATE_ERROR | DETECTOR_CALIBRATE_ERROR | \
                  DETECTOR_ERROR | DETECTOR_BUSY ) 
typedef enum
{
    NUM_DISTANCES             = 0x0F,
    NEAR_START_EDGE           = (1UL << 8),
    CALIBRATION_NEEDED        = (1UL << 9),
    MEASURE_DISTANCE_ERROR    = (1UL << 10),
    TEMPERATURE               = 0xFFFF0000,
} sensor_distance_result_t;

typedef enum
{
    APPLY_CONFIG_AND_CALIBRATE = 1,
    MEASURE_DISTANCE           = 2,
    APPLY_CONFIGURATION        = 3,
    CALIBRATE                  = 4,
    RECALIBRATE                = 5,

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
void A121_Get_Distance_mm();

#endif /* A121_DISTANCE_DETECTOR_H */
