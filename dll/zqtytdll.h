#pragma once
extern "C" __declspec(dllexport) char * start_game(int mode);
extern "C" __declspec(dllexport) char * move(float angle);
extern "C" __declspec(dllexport) int display();
extern "C" __declspec(dllexport) int shoot(float angle);
extern "C" __declspec(dllexport) int lock();
extern "C" __declspec(dllexport) char * get_state();
