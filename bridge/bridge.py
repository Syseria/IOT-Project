import os
import json
from urllib.parse import urlparse
import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS

# --- CONFIGURATION (Smart Load from Env) ---
MQTT_URL_STR = os.environ.get("MQTT_BROKER_URL", "tcp://mosquitto:1883")
parsed_url = urlparse(MQTT_URL_STR)

MQTT_BROKER = parsed_url.hostname
MQTT_PORT = parsed_url.port or 1883
USE_TLS = parsed_url.scheme in ['ssl', 'mqtts'] or MQTT_PORT == 8883

MQTT_USER = os.environ.get("MQTT_USER", "")
MQTT_PASS = os.environ.get("MQTT_PASS", "")
MQTT_TOPIC = os.environ.get("MQTT_TOPIC", "v3/+/devices/+/up")

INFLUX_URL = os.environ.get("INFLUX_URL", "http://influxdb:8086")
INFLUX_TOKEN = os.environ.get("INFLUX_TOKEN", "")
INFLUX_ORG = os.environ.get("INFLUX_ORG", "")
INFLUX_BUCKET = os.environ.get("INFLUX_BUCKET", "")

# --- MEMORY STATE ---
global_state = [0] * 19

# --- INFLUX SETUP ---
influx_client = InfluxDBClient(url=INFLUX_URL, token=INFLUX_TOKEN, org=INFLUX_ORG)
write_api = influx_client.write_api(write_options=SYNCHRONOUS)


# --- MQTT CALLBACKS ---
def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected to MQTT Broker at {MQTT_BROKER}:{MQTT_PORT}!")
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    global global_state

    try:
        payload = json.loads(msg.payload)

        decoded = payload.get("uplink_message", {}).get("decoded_payload", {})
        changes = decoded.get("byte_changes", [])

        if not changes:
            return

        print(f"Received update for {len(changes)} bytes.")

        # 1. Update memory
        for change in changes:
            idx = change["index"]
            val = change["value"]
            if idx < 19:
                global_state[idx] = val

        # 2. Unpack to 150 InfluxDB points
        points_to_write = []
        for i in range(150):
            byte_index = i // 8
            bit_index = i % 8
            is_occupied = (global_state[byte_index] >> bit_index) & 1

            p = Point("parking_spot").tag("sensor_id", str(i)).field("occupied", int(is_occupied))
            points_to_write.append(p)

        # 3. Write to Influx
        write_api.write(bucket=INFLUX_BUCKET, org=INFLUX_ORG, record=points_to_write)
        print("Successfully pushed 150 states to InfluxDB.")

    except Exception as e:
        print(f"Error processing message: {e}")


# --- SETUP MQTT CLIENT (v2 API) ---
mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

if MQTT_USER and MQTT_PASS:
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASS)

if USE_TLS:
    mqtt_client.tls_set()

mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

print(f"Connecting to {MQTT_BROKER}...")
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
mqtt_client.loop_forever()