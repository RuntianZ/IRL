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
// ���е�Pָ����һ��Policy��
// Policy�������������
// double operator ()(State &s, Action &a) ��s������a�ĸ���

// ����Map M
// ���е�Mָ����һ��ӳ���࣬�������ڴ��value function
// MҪ�����������[]
// һ����Զ���Ϊ map<State, double>
// ����map�Ƚ����������Լ��������Ż��ٶ�

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