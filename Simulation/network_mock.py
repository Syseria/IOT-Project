import paho.mqtt.client as mqtt
import os
import json
from dotenv import load_dotenv

load_dotenv()

MQTT_TOPIC = os.environ.get("MQTT_TOPIC", "v3/+/devices/+/up")

class NetworkMock:
    def __init__(self, broker_address="127.0.0.1", port=1883, topic=MQTT_TOPIC):
        self.topic = topic
        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

        # Memory buffer to simulate Arduino's delta compression
        self.last_buffer = bytearray(19)
        self.first_send = True

        try:
            self.client.connect(broker_address, port, 60)
            self.client.loop_start()
            self.connected = True
            print("Connected to mock MQTT Broker")
        except Exception as e:
            self.connected = False
            print(f"MQTT Broker not found at {broker_address}. Simulating network locally.")

    def publish_bitmap(self, spots):
        """Converts spot statuses into a 19-byte payload and publishes it."""
        # Create a string of 150 bits (1 for occupied, 0 for free)
        bit_string = "".join(["1" if spot.occupied else "0" for spot in spots])

        # Pad with 2 zeros to make it exactly 152 bits (19 bytes)
        bit_string = bit_string.ljust(152, '0')

        # Convert to bytes
        current_buffer = bytearray(int(bit_string, 2).to_bytes(19, byteorder='big'))

        # Build the JSON object exactly how TTN's MQTT integration outputs it
        byte_changes = []
        for i in range(19):
            # If it's the first send, or the byte has changed since last time
            if self.first_send or current_buffer[i] != self.last_buffer[i]:
                byte_changes.append({
                    "index": i,
                    "value": current_buffer[i],
                    "hex_diff": f"0x{current_buffer[i]:02X}"
                })

            # Update memory
            self.last_buffer[i] = current_buffer[i]

        self.first_send = False

        # If nothing changed, don't send anything (matching LoRa behaviour)
        if not byte_changes:
            return None

        # 5. Build the payload exactly as the TTN Javascript Decoder outputs it
        ttn_mock_payload = {
            "uplink_message": {
                "decoded_payload": {
                    "compression_type": "DELTA_COMPRESSED",
                    "is_full_sync": len(byte_changes) == 19,
                    "byte_changes": byte_changes
                }
            }
        }

        if self.connected:
            self.client.publish(self.topic, json.dumps(ttn_mock_payload))
            print(f"[SIM] Published {len(byte_changes)} byte changes.")

        return ttn_mock_payload
