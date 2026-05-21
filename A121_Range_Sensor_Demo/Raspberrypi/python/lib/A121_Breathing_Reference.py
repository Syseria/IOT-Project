import time
from smbus2 import SMBus, i2c_msg
from gpiozero import Button
from enum import IntFlag, IntEnum

# ===============================
# General constants
# ===============================
USE_PRESENCE_PROCESSOR = True

# ===============================
# Register addresses
# ===============================
# Read Only
REG_VERSION = 0x0000
REG_PROTOCOL_STATUS = 0x0001
REG_MEASURE_COUNTER = 0x0002
REG_APP_STATUS = 0x0003
REG_BREATHING_RESULT = 0x0010
REG_BREATHING_RATE = 0x0011
REG_APP_STATE = 0x0012
REG_APPLICATION_ID = 0xFFFF

# Read / Write
REG_START = 0x0040
REG_END = 0x0041
REG_NUM_DISTANCES_TO_ANALYZE = 0x0042
REG_DISTANCE_DETERMINATION_DURATION_S = 0x0043
REG_USE_PRESENCE_PROCESSOR = 0x0044
REG_LOWEST_BREATHING_RATE = 0x0045
REG_HIGHEST_BREATHING_RATE = 0x0046
REG_TIME_SERIES_LENGTH_S = 0x0047
REG_FRAME_RATE = 0x0048
REG_SWEEPS_PER_FRAME = 0x0049
REG_HWAAS = 0x004A
REG_PROFILE = 0x004B
REG_INTRA_DETECTION_THRESHOLD = 0x004C

# Write Only
REG_COMMAND = 0x0100

# ===============================
# Sensor Protocol Error Flags
# ===============================
class SensorProtocolError(IntEnum):
    PROTOCOL_STATE_ERROR = 0x00000001
    PACKET_LENGTH_ERROR = 0x00000002
    ADDRESS_ERROR = 0x00000004
    WRITE_FAILED = 0x00000008
    WRITE_TO_READ_ONLY = 0x00000010

# ===============================
# Sensor Status Flags
# ===============================
class SensorStatus(IntEnum):
    RSS_REGISTER_OK = 1 << 0
    CONFIG_CREATE_OK = 1 << 1
    SENSOR_CREATE_OK = 1 << 2
    SENSOR_CALIBRATE_OK = 1 << 3
    APP_CREATE_OK = 1 << 4
    APP_BUFFER_OK = 1 << 5
    SENSOR_BUFFER_OK = 1 << 6
    CONFIG_APPLY_OK = 1 << 7

    RSS_REGISTER_ERROR = 1 << 16
    CONFIG_CREATE_ERROR = 1 << 17
    SENSOR_CREATE_ERROR = 1 << 18
    SENSOR_CALIBRATE_ERROR = 1 << 19
    APP_CREATE_ERROR = 1 << 20
    APP_BUFFER_ERROR = 1 << 21
    SENSOR_BUFFER_ERROR = 1 << 22
    CONFIG_APPLY_ERROR = 1 << 23
    APP_ERROR = 1 << 28

    APP_BUSY = 1 << 31

ALL_ERROR = (
    SensorStatus.RSS_REGISTER_ERROR
    | SensorStatus.CONFIG_CREATE_ERROR
    | SensorStatus.SENSOR_CREATE_ERROR
    | SensorStatus.APP_CREATE_ERROR
    | SensorStatus.SENSOR_BUFFER_ERROR
    | SensorStatus.APP_BUFFER_ERROR
    | SensorStatus.CONFIG_APPLY_ERROR
    | SensorStatus.SENSOR_CALIBRATE_ERROR
    | SensorStatus.APP_ERROR
    | SensorStatus.APP_BUSY
)

# ===============================
# Breathing Result Flags
# ===============================
class SensorBreathingResult(IntEnum):
    RESULT_READY = 0x01
    RESULT_READY_STICKY = 0x02
    TEMPERATURE = 0xFFFF0000

# ===============================
# Application States
# ===============================
class SensorAppState(IntEnum):
    INIT = 0x00
    NO_PRESENCE = 1
    INTRA_PRESENCE = 2
    DETERMINE_DISTANCE = 3
    ESTIMATE_BREATHING_RATE = 4

# ===============================
# Sensor Profiles
# ===============================
class SensorProfile(IntEnum):
    PROFILE1 = 1
    PROFILE2 = 2
    PROFILE3 = 3
    PROFILE4 = 4
    PROFILE5 = 5

# ===============================
# Sensor Commands
# ===============================
class SensorCommand(IntEnum):
    APPLY_CONFIGURATION = 1
    START_APP = 2
    STOP_APP = 3
    ENABLE_UART_LOGS = 32
    DISABLE_UART_LOGS = 33
    LOG_CONFIGURATION = 34
    RESET_MODULE = 1381192737

# ===============================
# Sensor Modes
# ===============================
class SensorMode(IntEnum):
    DISTANCE_DETECTOR = 1
    PRESENCE_DETECTOR = 2
    REF_APP_BREATHING = 3
    EXAMPLE_CARGO = 4

class A121_Breathing_Example:
    def __init__(self, bus=1, addr=0x52, busy_pin=4):
        self.bus = SMBus(bus)
        self.addr = addr
        self.busy_pin = Button(busy_pin)

    # ===============================
    # Write: 16-bit reg + 32-bit data
    # ===============================
    def write_u32(self, reg, data):
        data &= 0xFFFFFFFF
        buf = [
            (reg >> 8) & 0xFF,
            reg & 0xFF,
            (data >> 24) & 0xFF,
            (data >> 16) & 0xFF,
            (data >> 8) & 0xFF,
            data & 0xFF
        ]
        self.bus.i2c_rdwr(i2c_msg.write(self.addr, buf))

    # ===============================
    # Read: 16-bit reg → 32-bit data
    # ===============================
    def read_u32(self, reg):
        reg_buf = [(reg >> 8) & 0xFF, reg & 0xFF]
        write_msg = i2c_msg.write(self.addr, reg_buf)
        read_msg = i2c_msg.read(self.addr, 4)
        self.bus.i2c_rdwr(write_msg, read_msg)
        r = list(read_msg)
        return (r[0] << 24) | (r[1] << 16) | (r[2] << 8) | r[3]

    # ===============================
    # Setter functions
    # ===============================
    def set_start_end_range(self, start, end):
        self.write_u32(REG_START, start)
        self.write_u32(REG_END, end)

    def set_num_distances_to_analyze(self, value):
        self.write_u32(REG_NUM_DISTANCES_TO_ANALYZE, value)

    def set_distance_determination_duration_s(self, value):
        self.write_u32(REG_DISTANCE_DETERMINATION_DURATION_S, value)

    def set_use_presence_processor(self, value):
        self.write_u32(REG_USE_PRESENCE_PROCESSOR, int(value))

    def set_lowest_breathing_rate(self, value):
        self.write_u32(REG_LOWEST_BREATHING_RATE, value)

    def set_highest_breathing_rate(self, value):
        self.write_u32(REG_HIGHEST_BREATHING_RATE, value)

    def set_time_series_length_s(self, value):
        self.write_u32(REG_TIME_SERIES_LENGTH_S, value)

    def set_frame_rate(self, value):
        self.write_u32(REG_FRAME_RATE, value)

    def set_sweeps_per_frame(self, value):
        self.write_u32(REG_SWEEPS_PER_FRAME, value)

    def set_hwaas(self, value):
        self.write_u32(REG_HWAAS, value)

    def set_profile(self, value):
        self.write_u32(REG_PROFILE, value)

    def set_intra_detection_threshold(self, value):
        self.write_u32(REG_INTRA_DETECTION_THRESHOLD, value)

    def set_command(self, value):
        self.write_u32(REG_COMMAND, value)

    # ===============================
    # Getter functions
    # ===============================
    def get_version(self):
        value = self.read_u32(REG_VERSION)
        major = (value >> 16) & 0xFF
        minor = (value >> 8) & 0xFF
        patch = value & 0xFF
        print(f"Major: 0x{major:x} Minor: 0x{minor:x} Patch: 0x{patch:x}")
        return value

    def get_protocol_status(self):
        return self.read_u32(REG_PROTOCOL_STATUS)

    def get_measure_counter(self):
        return self.read_u32(REG_MEASURE_COUNTER)

    def get_app_status(self):
        return self.read_u32(REG_APP_STATUS)

    def get_breathing_result(self):
        return self.read_u32(REG_BREATHING_RESULT)

    def get_breathing_rate(self):
        return self.read_u32(REG_BREATHING_RATE)

    def get_app_state(self):
        return self.read_u32(REG_APP_STATE)

    def get_application_id(self):
        return self.read_u32(REG_APPLICATION_ID)

    # ===============================
    # Initialization
    # ===============================
    def init(self):
        while self.busy_pin.is_pressed:
            time.sleep(0.001)
        app_id = self.get_application_id()
        print(f"A121_Get_Application_Id: 0x{app_id:x}")

        self.set_command(SensorCommand.RESET_MODULE)
        time.sleep(0.1)

        self.set_command(SensorCommand.ENABLE_UART_LOGS)

        while self.busy_pin.is_pressed:
            time.sleep(0.001)

        self.set_start_end_range(300, 1000)
        self.set_num_distances_to_analyze(3)
        self.set_distance_determination_duration_s(5)
        if USE_PRESENCE_PROCESSOR:
            self.set_use_presence_processor(True)
            self.set_intra_detection_threshold(4000)
        else:
            self.set_use_presence_processor(False)
        self.set_lowest_breathing_rate(6)
        self.set_highest_breathing_rate(60)
        self.set_time_series_length_s(20)
        self.set_frame_rate(20000)
        self.set_sweeps_per_frame(16)
        self.set_hwaas(32)
        self.set_profile(SensorProfile.PROFILE3)

        self.set_command(SensorCommand.APPLY_CONFIGURATION)
        while True:
            status = self.get_app_status()
            print(f"A121_Get_App_Status: 0x{status:x}")
            if (status & ALL_ERROR) == 0:
                break
            time.sleep(0.1)

        print("A121 init OK")
        self.set_command(SensorCommand.START_APP)
        while True:
            status = self.get_app_status()
            print(f"A121_Get_App_Status: 0x{status:x}")
            if (status & ALL_ERROR) == 0:
                break
            time.sleep(0.01)

    # ===============================
    # Breathing application
    # ===============================
    def get_breathing_application(self):
        value = self.get_breathing_result()
        if (value & SensorStatus.APP_ERROR) == 0:
            if value & SensorBreathingResult.RESULT_READY:
                rate = self.get_breathing_rate()
                print(f"A121_Get_Breathing_Rate: {rate} Hz")
                time.sleep(2)
            elif value & SensorBreathingResult.RESULT_READY_STICKY:
                print("PRESENCE_DETECTED_STICKY")
            else:
                print("checking...")
        else:
            print("A121 DETECTOR ERROR")