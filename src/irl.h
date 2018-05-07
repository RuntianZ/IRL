#pragma once
// 关于环境E
// 以下template中所有的E指的是一个环境类
// 环境类包含以下内容
// typedef State                         状态的类型
// typedef Action                        动作的类型
// vector<Action> actions(State s)       在某个状态下能做的所有动作
// State go(State &s, Action &a)         在一个状态做一个动作到达的下一个状态(含随机性)
// vector<pair<State, double>> goAll(State &s, Action &a)
//                                       包含做动作后可能到达的每一个状态以及概率
// State first()                         获得初始状态(可选)
// vector<State> states()                获得所有状态(可选)
// double vreward(State s)               进入某一状态的瞬间可以获得的reward
// double qreward(State s, Action a)     在某一状态做某一动作能得到的即时reward
// bool terminate(State s)               返回某一个状态是否是终止状态

// 关于Policy P
// 所有的P指的是一个Policy类
// Policy类包含以下内容
// double operator ()(State &s, Action &a) 在s做动作a的概率

// 关于Map M
// 所有的M指的是一个映射类，例如用于存放value function
// M要求重载运算符[]
// 一般可以定义为 map<State, double>
// 但是map比较慢，可以自己定义以优化速度

#include <map>
#include <vector>
#include <iostream>

// dp - Dynamic Programming algorithm
template <class E, class P, class M>
void dp(E env, P policy, M &value_func, double gamma = 0.9, int trials = 1000) {
	typedef typename E::State State;
	typedef typename E::Action Action;
	using std::vector;
	vector<State> states = env.states();
	int n = states.size();
	for (int i = 0; i < n; i++)
		value_func[states[i]] = env.vreward(states[i]);
	
	for (int t = 0; t < trials; t++) {
		// std::cout << "check" << std::endl;
		for (int i = 0; i < n; i++) {
			State s = states[i];
			if (env.terminate(s))
				continue;
			double ans = env.vreward(s);
			vector<Action> actions = env.actions(s);
			for (auto ite = actions.begin(); ite != actions.end(); ite++) {
				double pr = policy(s, *ite);
				double j = 0;
				auto allStates = env.goAll(s, *ite);
				for (auto it2 = allStates.begin(); it2 != allStates.end(); it2++) {
					j += (it2->second) * value_func[it2->first];
				}
				j = j * gamma + env.qreward(s, *ite);
				ans += pr * j;
			}
			value_func[s] = ans;
		}
	}
}