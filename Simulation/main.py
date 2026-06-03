import simpy
import pygame
from simulation_engine import ParkingSimulation
from gui import ParkingGUI


def main():
    # 1. Setup SimPy Environment
    env = simpy.Environment()
    sim = ParkingSimulation(env)

    # 2. Setup Pygame GUI
    gui = ParkingGUI(sim)

    # 3. Main execution loop
    clock = pygame.time.Clock()
    running = True

    # We step the simulation manually in the Pygame loop
    # to keep the logic and rendering perfectly synchronized.
    sim_step_size = 1.0  # How many simulated minutes pass per frame

    while running:
        running = gui.handle_events()

        # Advance the logic engine
        env.run(until=env.now + sim_step_size)

        # Render the current state
        gui.render()

        # Cap at 30 frames per second (adjust to speed up/slow down the visual speed)
        clock.tick(30)

    pygame.quit()


if __name__ == "__main__":
    main()