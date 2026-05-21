import time
from machine import I2C, Pin

# ===============================
# Configuration
# ===============================
USE_ACTIVATE_PRESENCE = True

# ===============================
# Register Map
# ===============================
REG_VERSION                     = 0x0000
REG_PROTOCOL_STATUS             = 0x0001
REG_MEASURE_COUNTER             = 0x0002
REG_ACTUAL_PRESENCE_UPDATE_RATE = 0x0003
REG_APPLICATION_STATUS          = 0x0004

REG_CONTAINER_SIZE              = 0x0010
REG_ACTIVATE_UTILIZATION_LEVEL  = 0x0011
REG_UTILIZATION_SIGNAL_QUALITY  = 0x0012
REG_UTILIZATION_THRESHOLD_SENS  = 0x0013
REG_ACTIVATE_PRESENCE           = 0x0014
REG_PRESENCE_UPDATE_RATE        = 0x0015
REG_PRESENCE_SWEEPS_PER_FRAME   = 0x0016
REG_PRESENCE_SIGNAL_QUALITY     = 0x0017
REG_PRESENCE_INTER_THRESHOLD    = 0x0018
REG_PRESENCE_INTRA_THRESHOLD    = 0x0019

REG_RESULT_HEADER               = 0x0020
REG_UTILIZATION_DISTANCE        = 0x0021
REG_UTILIZATION_LEVEL_MM        = 0x0022
REG_UTILIZATION_LEVEL_PERCENT   = 0x0023
REG_PRESENCE_DETECTED           = 0x0024
REG_MAX_INTER_PRESENCE_SCORE    = 0x0025
REG_MAX_INTRA_PRESENCE_SCORE    = 0x0026

REG_COMMAND                     = 0x0030
REG_APPLICATION_ID              = 0xFFFF


PROTOCOL_STATE_ERROR = 0x00000001  # Pos 0
PACKET_LENGTH_ERROR  = 0x00000002  # Pos 1
ADDRESS_ERROR        = 0x00000004  # Pos 2
WRITE_FAILED         = 0x00000008  # Pos 3
WRITE_TO_READ_ONLY   = 0x00000010  # Pos 4

# ===============================
# Sensor status (bitmask)
# ===============================

RSS_REGISTER_OK        = 1 << 0
SENSOR_CREATE_OK       = 1 << 1
SENSOR_CALIBRATE_OK    = 1 << 2
CARGO_CREATE_OK        = 1 << 3
CARGO_CALIBRATE_OK     = 1 << 4
SENSOR_BUFFER_OK       = 1 << 5
CARGO_BUFFER_OK        = 1 << 6
CONFIG_APPLY_OK        = 1 << 7

RSS_REGISTER_ERROR     = 1 << 8
SENSOR_CREATE_ERROR    = 1 << 10
SENSOR_CALIBRATE_ERROR = 1 << 11
CARGO_CREATE_ERROR     = 1 << 12
CARGO_CALIBRATE_ERROR  = 1 << 13
SENSOR_BUFFER_ERROR    = 1 << 14
CARGO_BUFFER_ERROR     = 1 << 15
CONFIG_APPLY_ERROR     = 1 << 16
APPLICATION_ERROR      = 1 << 17

APP_BUSY               = 1 << 18

# All errors combined
ALL_ERROR = (
    RSS_REGISTER_ERROR |
    SENSOR_CREATE_ERROR |
    SENSOR_CALIBRATE_ERROR |
    CARGO_CREATE_ERROR |
    CARGO_CALIBRATE_ERROR |
    SENSOR_BUFFER_ERROR |
    CARGO_BUFFER_ERROR |
    CONFIG_APPLY_ERROR |
    APPLICATION_ERROR |
    APP_BUSY
)

# ===============================
# Sensor result header (bitmask)
# ===============================
UTILIZATION_LEVEL_VALID = 1 << 17
PRESENCE_VALID          = 1 << 18
TEMPERATURE             = 0x0000FFFF

# ===============================
# Sensor app state
# ===============================
INIT                   = 0x00
NO_PRESENCE            = 1
INTRA_PRESENCE         = 2
DETERMINE_DISTANCE     = 3
ESTIMATE_BREATHING_RATE= 4

# ===============================
# Sensor profile
# ===============================
PROFILE1 = 1
PROFILE2 = 2
PROFILE3 = 3
PROFILE4 = 4
PROFILE5 = 5

# ===============================
# Sensor command
# ===============================
APPLY_CONFIGURATION       = 1
MEASURE_UTILIZATION_LEVEL = 4
MEASURE_PRESENCE          = 5
ENABLE_UART_LOGS          = 32
DISABLE_UART_LOGS         = 33
LOG_CONFIGURATION         = 34
RESET_MODULE              = 1381192737

# ===============================
# Sensor mode
# ===============================
DISTANCE_DETECTOR   = 1
PRESENCE_DETECTOR   = 2
REF_APP_BREATHING   = 3
EXAMPLE_CARGO       = 4

class A121_Cargo_Example:
    def __init__(self, bus=1, addr=0x52, BUSY_PIN=8):
        self.i2c  = I2C(bus, scl=Pin(7), sda=Pin(6))
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

    # Container size: 10 / 20 / 40
    def set_container_size(self, value):
        self.write_u32(REG_CONTAINER_SIZE, value)

    def set_activate_utilization_level(self, value):
        self.write_u32(REG_ACTIVATE_UTILIZATION_LEVEL, value)

    def set_utilization_signal_quality(self, value):
        self.write_u32(REG_UTILIZATION_SIGNAL_QUALITY, value)

    def set_utilization_threshold_sens(self, value):
        self.write_u32(REG_UTILIZATION_THRESHOLD_SENS, value)

    def set_activate_presence(self, value):
        self.write_u32(REG_ACTIVATE_PRESENCE, value)

    def set_presence_update_rate(self, value):
        self.write_u32(REG_PRESENCE_UPDATE_RATE, value)

    def set_presence_sweeps_per_frame(self, value):
        self.write_u32(REG_PRESENCE_SWEEPS_PER_FRAME, value)

    def set_presence_signal_quality(self, value):
        self.write_u32(REG_PRESENCE_SIGNAL_QUALITY, value)

    def set_presence_inter_detection_threshold(self, value):
        self.write_u32(REG_PRESENCE_INTER_THRESHOLD, value)

    def set_presence_intra_detection_threshold(self, value):
        self.write_u32(REG_PRESENCE_INTRA_THRESHOLD, value)

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

    def get_actual_presence_update_rate(self):
        return self.read_u32(REG_ACTUAL_PRESENCE_UPDATE_RATE)

    def get_application_status(self):
        return self.read_u32(REG_APPLICATION_STATUS)

    def get_result_header(self):
        return self.read_u32(REG_RESULT_HEADER)

    def get_utilization_distance(self):
        return self.read_u32(REG_UTILIZATION_DISTANCE)

    def get_utilization_level_mm(self):
        return self.read_u32(REG_UTILIZATION_LEVEL_MM)

    def get_utilization_level_percent(self):
        return self.read_u32(REG_UTILIZATION_LEVEL_PERCENT)

    def get_presence_detected(self):
        return self.read_u32(REG_PRESENCE_DETECTED)

    def get_max_inter_presence_score(self):
        return self.read_u32(REG_MAX_INTER_PRESENCE_SCORE)

    def get_max_intra_presence_score(self):
        return self.read_u32(REG_MAX_INTRA_PRESENCE_SCORE)

    def get_application_id(self):
        return self.read_u32(REG_APPLICATION_ID)

    def init(self):
        while self.busy_pin.value() == 0:
            time.sleep(0.001)

        app_id = self.get_application_id()
        print(f"A121_Get_Application_Id: 0x{app_id:x}")

        self.set_command(RESET_MODULE)
        time.sleep(0.1)

        self.set_command(ENABLE_UART_LOGS)

        while self.busy_pin.value() == 0:
            time.sleep(0.001)

        self.set_container_size(20)
        self.set_activate_utilization_level(True)

        self.set_presence_update_rate(5000)          # 5 Hz
        self.set_utilization_signal_quality(25000)   # 25
        self.set_utilization_threshold_sens(500)     # 0.5

        if USE_ACTIVATE_PRESENCE:
            self.set_activate_presence(True)
            self.set_presence_update_rate(6000)      # 6 Hz
            self.set_presence_signal_quality(30000)  # 30
            self.set_presence_sweeps_per_frame(12)
            self.set_presence_inter_detection_threshold(2000)
            self.set_presence_intra_detection_threshold(2000)

        self.set_command(APPLY_CONFIGURATION)

        while True:
            status = self.get_application_status()
            print(f"A121_Get_Application_Status: 0x{status:x}")
            if (status & ALL_ERROR) == 0:
                break
            time.sleep(0.1)

        print("A121 init OK")

    def get_cargo_application(self):
        self.set_command(MEASURE_UTILIZATION_LEVEL)

        while True:
            status = self.get_application_status()
            if (status & ALL_ERROR) == 0:
                break
            time.sleep(0.01)

        header = self.get_result_header()

        if header & UTILIZATION_LEVEL_VALID:
            dist = self.get_utilization_distance()
            print(f"A121_Get_Utilization_Distance: {dist}")

            level_mm = self.get_utilization_level_mm()
            print(f"A121_Get_Utilization_Level_Mm: {level_mm} mm")

            level_pct = self.get_utilization_level_percent()
            print(f"A121_Get_Utilization_Level_Percent: {level_pct} %\n")


if __name__ == "__main__":
    a121 = A121_Cargo_Example()
    a121.init()

    try:
        while True:
            a121.get_cargo_application()
            time.sleep(0.02)

    except KeyboardInterrupt:
        print("Quit.")
