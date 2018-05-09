#include "irl.h"
#include "gridworld.h"

GridWorld gw(5, 5);

int main() {
	irl_init();

	gw.setReward(0, 0, 10.0);
	gw.setReward(4, 4, 10.0);
	gw.setReward(0, 4, 7.0);
	gw.setReward(4, 0, 7.0);
	GridWorld::Map mp, mp2;
	gw.loadMap(mp);
	gw.loadMap(mp2);
	GridWorld::Policy p;
	gw.loadPolicy(p);
	// policy_iteration(gw, p, mp);
	// value_iteration(gw, mp);
	// vgreedy(gw, p, mp);
	mc(gw, p, mp, mp2);
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			std::cout << mp[std::make_pair(i, j)] << "  ";
		}
		std::cout << std::endl;
	}

	return 0;
}