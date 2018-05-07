#include "irl.h"
#include "gridworld.h"

GridWorld gw(5, 5);

int main() {
	gw.setReward(0, 0, 10.0);
	gw.setReward(4, 4, 10.0);
	gw.setReward(0, 4, 7.0);
	gw.setReward(4, 0, 7.0);
	GridWorld::Map mp;
	gw.loadMap(mp);
	GridWorld::Policy p;
	gw.loadPolicy(p);
	dp(gw, p, mp, 0.9, 2000);
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			std::cout << mp[std::make_pair(i, j)] << "  ";
		}
		std::cout << std::endl;
	}
	vgreedy(gw, p, mp, 0.9);

	return 0;
}