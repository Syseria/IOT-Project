import paho.mqtt.client as mqtt


class NetworkMock:
    def __init__(self, broker_address="127.0.0.1", port=1883, topic="v1/parking/devices/master/up"):
        self.topic = topic
        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
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
        payload_bytes = int(bit_string, 2).to_bytes(19, byteorder='big')

        if self.connected:
            self.client.publish(self.topic, payload_bytes)

        return payload_bytes.hex()