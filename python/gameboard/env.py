# Game board simulator
# can only run on Windows
from ctypes import *
import os
import numpy as np
import math
import threading
import time

# Load library
dllfile = 'IRL.dll'
if not os.path.exists(dllfile):
    raise FileNotFoundError("Required dll not found.")
lib = cdll.LoadLibrary(dllfile)
lib.start_game.restype = c_char_p
lib.start_game.argtypes = [c_int]
lib.move.restype = c_char_p
lib.move.argtypes = [c_float]
lib.display.restype = c_int32
lib.display.argtypes = []
lib.shoot.restype = c_int32
lib.shoot.argtypes = [c_float]
lib.lock.restype = c_int32
lib.lock.argtypes = []
lib.get_state.restype = c_char_p
lib.get_state.argtypes = []

# Global data
current_state = ""
current_mode = 0
current_status = 1
current_score = 0
current_num_of_balls = 0
current_blocks = None
current_max_height = 0
# [type, life, centerx, centery, angle] type: Triangle(0), Circle(1), Square(2)


def parse():
    global current_state, current_max_height
    parse_res = current_state.split()
    current_max_height = 0
    global current_status, current_score, current_num_of_balls, current_blocks
    current_status = int(parse_res[0])
    current_score = int(parse_res[1])
    current_num_of_balls = int(parse_res[2])
    current_blocks = []
    n = len(parse_res)
    for i in range(4, n - 1, 5):
        new_block = [int(parse_res[i]), int(parse_res[i+1]),
                     float(parse_res[i + 2]), float(parse_res[i + 3]), float(parse_res[i + 4])]
        # print(new_block[4])
        current_blocks.append(new_block)
        h = int((new_block[3] - 24.0) * 0.076923076923)
        current_max_height = max(h, current_max_height)


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
    current_state = lib.move(angle)
    parse()


max_x = 80
max_y = 128
radius = 6
atan_value = [[-45.0, -50.19442890773481, -56.309932474020215, -63.43494882292201, 
               -71.56505117707799, -80.53767779197439, 90.0, 80.53767779197439, 71.56505117707799, 
               63.43494882292201, 56.309932474020215, 50.19442890773481, 45.0], 
              [-39.8055710922652, -45.0, -51.34019174590991, -59.03624346792648, 
               -68.19859051364818, -78.69006752597979, 90.0, 78.69006752597979, 
               68.19859051364818, 59.03624346792648, 51.34019174590991, 45.0, 39.8055710922652], 
              [-33.690067525979785, -38.659808254090095, -45.0, -53.13010235415598, 
               -63.43494882292201, -75.96375653207353, 90.0, 75.96375653207353, 63.43494882292201, 
               53.13010235415598, 45.0, 38.659808254090095, 33.690067525979785], 
              [-26.56505117707799, -30.96375653207352, -36.86989764584402, -45.0, 
               -56.309932474020215, -71.56505117707799, 90.0, 71.56505117707799, 56.309932474020215, 
               45.0, 36.86989764584402, 30.96375653207352, 26.56505117707799], 
              [-18.43494882292201, -21.80140948635181, -26.56505117707799, -33.690067525979785, 
               -45.0, -63.43494882292201, 90.0, 63.43494882292201, 45.0, 33.690067525979785, 
               26.56505117707799, 21.80140948635181, 18.43494882292201], 
              [-9.462322208025617, -11.309932474020213, -14.036243467926477, -18.43494882292201, 
               -26.56505117707799, -45.0, 90.0, 45.0, 26.56505117707799, 18.43494882292201, 
               14.036243467926477, 11.309932474020213, 9.462322208025617], 
              [180.0, 180.0, 180.0, 180.0, 180.0, 180.0, -90.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0], 
              [9.462322208025617, 11.309932474020213, 14.036243467926477, 18.43494882292201, 
               26.56505117707799, 45.0, -90.0, -45.0, -26.56505117707799, -18.43494882292201, 
               -14.036243467926477, -11.309932474020213, -9.462322208025617], 
              [18.43494882292201, 21.80140948635181, 26.56505117707799, 33.690067525979785, 
               45.0, 63.43494882292201, -90.0, -63.43494882292201, -45.0, -33.690067525979785, 
               -26.56505117707799, -21.80140948635181, -18.43494882292201], 
              [26.56505117707799, 30.96375653207352, 36.86989764584402, 45.0, 56.309932474020215, 
               71.56505117707799, -90.0, -71.56505117707799, -56.309932474020215, -45.0, 
               -36.86989764584402, -30.96375653207352, -26.56505117707799], 
              [33.690067525979785, 38.659808254090095, 45.0, 53.13010235415598, 63.43494882292201, 
               75.96375653207353, -90.0, -75.96375653207353, -63.43494882292201, -53.13010235415598, 
               -45.0, -38.659808254090095, -33.690067525979785], 
              [39.8055710922652, 45.0, 51.34019174590991, 59.03624346792648, 
               68.19859051364818, 78.69006752597979, -90.0, -78.69006752597979, 
               -68.19859051364818, -59.03624346792648, -51.34019174590991, -45.0, -39.8055710922652], 
              [45.0, 50.19442890773481, 56.309932474020215, 63.43494882292201, 71.56505117707799, 
               80.53767779197439, -90.0, -80.53767779197439, -71.56505117707799, 
               -63.43494882292201, -56.309932474020215, -50.19442890773481, -45.0]]
tmp_num_of_balls = 0
tmp_blocks = None


def intensity(x):
    global tmp_num_of_balls
    y = x / tmp_num_of_balls
    y = 148.4040604 * (math.e - math.exp(1.0 / (0.1 * y + 1.0)))
    return y


def image(blocks=None, num_of_balls=0):
    img = np.zeros([max_x, max_y], np.int32)
    global current_blocks, current_num_of_balls, tmp_blocks, tmp_num_of_balls
    tmp_blocks = blocks or current_blocks
    tmp_num_of_balls = num_of_balls or current_num_of_balls

    for block in current_blocks:
        cx = block[2]
        cy = block[3]
        cl = block[1]
        ca = block[4]
        ca = ca - int(ca / 360.0) * 360.0
        for dx in range(-radius, radius + 1):
            for dy in range(-radius, radius + 1):
                x = int(cx + dx)
                y = int(cy + dy)
                if x < 5 or x >= 85 or y < 16 or y >= 144:
                    continue
                a = atan_value[dx + radius][dy + radius]
                if block[0] == 0:
                    a = a - ca
                    while a < -60.0:
                        a = a + 120.0
                    while a > 60.0:
                        a = a - 120.0
                    a = abs(a)
                    maxd = 5.0 * math.cos(a * 0.017453292519943295)
                elif block[0] == 2:
                    a = a - ca
                    while a < 0.0:
                        a = a + 90.0
                    while a > 90.0:
                        a = a - 90.0
                    a = abs(a - 45.0)
                    maxd = 5.0 * math.cos(a * 0.017453292519943295)
                else:
                    maxd = 4.0
                d = math.sqrt(dx * dx + dy * dy)
                if d <= maxd:
                    img[x-5, y-16] = math.ceil(intensity(cl))
    return img


def vector(blocks=None, num_of_balls=0):
    global current_blocks, current_num_of_balls, tmp_blocks, tmp_num_of_balls
    tmp_blocks = blocks or current_blocks
    tmp_num_of_balls = num_of_balls or current_num_of_balls

    ans = np.zeros(81, np.float32)
    c = [0, 9, 18, 27, 36, 45, 54, 63, 72]
    for block in current_blocks:
        h = int((block[3] - 24.0) * 0.076923076923)
        if h <= 8:
            i = c[h]
            ans[i] = float(block[2])
            ans[i + 1] = float(block[4])
            ans[i + 2] = intensity(block[1])
            c[h] = c[h] + 3
    # print(ans)
    return ans


def display():
    t = threading.Thread(target=lib.display)
    t.start()
    time.sleep(2)


def shoot(angle):
    lib.shoot(angle)
    while lib.lock() == 1:
        time.sleep(1)
    global current_state
    current_state = lib.get_state()
    parse()
