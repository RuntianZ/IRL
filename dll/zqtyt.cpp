#include "../src/gl_env.h"
#ifdef DLL_MODE
#include "zqtytdll.h"
#include "../src/zqtyt.h"

GameBoard *gb = nullptr;
char state_string[20000];
int status = 1;
const float delta_t = 0.005;

void parse_board() {
    char *p = state_string;
    p += sprintf(p, "%d %d %d %d ", status, gb->score, gb->num_of_balls, gb->blocks.size());
    for (auto ite = gb->blocks.begin(); ite != gb->blocks.end(); ite++)
        p += sprintf(p, "%d %d %f %f %f ", ite->type, ite->life, ite->centerx, ite->centery, ite->angle);
}

void parse_state(int mode, char *state) {
    gb = new GameBoard(mode, false);
    std::istringstream sstr(state);
    int n;
    sstr >> status >> gb->score >> gb->num_of_balls >> n;
    gb->blocks.reserve(n);
    for (int i = 0; i < n; i++) {
        GameBoard::Block bk;
        int t;
        sstr >> t >> bk.life >> bk.centerx >> bk.centery >> bk.angle;
        bk.type = (GameBoard::Block::Type)t;
        gb->blocks.push_back(bk);
    }
}

char * start_game(int mode) {
    gb = new GameBoard(mode);
    parse_board();
    return state_string;
}

char * move(int mode, char *state, float angle) {
    parse_state(mode, state);
    gb->shooter_angle = angle;
    gb->shoot();
    while (true) {
        int res = gb->step(delta_t);
        if (res == 1)
            break;
        else if (res == 2) {
            status = 2;
            break;
        }
    }
    parse_board();
    return state_string;
}
#endif