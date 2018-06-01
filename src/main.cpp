#include "gl_env.h"
#ifndef DLL_MODE
#include "irl.h"
#include "gridworld.h"
#include "zqtyt.h"
#include "easy_model.h"

#define DEBUGGL
#define LEARNING

void k_func() {
    EasyModelAI::save("test.txt", "pre.txt");
}

int main(int argc, char **argv) {
    irl_init();
#ifdef DEBUGGL
    Viewer::init(&argc, argv);
    //GameBoard gb(0);
    //Viewer::showWindow(gb, nullptr, false);
    EasyModelAI::init("test.txt", "pre.txt", k_func);

#ifdef LEARNING
    int r = 0;
    while (true) {
        qlearning(EasyModelAI::em, EasyModelAI::qtable, nullptr, 10000);
        if (r == 0)
            EasyModelAI::save("test2.txt");
        else
            EasyModelAI::save("test.txt");
        r = 1 - r;
    }
#else
    
    EasyModelAI::em.first();
    EasyModelAI::showWindow(true);
#endif
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
    //mc(gw, p, mp, aux);
    //td(gw, p, mp, trace, aux);
    qlearning(gw, p);
    /*
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            std::cout << mp[std::make_pair(i, j)] << "  ";
        }
        std::cout << std::endl;
    }
    */

#endif
    return 0;
}
#endif