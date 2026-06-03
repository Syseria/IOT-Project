import random
from battery_model import BatteryModel
from network_mock import NetworkMock


class ParkingSpot:
    def __init__(self, spot_id, row_name):
        self.spot_id = spot_id
        self.row_name = row_name
        self.occupied = False
        self.battery = BatteryModel()


class ParkingSimulation:
    def __init__(self, env):
        self.env = env
        self.network = NetworkMock()

        # Average 1 car every 2 simulated minutes
        self.arrival_rate = 10.0
        self.avg_stay = 45.0

        # Logical breakdown: 150 spots divided into 3 rows to match your layout idea
        self.spots = [ParkingSpot(i, "Row A") for i in range(30)] + \
                     [ParkingSpot(i, "Row B") for i in range(30, 60)] + \
                     [ParkingSpot(i, "Row C") for i in range(60, 90)] + \
                     [ParkingSpot(i, "Row D") for i in range(90, 120)] + \
                     [ParkingSpot(i, "Row E") for i in range(120, 135)] + \
                     [ParkingSpot(i, "Row F") for i in range(135, 150)]

        self.env.process(self.car_generator())

    def car_generator(self):
        """Continuously spawns cars looking for parking."""
        while True:
            current_rate = max(0.1, self.arrival_rate)

            yield self.env.timeout(random.expovariate(1.0 / current_rate))

            free_spots = [s for s in self.spots if not s.occupied]
            if free_spots:
                spot = random.choice(free_spots)
                self.env.process(self.car_lifecycle(spot))

    def car_lifecycle(self, spot):
        """Handles a single car parking and eventually leaving."""
        # Park
        spot.occupied = True
        spot.battery.trigger_event()
        self.network.publish_bitmap(self.spots)

        stay_time = max(0.1, random.normalvariate(self.avg_stay, 10))
        yield self.env.timeout(stay_time)

        # Leave
        spot.occupied = False
        spot.battery.trigger_event()
        self.network.publish_bitmap(self.spots)

    def get_row_counts(self):
        """Calculates free spots per row for the local LED screens."""
        counts = {
            "Row A": 0, "Row B": 0,
            "Row C": 0, "Row D": 0,
            "Row E": 0, "Row F": 0
        }
        for spot in self.spots:
            if not spot.occupied:
                counts[spot.row_name] += 1
        return counts