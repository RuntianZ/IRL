# Game board simulator
# can only run on Windows
from ctypes import *
import os
import platform

# Load library
dllfile = 'zqtytdll.dll'
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


def image():
    pass
