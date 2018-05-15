#include "irl.h"
#include "gridworld.h"
#include "zqtyt.h"
#include "easy_model.h"
#define DEBUGGLx

// EasyModel em;
// EasyModel::Policy p;
// EasyModel::Map mp;

int main(int argc, char **argv) {
    irl_init();
#ifdef DEBUGGL
    GameBoard gb(0);
    Viewer::init(&argc, argv);
    Viewer::showWindow(gb);
#else
    GridWorld gw(5, 5);
    gw.setReward(0, 0, 10.0);
    gw.setReward(4, 4, 10.0);
    gw.setReward(0, 4, 7.0);
    gw.setReward(4, 0, 7.0);
    GridWorld::Map<double> mp;
    GridWorld::Map<double> trace;
    GridWorld::Map<int> aux;
    gw.loadMap(mp);
    gw.loadMap(trace);
    gw.loadMap(aux);
    GridWorld::Policy<double> p;
    gw.loadPolicy(p);
    //policy_iteration(gw, p, mp);
    //value_iteration(gw, mp);
    //vgreedy(gw, p, mp);
    //mc(gw, p, mp, cnt);
    td(gw, p, mp, trace, aux);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            std::cout << mp[std::make_pair(i, j)] << "  ";
        }
        std::cout << std::endl;
    }

#endif
    return 0;
}