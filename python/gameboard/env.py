# Game board simulator
# can only run on Windows
from ctypes import *
import os
import numpy as np
import math

# Load library
dllfile = 'IRL.dll'
if not os.path.exists(dllfile):
    raise FileNotFoundError("Required dll not found.")
lib = cdll.LoadLibrary(dllfile)
lib.start_game.restype = c_char_p
lib.start_game.argtypes = [c_int]
lib.move.restype = c_char_p
lib.move.argtypes = [c_int, c_char_p, c_float]

# Global data
current_state = ""
current_mode = 0
current_status = 1
current_score = 0
current_num_of_balls = 0
current_blocks = []
# [type, life, centerx, centery, angle] type: Triangle(0), Circle(1), Square(2)


def parse():
    global current_state
    parse_res = current_state.split()
    global current_status, current_score, current_num_of_balls, current_blocks
    current_status = int(parse_res[0])
    current_score = int(parse_res[1])
    current_num_of_balls = int(parse_res[2])
    current_blocks = []
    n = len(parse_res)
    for i in range(4, n - 1, 5):
        current_blocks.append([int(parse_res[i]), int(parse_res[i+1]),
                               float(parse_res[i + 2]), float(parse_res[i + 3]), float(parse_res[i + 4])])


def start_game(game_mode=0):
    global current_state, current_mode
    current_state = lib.start_game(game_mode)
    current_mode = game_mode
    parse()


def move(angle):
    global current_state, current_mode
    if current_state == '':
        raise EnvironmentError("Game is not started.")
    if current_status == 2:
        raise EnvironmentError("Game is over.")
    current_state = lib.move(current_mode, current_state, angle)
    parse()


max_x = 90
max_y = 160
radius = 20


def intensity(x):
    y = x / current_num_of_balls
    y = 148.4040604 * (math.e - math.exp(1 / (0.1 * y + 1)))
    return math.ceil(y)


def image():
    img = np.zeros([max_x, max_y], np.int8)
    global current_blocks

    for block in current_blocks:
        cx = block[2]
        cy = block[3]
        cl = block[1]
        ca = block[4]
        ca = ca - int(ca / 360) * 360
        for dx in range(-radius, radius):
            for dy in range(-radius, radius):
                x = int(cx + dx)
                y = int(cy + dy)
                if dy == 0:
                    if dx < 0:
                        a = 90
                    else:
                        a = -90
                elif dx == 0:
                    if dy > 0:
                        a = 0
                    else:
                        a = 180
                else:
                    a = math.atan(-dx/dy) * 180 / math.pi
                if block[0] == 0:
                    a = a - ca
                    while a < -60:
                        a = a + 120
                    while a > 60:
                        a = a - 120
                    a = abs(a)
                    maxd = 5 * math.cos(a * math.pi / 180)
                elif block[0] == 2:
                    a = a - ca
                    while a < 0:
                        a = a + 90
                    while a > 90:
                        a = a - 90
                    a = abs(a - 45)
                    maxd = 5 * math.cos(a * math.pi / 180)
                else:
                    maxd = 4
                d = math.sqrt(dx * dx + dy * dy)
                if d <= maxd:
                    img[x, y] = intensity(cl)
    return img
