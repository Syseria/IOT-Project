class BatteryModel:
    def __init__(self, capacity_mah=2700):
        self.capacity_mah = capacity_mah
        self.current_mah = capacity_mah

        # Simulated consumption values (in mAh) per event
        self.consume_sense = 0.0002
        self.consume_tx = 0.0016

    def trigger_event(self):
        """Called when a car arrives or leaves (Sense + Transmit)"""
        self.current_mah -= (self.consume_sense + self.consume_tx)

    def get_percentage(self):
        return max(0.0, (self.current_mah / self.capacity_mah) * 100)