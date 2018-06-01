#pragma once
#include "irl.h"
#include "zqtyt.h"
#include <cstdio>

// easiest mode的一个简单环境模型
// 注意，该模型的使用必须严格按照以下使用顺序
// 在一开始使用和每次gameover后再要使用之前，须调用一次first()
// 要执行一次发射动作，先调用qreward(s, a)获得回报，再调用go(s, a)获得下一个状态
// qreward和go两个方法中，传入的参数s对结果没有影响
// 不可以对于同一个s多次调用qreward或go
class EasyModel {
private:
    static const float delta_angle;
    static const float delta_t;
    static const int zero_code, max_size, angle_num;
    static const float death_penalty;

public:
    GameBoard *gb;
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

    template <class T>
    struct Policy {
        std::vector<T> storage;

        Policy() :
            storage(max_size * angle_num) {}

        T & operator ()(State& s, Action& a) {
            return storage[s.hashcode() * actioncode(a)];
        }
        void clear() {
            for (int i = 0; i < max_size * angle_num; i++)
                storage[i] = (T)0;
        }
        void save(const char* path) {
            FILE *f;
            f = fopen(path, "w");
            fprintf(stderr, "Saving...\n");
            for (int i = 0; i < max_size * angle_num; i++)
                fprintf(f, "%lf ", storage[i]);
            fclose(f);
            fprintf(stderr, "Saved.\n");
        }
        void load(const char* path) {
            FILE *f;
            f = fopen(path, "r");
            fprintf(stderr, "Loading...\n");
            for (int i = 0; i < max_size * angle_num; i++)
                fscanf(f, "%lf", &storage[i]);
            fclose(f);
            fprintf(stderr, "Loaded.\n");
        }
    };

    Policy<double> *pre_qtable;

    double qreward(State& s, Action& a) {
        double reward = 0;
        if (pre_qtable != nullptr)
            reward = (*pre_qtable)(state_now, a);
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
        return reward + gb->score - score_last;
    }

    double vreward(State& s) {
        return (s.game_over) ? death_penalty : 0.0;
    }

    inline static int actioncode(Action& a) {
        return int((a + GameBoard::max_shooter_angle + 0.001) / delta_angle);
    }

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

    EasyModel(Policy<double>* pre_q = nullptr) :
        gb(nullptr), pre_qtable(pre_q) {}

    ~EasyModel() {
        if (gb)
            delete gb;
    }

}; // class EasyModel

const float EasyModel::delta_angle = 5.0;
const float EasyModel::delta_t = 0.005;
const int EasyModel::angle_num = 
    int((GameBoard::max_shooter_angle * 2.0 + 0.001) / EasyModel::delta_angle);
const int EasyModel::zero_code = 98304;
const int EasyModel::max_size = 294912;
const float EasyModel::death_penalty = -10.0;

namespace EasyModelAI {
    EasyModel::Policy<double> qtable, pre_q;
    EasyModel em(&pre_q);
    float my_action;
    bool is_learning;
    void(*k_func)();

    void init(const char* path = nullptr, const char* path_pre = nullptr,
        void(*func)() = nullptr) {
        if (path == nullptr)
            qtable.clear();
        else
            qtable.load(path);
        if (path_pre == nullptr)
            pre_q.clear();
        else
            pre_q.load(path_pre);
        k_func = func;
    }

    void save(const char* path, const char* path_pre = nullptr) {
        qtable.save(path);
        if (path_pre != nullptr)
            pre_q.save(path_pre);
    }

    void ai(float& angle) {
        em.get_state();
        auto actions = em.actions(em.state_now);
        double mq = -oo;
        for (auto pa = actions.begin(); pa != actions.end(); pa++) {
            double cq = qtable(em.state_now, *pa);
            if (cq > mq) {
                mq = cq;
                my_action = *pa;
            }
        }
        if (!is_learning)
            angle = my_action;
    }

    void learning(float& angle) {
        float your_action = angle;
        auto actions = em.actions(em.state_now);
        float your_score = qtable(em.state_now, your_action);
        std::cout << qtable(em.state_now, my_action) - your_score << std::endl;
        for (auto pa = actions.begin(); pa != actions.end(); pa++) {
            float delta_q = qtable(em.state_now, *pa) - your_score;
            delta_q = 2.0 * (delta_q + 1.0);
            qtable(em.state_now, *pa) -= delta_q;
            pre_q(em.state_now, your_action) -= delta_q;
        }
    }

    void showWindow(bool learn = false) {
        is_learning = learn;
        if (learn)
            Viewer::showWindow(*em.gb, ai, learning, true, k_func);
        else
            Viewer::showWindow(*em.gb, ai);
    }
} // namespace EasyModelAI