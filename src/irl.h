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
// value_type(&) operator ()(State &s, Action &a)
// 某些修改policy的函数要求返回值必须是一个引用

// 关于Map M
// 所有的M指的是一个映射类，例如用于存放value function
// value_type& operator [](State &s)
// 一般可以定义为 map<State, value_type>
// 但是map比较慢，可以自己定义以优化速度

// 实现细节
// 1. vreward(s)和qreward(s, a)必须放在go(s, a)之前调用

#include <map>
#include <vector>
#include <stack>
#include <iostream>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <cstring>
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
    for (auto ps = states.begin(); ps != states.end(); ps++)
        value_func[*ps] = value_func2[*ps] = env.vreward(*ps);

    for (int t = 0; t < trials; t++) {
        int i0 = (trials + t) & 1;
        // std::cout << "check" << std::endl;
        for (auto ps = states.begin(); ps != states.end(); ps++) {
            if (env.terminate(*ps))
                continue;
            double ans = env.vreward(*ps);
            auto actions = env.actions(*ps);
            assert(actions.size() > 0);
            for (auto pa = actions.begin(); pa != actions.end(); pa++) {
                double pr = policy(*ps, *pa);
                double j = 0;
                auto allStates = env.goAll(*ps, *pa);
                for (auto it2 = allStates.begin(); it2 != allStates.end(); it2++) {
                    j += (it2->second) * (i0 ? value_func2[it2->first] : value_func[it2->first]);
                }
                j = j * gamma + env.qreward(*ps, *pa);
                ans += pr * j;
            }
            if (i0)
                value_func[*ps] = ans;
            else
                value_func2[*ps] = ans;
        }
    }
}

// dp - In-place dynamic programming algorithm
template <class E, class P, class M>
void dp(E& env, P& policy, M& value_func, double gamma = 0.9, int trials = 1000) {
    typedef typename E::State State;
    typedef typename E::Action Action;
    auto states = env.states();
    for (auto ps = states.begin(); ps != states.end(); ps++)
        value_func[*ps] = env.vreward(*ps);

    for (int t = 0; t < trials; t++) {
        // std::cout << "check" << std::endl;
        for (auto ps = states.begin(); ps != states.end(); ps++) {
            if (env.terminate(*ps))
                continue;
            double ans = env.vreward(*ps);
            auto actions = env.actions(*ps);
            assert(actions.size() > 0);
            for (auto pa = actions.begin(); pa != actions.end(); pa++) {
                double pr = policy(*ps, *pa);
                double j = 0;
                auto allStates = env.goAll(*ps, *pa);
                for (auto it2 = allStates.begin(); it2 != allStates.end(); it2++) {
                    j += (it2->second) * value_func[it2->first];
                }
                j = j * gamma + env.qreward(*ps, *pa);
                ans += pr * j;
            }
            value_func[*ps] = ans;
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

    for (auto ps = states.begin(); ps != states.end(); ps++) {
        if (env.terminate(*ps))
            continue;
        auto actions = env.actions(*ps);
        assert(actions.size() > 0);
        double m = -oo;
        Action& ma = actions[0];
        for (auto pa = actions.begin(); pa != actions.end(); pa++) {
            auto allStates = env.goAll(*ps, *pa);
            double j = 0;
            for (auto it2 = allStates.begin(); it2 != allStates.end(); it2++) {
                j += (it2->second) * value_func[it2->first];
            }
            j = j * gamma + env.qreward(*ps, *pa);
            if (j > m) {
                m = j;
                ma = *pa;
            }
        }
        for (auto pa = actions.begin(); pa != actions.end(); pa++) {
            if (*pa == ma)
                policy(*ps, *pa) = 1.0;
            else
                policy(*ps, *pa) = 0.0;
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
    for (auto ps = states.begin(); ps != states.end(); ps++)
        value_func[*ps] = value_func2[*ps] = env.vreward(*ps);

    for (int i = 0; i < iterations; i++) {
        int i0 = (iterations + i) & 1;
        for (auto ps = states.begin(); ps != states.end(); ps++) {
            if (env.terminate(*ps))
                continue;
            auto actions = env.actions(*ps);
            assert(actions.size() > 0);
            double m = -oo;
            for (auto pa = actions.begin(); pa != actions.end(); pa++) {
                auto allStates = env.goAll(*ps, *pa);
                double j = 0;
                for (auto it2 = allStates.begin(); it2 != allStates.end(); it2++) {
                    j += (it2->second) * (i0 ? value_func2[it2->first] : value_func[it2->first]);
                }
                j = j * gamma + env.qreward(*ps, *pa);
                m = std::max(m, j);
            }
            m += env.vreward(*ps);
            if (i0)
                value_func[*ps] = m;
            else
                value_func2[*ps] = m;
        }
    }
}

// value_iteration - In-place value iteration
template <class E, class M>
void value_iteration(E& env, M& value_func, double gamma = 0.9, int iterations = 1000) {
    typedef typename E::State State;
    typedef typename E::Action Action;
    auto states = env.states();
    for (auto ps = states.begin(); ps != states.end(); ps++)
        value_func[*ps] = env.vreward(*ps);

    for (int i = 0; i < iterations; i++) {
        for (auto ps = states.begin(); ps != states.end(); ps++) {
            if (env.terminate(*ps))
                continue;
            auto actions = env.actions(*ps);
            assert(actions.size() > 0);
            double m = -oo;
            for (auto pa = actions.begin(); pa != actions.end(); pa++) {
                auto allStates = env.goAll(*ps, *pa);
                double j = 0;
                for (auto it2 = allStates.begin(); it2 != allStates.end(); it2++) {
                    j += (it2->second) * value_func[it2->first];
                }
                j = j * gamma + env.qreward(*ps, *pa);
                m = std::max(m, j);
            }
            m += env.vreward(*ps);
            value_func[*ps] = m;
        }
    }
}

// 2. Monte Carlo and Temporal Difference
// Algorithms in this part are based on value functions

// mc - every-visit Monte Carlo policy evaluation
// requires an auxillary counter array initialized to all zero
// if first == nullptr, env.first() method will be used
// does not guarantee that every state will receive a value
template <class E, class P, class M1, class M2>
void mc(E& env, P& policy, M1& value_func, M2& counter,
    typename E::State *first = nullptr, double gamma = 0.9, int episodes = 200) {
    typedef typename E::State State;
    typedef typename E::Action Action;

    for (int i = 0; i < episodes; i++) {
        // std::cout << i << std::endl;
        std::stack<std::pair<std::pair<State, Action>, std::pair<double, double> > > history;
        State s = first ? *first : env.first();
        while (!env.terminate(s)) {
            double vr = env.vreward(s);
            auto actions = env.actions(s);
            Action& ma = actions[0];
            int flag = 0;
            double r = double(rand()) / double(RAND_MAX);
            double j = 0;
            const double eps = 1e-6;
            for (auto pa = actions.begin(); pa != actions.end(); pa++) {
                j += policy(s, *pa);
                if (j + eps > r) {
                    flag = 1;
                    ma = *pa;
                    break;
                }
            }
            assert(flag);
            double qr = env.qreward(s, ma);
            history.push(std::make_pair(std::make_pair(s, ma), std::make_pair(vr, qr)));
            s = env.go(s, ma);
        }
        double j = env.vreward(s);
        value_func[s] = j;
        while (!history.empty()) {
            auto t = history.top();
            State& s0 = t.first.first;
            Action& a = t.first.second;
            j = j * gamma + t.second.first + t.second.second;
            if (counter[s0] == 0) {
                counter[s0] = 1;
                value_func[s0] = j;
            }
            else {
                value_func[s0] = double(counter[s0]) / (counter[s0] + 1) * value_func[s0] + 
                    double(j) / (counter[s0] + 1);
                counter[s0] += 1;
            }
            history.pop();
        }
    }
}

// td - backward view td(lambda)
// value_func should be initialized to all 0
// requires an auxillary trace array (double type) initialized to all 0
// requires a second auxillary array (boolean type) initialized to all 0
// does not guarantee that every state will receive a value
template <class E, class P, class M1, class M2, class M3>
void td(E& env, P& policy, M1& value_func, M2& trace, 
    M3& aux, typename E::State *first = nullptr, int iterations = 2000,
    double gamma = 0.9, double alpha = 0.1, double lambda = 0.9) {
    typedef typename E::State State;
    typedef typename E::Action Action;
    State s, s0;
    std::vector<State> vec;
    
    bool first_run = true;

    for (int i = 0; i < iterations; i++) {
        if (first_run || env.terminate(s0)) {
            if (first_run)
                first_run = false;
            else
                value_func[s0] = env.vreward(s0);
            for (auto ps = vec.begin(); ps != vec.end(); ps++) {
                trace[*ps] = 0.0;
                aux[*ps] = 0;
            }
            vec.resize(0);
            while (true) {
                if (i >= iterations)
                    return;
                s = first ? *first : env.first();
                if (!env.terminate(s))
                    break;
                i++;
                value_func[s] = env.vreward(s);
            }
        }
        else
            s = s0;
        auto actions = env.actions(s);
        Action& ma = actions[0];
        int flag = 0;
        double r = double(rand()) / double(RAND_MAX);
        double j = 0;
        const double eps = 1e-6;
        for (auto pa = actions.begin(); pa != actions.end(); pa++) {
            j += policy(s, *pa);
            if (j + eps > r) {
                flag = 1;
                ma = *pa;
                break;
            }
        }
        assert(flag);
        double delta = env.qreward(s, ma);
        s0 = env.go(s, ma);
        if (aux[s] == 0) {
            aux[s] = 1;
            vec.push_back(s);
        }
        delta += env.vreward(s) + gamma * value_func[s0] - value_func[s];
        for (auto ps = vec.begin(); ps != vec.end(); ps++)
            trace[*ps] = gamma * lambda * trace[*ps] + (*ps == s) ? 1 : 0;
        for (auto ps = vec.begin(); ps != vec.end(); ps++)
            value_func[*ps] += alpha * delta * trace[*ps];
    }
}

// 3. Sarsa and Q-Learning
// Algorithms in this part are based on Q-tables

// sarsa


// qlearning - Q-Learning algorithm
// off-policy control
template <class E, class P>
void qlearning(E& env, P& qtable, typename E::State *first = nullptr, int iterations = 50000,
    double gamma = 0.9, double alpha = 0.1, double epsilon = 0.9) {
    typedef typename E::State State;
    typedef typename E::Action Action;

    State s, s0;
    int j, j0;
    bool first_run = true, flag = false;

    for (int i = 0; i < iterations; i++) {
        // std::cout << i << std::endl;
        if (first_run || flag) {
            first_run = false;
            flag = false;
            do {
                s = first ? *first : env.first();
            } while (env.terminate(s));
        }
        else
            s = s0;
        auto actions = env.actions(s);
        int m = actions.size();

        // use epsilon-greedy to select j
        double r = double(rand()) / double(RAND_MAX);
        if (r < epsilon) 
            j = int(r / epsilon * m);
        else {
            double mq = -oo;
            j = 0;
            for (int k = 0; k < m; k++) {
                double cq = qtable(s, actions[k]);
                if (cq > mq) {
                    mq = cq;
                    j = k;
                }
            }
        }
        double qr = env.qreward(s, actions[j]);
        s0 = env.go(s, actions[j]);
        double vr = env.vreward(s0);
        if (env.terminate(s0)) {
            flag = true;
            qtable(s, actions[j]) += alpha * (qr + vr - qtable(s, actions[j]));
            continue;
        }

        auto actions0 = env.actions(s0);
        int m0 = actions0.size();

        // use greedy to select j0
        double mq = -oo;
        j0 = 0;
        for (int k = 0; k < m0; k++) {
            double cq = qtable(s0, actions0[k]);
            if (cq > mq) {
                mq = cq;
                j0 = k;
            }
        }

        qtable(s, actions[j]) += alpha * (qr + vr + 
            gamma * qtable(s0, actions0[j0]) - qtable(s, actions[j]));
        // std::cout << qtable(s, actions[j]) << std::endl;
    }
}