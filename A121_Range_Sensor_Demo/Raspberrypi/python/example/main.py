# coding: UTF-8
import sys
import time

sys.path.append("..")
from lib import A121_Distance_Detector
from lib import A121_Presence_Detector
from lib import A121_Cargo_Example
from lib import A121_Breathing_Reference

if len(sys.argv) < 2:
    print("Usage: python main.py [distance|presence|cargo|breathing]")
    sys.exit(1)

MODE = sys.argv[1]

if MODE == "distance":
    a121 = A121_Distance_Detector.A121_Distance_Detector(1)
    loop_func = a121.get_distance_mm

elif MODE == "presence":
    a121 = A121_Presence_Detector.A121_Presence_Detector(1)
    loop_func = a121.get_presence_detector

elif MODE == "cargo":
    a121 = A121_Cargo_Example.A121_Cargo_Example(1)
    loop_func = a121.get_cargo_application

elif MODE == "breathing":
    a121 = A121_Breathing_Reference.A121_Breathing_Example(1)
    loop_func = a121.get_breathing_application

else:
    raise ValueError(f"Unknown MODE: {MODE}")

a121.init()

try:
    while True:
        loop_func()
        time.sleep(0.02)

except KeyboardInterrupt:
    print("Quit.")
