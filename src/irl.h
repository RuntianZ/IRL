#pragma once
// 关于环境E
// 以下template中所有的E指的是一个环境类
// 环境类包含以下方法(不一定全部包含，特定的算法要求包含特定的方法)
// typedef State                         状态的类型
// typedef Action                        动作的类型
// vector<Action> actions(State s)       在某个状态下能做的所有动作
// State go(State &s, Action &a)         在一个状态做一个动作到达的下一个状态(含随机性)
// vector<pair<State, double>> goAll(State &s, Action &a)
//                                       包含做动作后可能到达的每一个状态以及概率
// State first()                         获得初始状态
// vector<State> states()                获得所有状态
// double vreward(State s)               进入某一状态的瞬间可以获得的reward
// double qreward(State s, Action a)     在某一状态做某一动作能得到的即时reward
// bool terminate(State s)               返回某一个状态是否是终止状态

// 关于Policy P
// 所有的P指的是一个Policy类，也可以用于存放q(s,a)
// Policy类包含以下内容
// double(&) operator ()(State &s, Action &a) 在s做动作a的概率
// 某些修改policy的函数要求返回值必须是一个引用

// 关于Map M
// 所有的M指的是一个映射类，例如用于存放value function
// double& operator [](State &s)
// 一般可以定义为 map<State, double>
// 但是map比较慢，可以自己定义以优化速度

#include <map>
#include <vector>
#include <stack>
#include <iostream>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <algorithm>
const double oo = 1e10;

// 0. Helper functions

// irl_init - Warm up irl
void irl_init() {
    srand(time(0));
}

// move - Make one move from a state (with randomness)
// suitable for known models
// Will modify s
template <class E>
void move(E& env, typename E::State &s, typename E::Action &a) {
    typedef typename E::State State;
    typedef typename E::Action Action;
    double r = double(rand()) / (double)(RAND_MAX);
    const double eps = 1e-6;
    auto allStates = env.goAll(s, a);
    double j = 0;
    auto ite = allStates.begin();
    for (; ; ite++) {
        assert(ite != allStates.end());
        j += ite->second;
        if (j + eps > r)
            break;
    }
    s = ite->first;
}

// 1. Dynamic Programming Method

// dp - dynamic programming policy evaluation
// Will modify both value_func and value_func2
// Final result will be stored in value_func
// value_func2 is an auxillary storage
template <class E, class P, class M1, class M2>
void dp(E& env, P& policy, M1& value_func, M2& value_func2, 
    double gamma = 0.9, int trials = 1000) {
    typedef typename E::State State;
    typedef typename E::Action Action;
    auto states = env.states();
    for each (State s in states)
        value_func[s] = value_func2[s] = env.vreward(s);

    for (int t = 0; t < trials; t++) {
        int i0 = (trials + t) & 1;
        // std::cout << "check" << std::endl;
        for each (State s in states) {
            if (env.terminate(s))
                continue;
            double ans = env.vreward(s);
            auto actions = env.actions(s);
            assert(actions.size() > 0);
            for each (Action a in actions) {
                double pr = policy(s, a);
                double j = 0;
                auto allStates = env.goAll(s, a);
                for (auto it2 = allStates.begin(); it2 != allStates.end(); it2++) {
                    j += (it2->second) * (i0 ? value_func2[it2->first] : value_func[it2->first]);
                }
                j = j * gamma + env.qreward(s, a);
                ans += pr * j;
            }
            if (i0)
                value_func[s] = ans;
            else
                value_func2[s] = ans;
        }
    }
}

// dp - In-place dynamic programming algorithm
template <class E, class P, class M>
void dp(E& env, P& policy, M& value_func, double gamma = 0.9, int trials = 1000) {
    typedef typename E::State State;
    typedef typename E::Action Action;
    auto states = env.states();
    for each (State s in states)
        value_func[s] = env.vreward(s);

    for (int t = 0; t < trials; t++) {
        // std::cout << "check" << std::endl;
        for each (State s in states) {
            if (env.terminate(s))
                continue;
            double ans = env.vreward(s);
            auto actions = env.actions(s);
            assert(actions.size() > 0);
            for each (Action a in actions) {
                double pr = policy(s, a);
                double j = 0;
                auto allStates = env.goAll(s, a);
                for (auto it2 = allStates.begin(); it2 != allStates.end(); it2++) {
                    j += (it2->second) * value_func[it2->first];
                }
                j = j * gamma + env.qreward(s, a);
                ans += pr * j;
            }
            value_func[s] = ans;
        }
    }
}

// vgreedy - Fill a policy greedily using value function
// Will modify policy
template <class E, class P, class M>
void vgreedy(E& env, P& policy, M& value_func, double gamma = 0.9) {
    typedef typename E::State State;
    typedef typename E::Action Action;
    auto states = env.states();

    for each (State s in states) {
        if (env.terminate(s))
            continue;
        auto actions = env.actions(s);
        assert(actions.size() > 0);
        double m = -oo;
        Action ma = actions[0];
        for each (Action a in actions) {
            auto allStates = env.goAll(s, a);
            double j = 0;
            for (auto it2 = allStates.begin(); it2 != allStates.end(); it2++) {
                j += (it2->second) * value_func[it2->first];
            }
            j = j * gamma + env.qreward(s, a);
            if (j > m) {
                m = j;
                ma = a;
            }
        }
        for each (Action a in actions) {
            if (a == ma)
                policy(s, a) = 1.0;
            else
                policy(s, a) = 0.0;
        }
    }
}

// policy_iteration
// Will modify policy and value_func
template <class E, class P, class M1, class M2>
void policy_iteration(E& env, P& policy, M1& value_func, M2& value_func2, double gamma = 0.9, 
    int trials = 200, int iterations = 30) {
    for (int i = 0; i < iterations; i++) {
        dp(env, policy, value_func, value_func2, gamma, trials);
        vgreedy(env, policy, value_func, gamma);
    }
}

// policy_iteration - In-place policy iteration
template <class E, class P, class M>
void policy_iteration(E& env, P& policy, M& value_func, double gamma = 0.9,
    int trials = 200, int iterations = 30) {
    for (int i = 0; i < iterations; i++) {
        dp(env, policy, value_func, gamma, trials);
        vgreedy(env, policy, value_func, gamma);
    }
}

// value_iteration
template <class E, class M1, class M2>
void value_iteration(E& env, M1& value_func, M2& value_func2, double gamma = 0.9, int iterations = 1000) {
    typedef typename E::State State;
    typedef typename E::Action Action;
    auto states = env.states();
    for each (State s in states)
        value_func[s] = value_func2[s] = env.vreward(s);

    for (int i = 0; i < iterations; i++) {
        int i0 = (iterations + i) & 1;
        for each (State s in states) {
            if (env.terminate(s))
                continue;
            auto actions = env.actions(s);
            assert(actions.size() > 0);
            double m = -oo;
            for each (Action a in actions) {
                auto allStates = env.goAll(s, a);
                double j = 0;
                for (auto it2 = allStates.begin(); it2 != allStates.end(); it2++) {
                    j += (it2->second) * (i0 ? value_func2[it2->first] : value_func[it2->first]);
                }
                j = j * gamma + env.qreward(s, a);
                m = std::max(m, j);
            }
            m += env.vreward(s);
            if (i0)
                value_func[s] = m;
            else
                value_func2[s] = m;
        }
    }
}

// value_iteration - In-place value iteration
template <class E, class M>
void value_iteration(E& env, M& value_func, double gamma = 0.9, int iterations = 1000) {
    typedef typename E::State State;
    typedef typename E::Action Action;
    auto states = env.states();
    for each (State s in states)
        value_func[s] = env.vreward(s);

    for (int i = 0; i < iterations; i++) {
        for each (State s in states) {
            if (env.terminate(s))
                continue;
            auto actions = env.actions(s);
            assert(actions.size() > 0);
            double m = -oo;
            for each (Action a in actions) {
                auto allStates = env.goAll(s, a);
                double j = 0;
                for (auto it2 = allStates.begin(); it2 != allStates.end(); it2++) {
                    j += (it2->second) * value_func[it2->first];
                }
                j = j * gamma + env.qreward(s, a);
                m = std::max(m, j);
            }
            m += env.vreward(s);
            value_func[s] = m;
        }
    }
}

// 2. Monte Carlo and Temporal Difference

// mc - every-visit Monte Carlo policy evaluation
// requires an auxillary counter array initialized to all zero
// if first == nullptr, env.first() method will be used
// does not guarantee that every state will receive a value
template <class E, class P, class M, class Counter>
void mc(E& env, P& policy, M& value_func, Counter& counter,
    typename E::State *first = nullptr, double gamma = 0.9, int episodes = 200) {
    typedef typename E::State State;
    typedef typename E::Action Action;

    for (int i = 0; i < episodes; i++) {
        std::cout << i << std::endl;
        std::stack<std::pair<State, Action> > history;
        State s = first ? *first : env.first();
        while (!env.terminate(s)) {
            auto actions = env.actions(s);
            Action ma = actions[0];
            int flag = 0;
            double r = double(rand()) / double(RAND_MAX);
            double j = 0;
            const double eps = 1e-6;
            for each (Action a in actions) {
                j += policy(s, a);
                if (j + eps > r) {
                    flag = 1;
                    ma = a;
                    break;
                }
            }
            assert(flag);
            history.push(std::make_pair(s, ma));
            s = env.go(s, ma);
        }
        double j = env.vreward(s);
        value_func[s] = j;
        while (!history.empty()) {
            auto t = history.top();
            State s = t.first;
            Action a = t.second;
            history.pop();
            j = j * gamma + env.vreward(s) + env.qreward(s, a);
            if (counter[s] == 0) {
                counter[s] = 1;
                value_func[s] = j;
            }
            else {
                value_func[s] = double(counter[s]) / (counter[s] + 1) * value_func[s] + double(j) / (counter[s] + 1);
                counter[s] += 1;
            }
        }
    }
}