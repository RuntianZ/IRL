#pragma once
#include "irl.h"

class GridWorld {
private:
    int m, n;
    std::vector<double> reward;
public:
    GridWorld() :
        m(1), n(1), reward(1) {}
    GridWorld(int m, int n) :
        m(m), n(n), reward(m * n) {}
    void setReward(int x, int y, double r) {
        reward[x * n + y] = r;
    }
    void setReward(const double *data) {
        for (int i = 0; i < m * n; i++)
            reward[i] = data[i];
    }
    void setReward(const std::vector<double> &data) {
        for (int i = 0; i < m * n; i++)
            reward[i] = data[i];
    }

    enum Action {
        UP, DOWN, LEFT, RIGHT
    };
    typedef std::pair<int, int> State;
    template <class T>
    struct Map {
        std::vector<T> storage;
        int n;
        T & operator [] (State &s) {
            return storage[s.first * n + s.second];
        }
    };
    template <class T>
    void loadMap(Map<T> &mp) {
        mp.storage.resize(m * n);
        mp.n = n;
        for (int i = 0; i < m * n; i++)
            mp.storage[i] = (T)0;
    }

    std::vector<State> states() {
        std::vector<State> ans;
        ans.reserve(m * n);
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                ans.push_back(std::make_pair(i, j));
        return ans;
    }
    State first() {
        // Random start
        double r1 = double(rand()) / double(RAND_MAX);
        double r2 = double(rand()) / double(RAND_MAX);
        int x = int(r1 * m), y = int(r2 * n);
        if (x >= m)
            x = m - 1;
        if (y >= n)
            y = n - 1;
        return std::make_pair(x, y);
    }
    State go(State &s, Action &a) {
        switch (a) {
        case UP:
            return std::make_pair(s.first - 1, s.second);
        case DOWN:
            return std::make_pair(s.first + 1, s.second);
        case LEFT:
            return std::make_pair(s.first, s.second - 1);
        case RIGHT:
            return std::make_pair(s.first, s.second + 1);
        }
        return std::make_pair(0, 0); // Never reach here
    }
    std::vector<std::pair<State, double> > goAll(State &s, Action &a) {
        std::vector<std::pair<State, double> > ans;
        switch (a) {
        case UP:
            ans.push_back(std::make_pair(std::make_pair(s.first - 1, s.second), 1.0));
            break;
        case DOWN:
            ans.push_back(std::make_pair(std::make_pair(s.first + 1, s.second), 1.0));
            break;
        case LEFT:
            ans.push_back(std::make_pair(std::make_pair(s.first, s.second - 1), 1.0));
            break;
        case RIGHT:
            ans.push_back(std::make_pair(std::make_pair(s.first, s.second + 1), 1.0));
        }
        return ans;
    }
    double vreward(State &s) {
        return reward[s.first * n + s.second];
    }
    double qreward(State &s, Action &a) {
        return -1.0;
    }
    bool terminate(State &s) {
        return (vreward(s) > 0);
    }
    std::vector<Action> actions(State &s) {
        int x = s.first, y = s.second;
        std::vector<Action> ans;
        if (terminate(s))
            return ans;
        if (x > 0)
            ans.push_back(UP);
        if (x < m - 1)
            ans.push_back(DOWN);
        if (y > 0)
            ans.push_back(LEFT);
        if (y < n - 1)
            ans.push_back(RIGHT);
        return ans;
    }

    template <class T>
    struct Policy {
        int n;
        std::vector<T> storage;
        T & operator () (State &s, Action &a) {
            return storage[(s.first * n + s.second) * 4 + a];
        }
    };
    template <class T>
    void loadPolicy(Policy<T> &p) {
        // Random policy
        p.storage.resize(m * n * 4);
        p.n = n;
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++) {
                State s = std::make_pair(i, j);
                auto ac = actions(s);
                int t = ac.size();
                if (t > 0) {
                    for each (Action a in ac) {
                        p(s, a) = (T)(1.0 / t);
                    }
                }
            }
    }
};