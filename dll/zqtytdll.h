#pragma once
extern "C" __declspec(dllexport) char * start_game(int mode);
extern "C" __declspec(dllexport) char * move(int mode, char *state, float angle);