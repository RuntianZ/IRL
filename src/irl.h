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
// double& operator ()(State &s, Action &a) ��s������a�ĸ���
// ĳЩ����Ҫ�󷵻�ֵ������һ������

// ����Map M
// ���е�Mָ����һ��ӳ���࣬�������ڴ��value function
// MҪ�����������[]
// һ����Զ���Ϊ map<State, double>
// ����map�Ƚ����������Լ��������Ż��ٶ�

#include <map>
#include <vector>
#include <iostream>
const double oo = 1e10;

// dp - Dynamic Programming algorithm
template <class E, class P, class M>
void dp(E& env, P& policy, M& value_func, double gamma = 0.9, int trials = 1000) {
	typedef typename E::State State;
	typedef typename E::Action Action;
	auto states = env.states();
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
			auto actions = env.actions(s);
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
	int n = states.size();

	for (int i = 0; i < n; i++) {
		State s = states[i];
		auto actions = env.actions(s);
		if (actions.size() == 0)
			continue;
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