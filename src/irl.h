#pragma once
// ���ڻ���E
// ����template�����е�Eָ����һ��������
// �����������������
// typedef State                         ״̬������
// typedef Action                        ����������
// vector<Action> actions(State s)       ��ĳ��״̬�����������ж���
// State go(State &s, Action &a)         ��һ��״̬��һ�������������һ��״̬(�������)
// vector<pair<State, double>> goAll(State &s, Action &a)
//                                       ��������������ܵ����ÿһ��״̬�Լ�����
// State first()                         ��ó�ʼ״̬(��ѡ)
// vector<State> states()                �������״̬(��ѡ)
// double vreward(State s)               ����ĳһ״̬��˲����Ի�õ�reward
// double qreward(State s, Action a)     ��ĳһ״̬��ĳһ�����ܵõ��ļ�ʱreward
// bool terminate(State s)               ����ĳһ��״̬�Ƿ�����ֹ״̬

// ����Policy P
// ���е�Pָ����һ��Policy�࣬Ҳ�������ڴ��q(s,a)
// Policy�������������
// double(&) operator ()(State &s, Action &a) ��s������a�ĸ���
// ĳЩ�޸�policy�ĺ���Ҫ�󷵻�ֵ������һ������

// ����Map M
// ���е�Mָ����һ��ӳ���࣬�������ڴ��value function
// double& operator [](State &s)
// һ����Զ���Ϊ map<State, double>
// ����map�Ƚ����������Լ��������Ż��ٶ�

#include <map>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
const double oo = 1e10;

// dp - Dynamic Programming algorithm
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

// dp - In-place dynamic Programming algorithm
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