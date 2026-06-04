import pygame
import math


class Slider:
    def __init__(self, x, y, width, height, min_val, max_val, start_val, label_format):
        self.rect = pygame.Rect(x, y, width, height)
        self.min_val = min_val
        self.max_val = max_val
        self.val = start_val
        self.label_format = label_format
        self.dragging = False
        self.knob_r = height // 2 + 6
        self.update_knob_x()

    def update_knob_x(self):
        ratio = (self.val - self.min_val) / (self.max_val - self.min_val)
        self.knob_x = self.rect.x + int(ratio * self.rect.width)
        self.knob_y = self.rect.centery

    def update_val_from_pos(self, x):
        x = max(self.rect.left, min(x, self.rect.right))
        ratio = (x - self.rect.left) / self.rect.width
        self.val = self.min_val + ratio * (self.max_val - self.min_val)
        self.update_knob_x()

    def handle_event(self, event):
        if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            # Check if mouse clicked the knob or the bar
            if math.hypot(event.pos[0] - self.knob_x, event.pos[1] - self.knob_y) <= self.knob_r:
                self.dragging = True
            elif self.rect.collidepoint(event.pos):
                self.dragging = True
                self.update_val_from_pos(event.pos[0])

        elif event.type == pygame.MOUSEBUTTONUP and event.button == 1:
            self.dragging = False

        elif event.type == pygame.MOUSEMOTION and self.dragging:
            self.update_val_from_pos(event.pos[0])

    def draw(self, screen, font):
        # Draw bar
        pygame.draw.rect(screen, (100, 100, 100), self.rect)
        # Draw active part of bar
        active_rect = pygame.Rect(self.rect.x, self.rect.y, self.knob_x - self.rect.x, self.rect.height)
        pygame.draw.rect(screen, (255, 165, 0), active_rect)
        # Draw Knob
        pygame.draw.circle(screen, (255, 255, 255), (self.knob_x, self.knob_y), self.knob_r)

        # Draw Text
        text = font.render(self.label_format.format(self.val), True, (255, 255, 255))
        screen.blit(text, (self.rect.x, self.rect.y - 25))

class ParkingGUI:
    def __init__(self, simulation):
        self.sim = simulation
        pygame.init()
        self.width, self.height = 1000, 700
        self.screen = pygame.display.set_mode((self.width, self.height))
        pygame.display.set_caption("IoT Parking Simulator")
        self.font = pygame.font.SysFont("Arial", 18, bold=True)
        self.small_font = pygame.font.SysFont("Arial", 12)

        # Instantiate the slider: min 0.1 mins (heavy traffic), max 10.0 mins (light traffic)
        self.slider_arrival = Slider(100, 640, 300, 8, 0.1, 10.0, self.sim.arrival_rate, "Arrival Rate: 1 car every {:.1f} mins")
        self.slider_stay = Slider(600, 640, 300, 8, 5.0, 120.0, self.sim.avg_stay, "Avg Stay Duration: {:.1f} mins")

    def render(self):
        self.screen.fill((40, 40, 40))  # Dark asphalt background

        # Draw roads
        # Vertical spine road
        pygame.draw.rect(self.screen, (60, 60, 60), (450, 0, 100, 700))
        # Horizontal Road 1
        pygame.draw.rect(self.screen, (60, 60, 60), (0, 100, 1000, 80))
        # Horizontal Road 2
        pygame.draw.rect(self.screen, (60, 60, 60), (0, 300, 1000, 80))
        # Horizontal Road 3
        pygame.draw.rect(self.screen, (60, 60, 60), (0, 500, 1000, 80))

        def draw_line_of_spots(start_idx, x_start, y_start):
            """Helper to draw a horizontal line of 15 spots"""
            for i in range(15):
                spot = self.sim.spots[start_idx + i]
                color = (200, 50, 50) if spot.occupied else (50, 200, 50)
                x = x_start + i * 28  # 25px width + 3px gap
                pygame.draw.rect(self.screen, color, (x, y_start, 25, 35))
                pygame.draw.rect(self.screen, (255, 255, 255), (x, y_start, 25, 35), 1)

        # Render Spots based on logical rows
        # ROAD 1 (Top)
        draw_line_of_spots(0, 20, 50)  # Row A: Above Rd 1 Left
        draw_line_of_spots(15, 20, 195)  # Row A: Below Rd 1 Left
        draw_line_of_spots(30, 560, 50)  # Row B: Above Rd 1 Right
        draw_line_of_spots(45, 560, 195)  # Row B: Below Rd 1 Right

        # ROAD 2 (Middle)
        draw_line_of_spots(60, 20, 245)  # Row C: Above Rd 2 Left (Back-to-back with Row A)
        draw_line_of_spots(75, 20, 395)  # Row C: Below Rd 2 Left
        draw_line_of_spots(90, 560, 245)  # Row D: Above Rd 2 Right
        draw_line_of_spots(105, 560, 395)  # Row D: Below Rd 2 Right

        # ROAD 3 (Bottom)
        draw_line_of_spots(120, 20, 445)  # Row E: Above Rd 3 Left (Back-to-back with Row C)
        draw_line_of_spots(135, 560, 445)  # Row F: Above Rd 3 Right

        # Render LED Screens
        counts = self.sim.get_row_counts()
        self.draw_led_screen(280, 120, f"ROW A: {counts['Row A']} FREE")
        self.draw_led_screen(580, 120, f"ROW B: {counts['Row B']} FREE")

        self.draw_led_screen(280, 320, f"ROW C: {counts['Row C']} FREE")
        self.draw_led_screen(580, 320, f"ROW D: {counts['Row D']} FREE")

        self.draw_led_screen(280, 520, f"ROW E: {counts['Row E']} FREE")
        self.draw_led_screen(580, 520, f"ROW F: {counts['Row F']} FREE")

        self.slider_arrival.draw(self.screen, self.font)
        self.slider_stay.draw(self.screen, self.font)

        pygame.display.flip()

    def draw_led_screen(self, x, y, text):
        pygame.draw.rect(self.screen, (0, 0, 0), (x, y, 160, 40))
        pygame.draw.rect(self.screen, (255, 165, 0), (x, y, 160, 40), 2)
        text_surface = self.font.render(text, True, (255, 165, 0))
        self.screen.blit(text_surface, (x + 10, y + 10))

    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return False

            self.slider_arrival.handle_event(event)
            self.slider_stay.handle_event(event)

        self.sim.arrival_rate = self.slider_arrival.val
        self.sim.avg_stay = self.slider_stay.val
        return True