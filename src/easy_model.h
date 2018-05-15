#pragma once
#include "irl.h"
#include "zqtyt.h"

// easiest mode的一个简单环境模型
// 注意，该模型的使用必须严格按照以下使用顺序
// 在一开始使用和每次gameover后再要使用之前，须调用一次first()
// 要执行一次发射动作，先调用qreward(s, a)获得回报，再调用go(s, a)获得下一个状态
// qreward和go两个方法中，传入的参数s对结果没有影响
// 不可以对于同一个s多次调用qreward或go
class EasyModel {
private:
    GameBoard *gb;
    static const float delta_angle;
    static const float delta_t;
    static const int zero_code, max_size, angle_num;

public:
    EasyModel():
        gb(nullptr) {}

    ~EasyModel() {
        if (gb)
            delete gb;
    }
    struct State {
        int h;
        bool has_block[5][3];
        bool game_over;
        int hashcode() {
            if (game_over)
                return zero_code;
            int ans = h;
            for (int j = 0; j < 3; j++)
                for (int i = 0; i < 5; i++) {
                    ans *= 2;
                    if (has_block[i][j]) ans++;
                }
            return ans;
        }
    } state_now;
    typedef float Action;

    std::vector<Action> actions(State &s) {
        float angles[50];
        int n = 0;
        float f = -GameBoard::max_shooter_angle;
        while (f < GameBoard::max_shooter_angle + 0.001) {
            angles[n++] = f;
            f += delta_angle;
        }
        std::vector<Action> ans(n);
        for (int i = 0; i < n; i++)
            ans[i] = angles[i];
        return ans;
    }

    void get_state() {
        assert(gb->balls.size() == 0);
        int h = 0;
        state_now.game_over = false;
        memset(state_now.has_block, 0, sizeof(state_now.has_block));
        for (auto bk = gb->blocks.begin(); bk != gb->blocks.end(); bk++) {
            int hc = int((bk->centery - GameBoard::birth_line + 0.01) / GameBoard::delta_height);
            h = std::max(h, hc);
        }
        state_now.h = h;
        for (auto bk = gb->blocks.begin(); bk != gb->blocks.end(); bk++) {
            int y = int((bk->centery - GameBoard::birth_line + 0.01) / GameBoard::delta_height);
            y = h - y;
            if (y <= 2) {
                int x = 0;
                while (GameBoard::easiest_new_x[x] + 0.01 - bk->centerx < 0)
                    x++;
                state_now.has_block[x][y] = true;
            }
        }
    }

    State first() {
        if (gb)
            delete gb;
        gb = new GameBoard(1);
        get_state();
        return state_now;
    }

    State go(State &s, Action &a) {
        return state_now;
    }

    bool terminate(State& s) {
        return s.game_over;
    }

    double qreward(State& s, Action& a) {
        int score_last = gb->score;
        gb->shooter_angle = a;
        gb->shoot();
        while (true) {
            int res = gb->step(delta_t);
            if (res == 1) {
                get_state();
                break;
            }
            else if (res == 2) {
                state_now.game_over = true;
                break;
            }
        }
        return gb->score - score_last;
    }

    double vreward(State& s) {
        return 0.0;
    }

    inline static int actioncode(Action& a) {
        return int((a + GameBoard::max_shooter_angle + 0.001) / delta_angle);
    }

    template <class T>
    struct Policy {
        std::vector<T> storage;

        Policy() :
            storage(max_size * angle_num) {}

        T & operator ()(State& s, Action& a) {
            return storage[s.hashcode() * actioncode(a)];
        }
    };

    template <class T>
    struct Map {
        std::vector<T> storage;

        Map() :
            storage(max_size) {}

        T & operator [](State& s) {
            storage[s.hashcode()];
        }
        void clear() {
            for (int i = 0; i < max_size; i++)
                storage[i] = (T)0;
        }
    };

};

const float EasyModel::delta_angle = 5.0;
const float EasyModel::delta_t = 0.005;
const int EasyModel::angle_num = 
    int((GameBoard::max_shooter_angle * 2.0 + 0.001) / EasyModel::delta_angle);
const int EasyModel::zero_code = 98304;
const int EasyModel::max_size = 294912;