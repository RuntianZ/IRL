# Inverse Reinforcement Learning
作者：翟润天(1600012737)，范旻昊(1600012748)  
这是算法设计与分析的课程项目，目标是实现RL和IRL的各种算法，并用它们来玩最强弹一弹。
![avatar](http://www.runtianz.cn/img/tytgif.gif)
#### 1. 强化学习算法
所有和强化学习算法相关的代码都放在<a href="https://github.com/RuntianZ/IRL/blob/master/src/irl.h">src/irl.h</a>文件内，其中所有算法都用模板函数实现。请阅读该文件头部的模板说明，使用其中的函数是非常容易的。实现的算法包括：
+ 动态规划(dp)
+ 蒙特卡洛(mc)
+ 时序差分(td)
+ Sarsa
+ Q-Learning
+ 函数估计法
  
这里我们实现了一个简单的GridWorld环境，你可以用它作为样例来测试各种算法，也可以仿照GridWorld类的写法编写你自己的环境类，并使用各个函数。  
#### 2. 最强弹一弹
我们编写了一个简单的最强弹一弹模拟器，放在GameBoard类中。你可以在此基础上包装一个环境类，从而使用各种算法。  
游戏说明：按下鼠标左键发射小球。你也可以单击鼠标右键开启或关闭重力提示。  
#### 3. 如何使用
我们推荐你使用Visual Studio 2017。你只需要打开目录下的VC++工程就可以编译、运行程序了。
