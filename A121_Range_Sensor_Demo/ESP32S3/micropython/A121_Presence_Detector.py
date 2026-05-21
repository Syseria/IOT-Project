import time
from machine import I2C, Pin

# ===============================
# Feature switches
# ===============================
AUTOMATIC_SUBSWEEPS     = True
OVERRIDE                = True
INTRA_MOTION_DETECTION  = True
INTER_MOTION_DETECTION  = True

# ===============================
# Register map
# ===============================
REG_VERSION                          = 0x0000
REG_PROTOCOL_STATUS                  = 0x0001
REG_MEASURE_COUNTER                  = 0x0002
REG_DETECTOR_STATUS                  = 0x0003

REG_PRESENCE_RESULT                  = 0x0010
REG_PRESENCE_DISTANCE                = 0x0011
REG_INTRA_PRESENCE_SCORE             = 0x0012
REG_INTER_PRESENCE_SCORE             = 0x0013
REG_PRESENCE_ACTUAL_FRAME_RATE       = 0x0020

REG_SWEEPS_PER_FRAME                 = 0x0040
REG_INTER_FRAME_PRESENCE_TIMEOUT     = 0x0041
REG_INTRA_DETECTION_ENABLED          = 0x0043
REG_INTER_DETECTION_ENABLED          = 0x0044
REG_FRAME_RATE                       = 0x0045
REG_INTRA_DETECTION_THRESHOLD        = 0x0046
REG_INTER_DETECTION_THRESHOLD        = 0x0047
REG_INTER_FRAME_DEVIATION_TIME_CONST = 0x0048
REG_INTER_FRAME_FAST_CUTOFF          = 0x0049
REG_INTER_FRAME_SLOW_CUTOFF          = 0x004A
REG_INTRA_FRAME_TIME_CONST           = 0x004B
REG_INTRA_OUTPUT_TIME_CONST          = 0x004C
REG_INTER_OUTPUT_TIME_CONST          = 0x004D
REG_AUTO_PROFILE_ENABLED             = 0x004E
REG_AUTO_STEP_LENGTH_ENABLED         = 0x004F

REG_MANUAL_PROFILE                   = 0x0050
REG_MANUAL_STEP_LENGTH               = 0x0051
REG_START                            = 0x0052
REG_END                              = 0x0053
REG_RESET_FILTERS_ON_PREPARE         = 0x0054
REG_HWAAS                            = 0x0055
REG_AUTOMATIC_SUBSWEEPS              = 0x0056
REG_SIGNAL_QUALITY                   = 0x0057

REG_DETECTION_ON_GPIO                = 0x0080

REG_COMMAND                          = 0x0100
REG_APPLICATION_ID                   = 0xFFFF


PROTOCOL_STATE_ERROR   = 0x00000001
PACKET_LENGTH_ERROR    = 0x00000002
ADDRESS_ERROR          = 0x00000004
WRITE_FAILED           = 0x00000008
WRITE_TO_READ_ONLY     = 0x00000010


RSS_REGISTER_OK           = (1 << 0)
CONFIG_CREATE_OK          = (1 << 1)
SENSOR_CREATE_OK          = (1 << 2)
DETECTOR_CREATE_OK        = (1 << 3)
DETECTOR_BUFFER_OK        = (1 << 4)
SENSOR_BUFFER_OK          = (1 << 5)
CALIBRATION_BUFFER_OK     = (1 << 6)
CONFIG_APPLY_OK           = (1 << 7)
SENSOR_CALIBRATE_OK       = (1 << 8)
DETECTOR_CALIBRATE_OK     = (1 << 9)

RSS_REGISTER_ERROR        = (1 << 16)
CONFIG_CREATE_ERROR       = (1 << 17)
SENSOR_CREATE_ERROR       = (1 << 18)
DETECTOR_CREATE_ERROR     = (1 << 19)
DETECTOR_BUFFER_ERROR     = (1 << 20)
SENSOR_BUFFER_ERROR       = (1 << 21)
CALIBRATION_BUFFER_ERROR  = (1 << 22)
CONFIG_APPLY_ERROR        = (1 << 23)
SENSOR_CALIBRATE_ERROR    = (1 << 24)
DETECTOR_CALIBRATE_ERROR  = (1 << 25)
DETECTOR_ERROR            = (1 << 28)

DETECTOR_BUSY             = (1 << 31)

ALL_ERROR = (
    RSS_REGISTER_ERROR |
    CONFIG_CREATE_ERROR |
    SENSOR_CREATE_ERROR |
    DETECTOR_CREATE_ERROR |
    DETECTOR_BUFFER_ERROR |
    SENSOR_BUFFER_ERROR |
    CALIBRATION_BUFFER_ERROR |
    CONFIG_APPLY_ERROR |
    SENSOR_CALIBRATE_ERROR |
    DETECTOR_CALIBRATE_ERROR |
    DETECTOR_ERROR |
    DETECTOR_BUSY
)


PRESENCE_DETECTED        = 0x01
PRESENCE_DETECTED_STICKY = 0x02
RESULT_DETECTOR_ERROR    = (1 << 15)
TEMPERATURE              = 0xFFFF0000


PROFILE1 = 1
PROFILE2 = 2
PROFILE3 = 3
PROFILE4 = 4
PROFILE5 = 5


APPLY_CONFIGURATION = 1
START_DETECTOR      = 2
STOP_DETECTOR       = 3

ENABLE_UART_LOGS    = 32
DISABLE_UART_LOGS   = 33
LOG_CONFIGURATION  = 34

RESET_MODULE        = 1381192737


DISTANCE_DETECTOR = 1
PRESENCE_DETECTOR = 2
REF_APP_BREATHING = 3
EXAMPLE_CARGO     = 4


class A121_Presence_Detector:
    def __init__(self, bus=0, addr=0x52, BUSY_PIN=18):
        self.i2c  = I2C(bus, scl=Pin(16), sda=Pin(15))
        self.addr = addr
        self.busy_pin = Pin(BUSY_PIN, Pin.IN)

    # ===============================
    # Write: 16-bit reg + 32-bit data
    # ===============================
    def write_u32(self, reg, data):
        data &= 0xFFFFFFFF
        buf = bytearray(6)
        buf[0] = (reg >> 8) & 0xFF
        buf[1] = reg & 0xFF
        buf[2] = (data >> 24) & 0xFF
        buf[3] = (data >> 16) & 0xFF
        buf[4] = (data >> 8) & 0xFF
        buf[5] = data & 0xFF
        self.i2c.writeto(self.addr, buf)

    # ===============================
    # Read: 16-bit reg → 32-bit data
    # ===============================
    def read_u32(self, reg):
        buf = bytearray(2)
        buf[0] = (reg >> 8) & 0xFF
        buf[1] = reg & 0xFF
        self.i2c.writeto(self.addr, buf)
        data = self.i2c.readfrom(self.addr, 4)
        return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]

    def set_start_and_end_range(self, start, end):
        self.write_u32(REG_START, start)
        self.write_u32(REG_END, end)

    def set_sweeps_per_frame(self, value):
        self.write_u32(REG_SWEEPS_PER_FRAME, value)

    def set_inter_frame_presence_timeout(self, value):
        self.write_u32(REG_INTER_FRAME_PRESENCE_TIMEOUT, value)

    def set_intra_detection_enabled(self, value):
        self.write_u32(REG_INTRA_DETECTION_ENABLED, value)

    def set_inter_detection_enabled(self, value):
        self.write_u32(REG_INTER_DETECTION_ENABLED, value)

    def set_frame_rate(self, value):
        self.write_u32(REG_FRAME_RATE, value)

    def set_intra_detection_threshold(self, value):
        self.write_u32(REG_INTRA_DETECTION_THRESHOLD, value)

    def set_inter_detection_threshold(self, value):
        self.write_u32(REG_INTER_DETECTION_THRESHOLD, value)

    def set_inter_frame_deviation_time_const(self, value):
        self.write_u32(REG_INTER_FRAME_DEVIATION_TIME_CONST, value)

    def set_inter_frame_fast_cutoff(self, value):
        self.write_u32(REG_INTER_FRAME_FAST_CUTOFF, value)

    def set_inter_frame_slow_cutoff(self, value):
        self.write_u32(REG_INTER_FRAME_SLOW_CUTOFF, value)

    def set_intra_frame_time_const(self, value):
        self.write_u32(REG_INTRA_FRAME_TIME_CONST, value)

    def set_intra_output_time_const(self, value):
        self.write_u32(REG_INTRA_OUTPUT_TIME_CONST, value)

    def set_inter_output_time_const(self, value):
        self.write_u32(REG_INTER_OUTPUT_TIME_CONST, value)

    def set_auto_profile_enabled(self, value):
        self.write_u32(REG_AUTO_PROFILE_ENABLED, value)

    def set_auto_step_length_enabled(self, value):
        self.write_u32(REG_AUTO_STEP_LENGTH_ENABLED, value)

    def set_manual_profile(self, value):
        self.write_u32(REG_MANUAL_PROFILE, value)

    def set_manual_step_length(self, value):
        self.write_u32(REG_MANUAL_STEP_LENGTH, value)

    def set_reset_filters_on_prepare(self, value):
        self.write_u32(REG_RESET_FILTERS_ON_PREPARE, value)

    def set_hwaas(self, value):
        self.write_u32(REG_HWAAS, value)

    def set_automatic_subsweeps(self, value):
        self.write_u32(REG_AUTOMATIC_SUBSWEEPS, value)

    def set_signal_quality(self, value):
        self.write_u32(REG_SIGNAL_QUALITY, value)

    def set_command(self, value):
        self.write_u32(REG_COMMAND, value)

    def get_version(self):
        v = self.read_u32(REG_VERSION)
        print(f"Major: {v >> 16:#x} Minor: {(v >> 8) & 0xff:#x} Patch: {v & 0xff:#x}")
        return v

    def get_protocol_status(self):
        return self.read_u32(REG_PROTOCOL_STATUS)

    def get_measure_counter(self):
        return self.read_u32(REG_MEASURE_COUNTER)

    def get_detector_status(self):
        return self.read_u32(REG_DETECTOR_STATUS)

    def get_presence_result(self):
        return self.read_u32(REG_PRESENCE_RESULT)

    def get_presence_distance(self):
        return self.read_u32(REG_PRESENCE_DISTANCE)

    def get_intra_presence_score(self):
        return self.read_u32(REG_INTRA_PRESENCE_SCORE)

    def get_inter_presence_score(self):
        return self.read_u32(REG_INTER_PRESENCE_SCORE)

    def get_presence_actual_frame_rate(self):
        return self.read_u32(REG_PRESENCE_ACTUAL_FRAME_RATE)

    def get_application_id(self):
        return self.read_u32(REG_APPLICATION_ID)

    def init(self):
        # wait BUSY
        while self.busy_pin.value() == 0:
            time.sleep(0.001)

        app_id = self.get_application_id()
        print(f"A121_Get_Application_Id: 0x{app_id:x}")

        self.set_command(RESET_MODULE)
        time.sleep(0.1)

        self.set_command(ENABLE_UART_LOGS)

        while self.busy_pin.value() == 0:
            time.sleep(0.001)

        self.set_start_and_end_range(300, 2500)
        self.set_frame_rate(12 * 1000)
        self.set_sweeps_per_frame(16)

        if AUTOMATIC_SUBSWEEPS:
            self.set_automatic_subsweeps(AUTOMATIC_SUBSWEEPS)
            self.set_signal_quality(20 * 1000)
        else:
            self.set_automatic_subsweeps(0)
            self.set_hwaas(32)
            if OVERRIDE:
                self.set_manual_profile(PROFILE1)
                self.set_manual_step_length(24)

        if INTRA_MOTION_DETECTION:
            self.set_intra_detection_threshold(1300)
            self.set_intra_frame_time_const(150)
            self.set_intra_output_time_const(300)

        if INTER_MOTION_DETECTION:
            self.set_inter_detection_threshold(1000)
            self.set_inter_frame_fast_cutoff(6000)
            self.set_inter_frame_slow_cutoff(200)
            self.set_inter_frame_deviation_time_const(500)
            self.set_inter_output_time_const(2000)
            self.set_inter_frame_presence_timeout(3)

        self.set_command(APPLY_CONFIGURATION)

        while True:
            status = self.get_detector_status()
            print(f"A121_Get_Detector_Status: 0x{status:x}")
            if (status & ALL_ERROR) == 0:
                break
            time.sleep(0.1)

        print("A121 init OK")

    def get_presence_detector(self):
        self.set_command(START_DETECTOR)

        while True:
            status = self.get_detector_status()
            if (status & ALL_ERROR) == 0:
                break
            time.sleep(0.01)

        result = self.get_presence_result()

        if (result & RESULT_DETECTOR_ERROR) == 0:
            if result & PRESENCE_DETECTED:
                dist = self.get_presence_distance()
                print(f"Presence_Distance: {dist} mm")

                intra = self.get_intra_presence_score()
                print(f"Intra_Presence_Score: {intra}")

                inter = self.get_inter_presence_score()
                print(f"Inter_Presence_Score: {inter}")

                fps = self.get_presence_actual_frame_rate()
                print(f"Actual_Frame_Rate: {fps}\n")

            elif result & PRESENCE_DETECTED_STICKY:
                print("PRESENCE_DETECTED_STICKY")
            else:
                print("NO PRESENCE DETECTED")
        else:
            print("A121 DETECTOR ERROR (need RESET_MODULE)")


if __name__ == "__main__":
    a121 = A121_Presence_Detector()
    a121.init()

    try:
        while True:
            a121.get_presence_detector()
            time.sleep(0.02)

    except KeyboardInterrupt:
        print("Quit.")

