// 最强弹一弹模拟程序
#pragma once
#include "gl_env.h"
#define _DEBUGx

class GameBoard {
private:
    const static float max_u;
    inline float dot(float ax, float ay, float bx, float by) {
        return ax * bx + ay * by;
    }

    inline std::pair<float, float> symp(float px, float py, float ax, float ay, float bx, float by) {
        float dm = (by - ay) * (by - ay) + (bx - ax) * (bx - ax);
        assert(dm > 0);
        float dx = ((2 * ax - px) * (by - ay) * (by - ay)
            + (2 * py - 2 * ay) * (bx - ax) * (by - ay)
            + px * (bx - ax) * (bx - ax)) / dm;
        float dy = ((2 * ay - py) * (bx - ax) * (bx - ax)
            + (2 * px - 2 * ax) * (by - ay) * (bx - ax)
            + py * (by - ay) * (by - ay)) / dm;
        return std::make_pair(dx, dy);
    }

    inline std::pair<float, float> norm(float cx, float cy, float ax, float ay, float bx, float by) {
        auto sp = symp(cx, cy, ax, ay, bx, by);
        return std::make_pair(sp.first - cx, sp.second - cy);
    }

    inline float dist(float ax, float ay, float bx, float by) {
        return sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
    }

    inline std::pair<float, bool> dist(float px, float py, float ax, float ay, float bx, float by) {
        auto sp = symp(px, py, ax, ay, bx, by);
        float dx = sp.first, dy = sp.second;
        float mx = 0.5 * (dx + px), my = 0.5 * (dy + py);
        return std::make_pair(sqrt((dx - px) * (dx - px) + (dy - py) * (dy - py)) / 2.0,
            (mx - ax) * (mx - bx) <= 0 && (my - ay) * (my - by) <= 0);
    }

    inline std::pair<float, float> collide(float vx, float vy, float nx, float ny) {
        auto sp = symp(vx, vy, 0.0, 0.0, nx, ny);
        return std::make_pair(-sp.first, -sp.second);
    }

    // check - 碰撞检查
    // 返回值:
    // 0 - 无碰撞
    // 1 - 有效碰撞
    // 2 - 无效碰撞
    // 3 - 有效碰撞且碰到顶点
    int check(float px, float py, float& vx, float& vy, float r,
        float cx, float cy, float ax, float ay, float bx, float by, float fric) {
        double v = vx * vx + vy * vy;
        if (dist(px, py, ax, ay) < r && dot(vx, vy, px - ax, py - ay) < 0) {
            auto nv = collide(vx, vy, px - ax, py - ay);
            vx = fric * nv.first;
            vy = fric * nv.second;
            return 3;
        }
        if (dist(px, py, bx, by) < r && dot(vx, vy, px - bx, py - by) < 0) {
            auto nv = collide(vx, vy, px - bx, py - by);
            vx = fric * nv.first;
            vy = fric * nv.second;
            return 3;
        }
        auto dt = dist(px, py, ax, ay, bx, by);
        if (dt.first > r || !dt.second)
            return 0;
        auto nm = norm(cx, cy, ax, ay, bx, by);
        float nx = nm.first, ny = nm.second;
        if (dot(vx, vy, nx, ny) >= 0)
            return 0;
        auto nv = collide(vx, vy, nx, ny);
        vx = fric * nv.first;
        vy = fric * nv.second;
        return 1;
    }

    int check(float px, float py, float& vx, float& vy, float r,
        float cx, float cy, float r2, float fric) {
        double v = vx * vx + vy * vy;
        if (dist(px, py, cx, cy) > r + r2)
            return 0;
        float nx = px - cx, ny = py - cy;
        if (dot(vx, vy, nx, ny) >= 0)
            return 0;
        auto nv = collide(vx, vy, nx, ny);
        vx = fric * nv.first;
        vy = fric * nv.second;
        return 1;
    }

    int check(float cx1, float cy1, float& vx1, float& vy1,
        float cx2, float cy2, float& vx2, float& vy2, float r, float fric) {
        float d = dist(cx1, cy1, cx2, cy2);
        float dt = dot(vx1 - vx2, vy1 - vy2, cx1 - cx2, cy1 - cy2);
        if (d > 2.0 * r || dt >= 0)
            return 0;
        float d1 = dt / (d * d);
        float nx1 = vx1 - d1 * (cx1 - cx2);
        float ny1 = vy1 - d1 * (cy1 - cy2);
        float d2 = dt / (d * d);
        float nx2 = vx2 - d2 * (cx2 - cx1);
        float ny2 = vy2 - d2 * (cy2 - cy1);
        vx1 = nx1;
        vy1 = ny1;
        vx2 = nx2;
        vy2 = ny2;
        return 1;
    }
public:
    struct Block {
        static const float triangle_radius, circle_radius, square_radius;
        enum Type {
            TRIANGLE, CIRCLE, SQUARE, NUM_OF_TYPES
        } type;
        int life;
        float colorr, colorg, colorb;
        float centerx, centery, angle;
        // 关于角度angle的说明
        // 一律取逆时针为正
        // 如果是三角形，最上方顶点与center在一竖直线上为0度
        // 如果是正方形，四边与坐标轴平行时为0度

        Block() {
            colorr = float(rand()) / float(RAND_MAX) * 0.3 + 0.55;
            colorg = float(rand()) / float(RAND_MAX) * 0.3 + 0.55;
            colorb = float(rand()) / float(RAND_MAX) * 0.3 + 0.55;
        }
    };

    struct Ball {
        static const float radius;
        static const float colorr, colorg, colorb;
        float centerx, centery, vx, vy, time_death;
    };

    static const float windowx, windowy;
    static const float shooterx, shootery;
    static const float box[8];
    static const float max_shooter_angle;
    static const float shooter_freq;
    static const float birth_line, death_line;
    static const float delta_height;
    static const float init_v;
    static const float time_death;
    static const float valid_v;
    static const float easiest_new_x[5];
    int score, num_of_balls, ball_left;
    float time_turn, time_prev, turn_cnt;
    float shooter_angle;              // 发射器的角度(偏离垂直线，逆时针为正)
    std::vector<Block> blocks;        // 所有砖块
    std::vector<Ball> balls;          // 所有小球
    float ax, ay;                     // 加速度场
    float friction;                   // 摩擦系数
    int testval;
    int mode;
    static const float init_ax, init_ay;

    GameBoard(int game_mode = 0):
        score(0), num_of_balls(3), friction(0.8), shooter_angle(0.0), 
        ax(init_ax), ay(init_ay), turn_cnt(0), mode(game_mode) {
        ball_left = num_of_balls;
        Block bk1, bk2, bk3;
        bk1.type = bk2.type = bk3.type = Block::TRIANGLE;
        bk1.angle = bk2.angle = bk3.angle = 0.0;
        bk1.centerx = 15.0;
        bk2.centerx = 45.0;
        bk3.centerx = 75.0;
        bk1.centery = bk2.centery = bk3.centery = birth_line;
        bk1.life = bk2.life = bk3.life = 6;
        blocks.push_back(bk1);
        blocks.push_back(bk2);
        blocks.push_back(bk3);
        testval = 0;
    }

    void fillTriangle(Block& bk, float& x1, float& y1, float& x2, float& y2,
        float& x3, float& y3) {
        assert(bk.type == Block::TRIANGLE);
        x1 = bk.centerx + bk.triangle_radius * sin(-bk.angle / 180.0 * M_PI);
        y1 = bk.centery + bk.triangle_radius * cos(-bk.angle / 180.0 * M_PI);
        x2 = bk.centerx + bk.triangle_radius * sin((-bk.angle + 120.0) / 180.0 * M_PI);
        y2 = bk.centery + bk.triangle_radius * cos((-bk.angle + 120.0) / 180.0 * M_PI);
        x3 = bk.centerx + bk.triangle_radius * sin((-bk.angle - 120.0) / 180.0 * M_PI);
        y3 = bk.centery + bk.triangle_radius * cos((-bk.angle - 120.0) / 180.0 * M_PI);
    }

    void fillSquare(Block& bk, float& x1, float& y1, float& x2, float& y2,
        float& x3, float& y3, float& x4, float& y4) {
        assert(bk.type == Block::SQUARE);
        x1 = bk.centerx + bk.square_radius * sin((-bk.angle + 45.0) / 180.0 * M_PI);
        y1 = bk.centery + bk.square_radius * cos((-bk.angle + 45.0) / 180.0 * M_PI);
        x2 = bk.centerx + bk.square_radius * sin((-bk.angle - 45.0) / 180.0 * M_PI);
        y2 = bk.centery + bk.square_radius * cos((-bk.angle - 45.0) / 180.0 * M_PI);
        x4 = bk.centerx + bk.square_radius * sin((-bk.angle + 135.0) / 180.0 * M_PI);
        y4 = bk.centery + bk.square_radius * cos((-bk.angle + 135.0) / 180.0 * M_PI);
        x3 = bk.centerx + bk.square_radius * sin((-bk.angle - 135.0) / 180.0 * M_PI);
        y3 = bk.centery + bk.square_radius * cos((-bk.angle - 135.0) / 180.0 * M_PI);
    }

    // shoot - 按照当前角度发射
    void shoot() {
        time_turn = shooter_freq;
        time_prev = 0;
        assert(balls.size() == 0);
    }

    // step - 时间过去t秒钟
    // 要求t足够小
    // 返回值:
    // 0 - 当前回合还在继续进行
    // 1 - 当前回合已经结束，游戏继续
    // 2 - 游戏结束
    int step(float t) {
        time_turn += t;
        testval++;
        if (testval == 591) {
            testval = 590;
        }

        // 1. Balls move forward
        for (auto bl = balls.begin(); bl != balls.end(); bl++) {
            bl->centerx += bl->vx * t;
            bl->centery += bl->vy * t;
            bl->vx += ax * t;
            bl->vy += ay * t;
        }

        // 2. Create a new ball if needed
        if (ball_left > 0 && time_turn > time_prev + shooter_freq) {
            time_prev += shooter_freq;
            ball_left--;
            Ball bl;
            bl.vx = init_v * sin(-shooter_angle * M_PI / 180.0);
            bl.vy = -init_v * cos(-shooter_angle * M_PI / 180.0);
            float t0 = time_turn - time_prev;
            bl.centerx = shooterx + bl.vx * t0;
            bl.centery = shootery + bl.vy * t0;
            bl.vx += ax * t0;
            bl.vy += ay * t0;
            bl.time_death = time_death;
            balls.push_back(bl);
        }

        // 3. Balls and walls
        for (auto bl = balls.begin(); bl != balls.end(); bl++) {
            check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                0.0, 0.0, box[0], box[1], box[2], box[3], friction);
            check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                windowx, 0.0, box[4], box[5], box[6], box[7], friction);
            float mx = 0.5 * (box[0] + box[6]);
            float my = 0.5 * (box[1] + box[7]);
            check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                mx, my, box[0], box[1], shooterx, shootery, friction);
            check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                mx, my, box[6], box[7], shooterx, shootery, friction);
            // check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
            //     0.0, 0.0, box[4], box[5], box[2], box[3], friction);
        }

#define _COLLIDE_PROCESS { \
        bk->life--; \
        score++; \
        }

        // 4. Balls and blocks
        for (auto bl = balls.begin(); bl != balls.end(); bl++) {
            for (auto bk = blocks.begin(); bk != blocks.end(); bk++) {
                float x1, y1, x2, y2, x3, y3, x4, y4;
                switch (bk->type) {
                case Block::TRIANGLE:
                    fillTriangle(*bk, x1, y1, x2, y2, x3, y3);
                    if (check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                        bk->centerx, bk->centery, x1, y1, x2, y2, friction) & 1)
                        _COLLIDE_PROCESS
                    if (check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                        bk->centerx, bk->centery, x3, y3, x2, y2, friction) & 1)
                        _COLLIDE_PROCESS
                    if (check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                        bk->centerx, bk->centery, x1, y1, x3, y3, friction) & 1)
                        _COLLIDE_PROCESS
                    break;
                case Block::SQUARE:
                    fillSquare(*bk, x1, y1, x2, y2, x3, y3, x4, y4);
                    if (check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                        bk->centerx, bk->centery, x1, y1, x2, y2, friction) & 1)
                        _COLLIDE_PROCESS
                    if (check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                        bk->centerx, bk->centery, x3, y3, x2, y2, friction) & 1)
                        _COLLIDE_PROCESS
                    if (check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                        bk->centerx, bk->centery, x3, y3, x4, y4, friction) & 1)
                        _COLLIDE_PROCESS
                    if (check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                        bk->centerx, bk->centery, x1, y1, x4, y4, friction) & 1)
                        _COLLIDE_PROCESS
                    break;
                case Block::CIRCLE:
                    if (check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                        bk->centerx, bk->centery, bk->circle_radius, friction) == 1)
                        _COLLIDE_PROCESS
                }
            }
        }

        // 5. Clean up blocks
        int nblocks = 0;
        for (auto bk = blocks.begin(); bk != blocks.end(); bk++) {
            if (bk->life < 0) {
                // std::cerr << bk->life << std::endl;
                score -= bk->life;
            }
            if (bk->life > 0)
                blocks[nblocks++] = *bk;
        }
        blocks.resize(nblocks);

        // 6. Balls and balls
        for (auto bl = balls.begin(); bl != balls.end(); bl++)
            for (auto bl2 = bl + 1; bl2 != balls.end(); bl2++) {
                check(bl->centerx, bl->centery, bl->vx, bl->vy, bl2->centerx,
                    bl2->centery, bl2->vx, bl2->vy, bl->radius, friction);
            }

        // 7. Clean up balls
        int nballs = 0;
        for (auto bl = balls.begin(); bl != balls.end(); bl++) {
            if (bl->vx * bl->vx + bl->vy * bl->vy < valid_v * valid_v)
                bl->time_death -= t;
            else
                bl->time_death = time_death;
            if (bl->time_death < 0)
                continue;
            if (bl->centery < box[3] && bl->centery < box[5] &&
                dist(bl->centerx, bl->centery, box[2], box[3], box[4], box[5]).first > bl->radius)
                continue;
            balls[nballs++] = *bl;
        }
        balls.resize(nballs);

        // 8. End turn
        if (nballs == 0 && ball_left == 0) {
            int game_over = 1;

            // Blocks go up
            for (auto bk = blocks.begin(); bk != blocks.end(); bk++) {
                bk->centery += delta_height;
                if (bk->centery > death_line)
                    game_over = 2;
            }

            if (game_over == 2)
                return 2;

            // Create new blocks
            switch (mode) {
            case 0:
            {
                // Default mode
                turn_cnt++;
                float r = float(rand()) / float(RAND_MAX);
                if (r * turn_cnt > 1.0) {
                    num_of_balls++;
                    turn_cnt = 0;
                }
                r = float(rand()) / float(RAND_MAX);
                int num_of_babies = int(r * 3) + 1;
                r = float(rand()) / float(RAND_MAX);
                int sum_of_babies = int((r * 4 + 3.5) * num_of_balls);
                float rd[6], rrd[6];
                r = 0;
                for (int i = 0; i <= num_of_babies; i++) {
                    rd[i] = float(rand()) / float(RAND_MAX);
                    r += rd[i];
                }
                for (int i = 0; i <= num_of_babies; i++)
                    rd[i] /= r;
                r = 0;
                for (int i = 0; i < num_of_babies; i++) {
                    rrd[i] = float(rand()) / float(RAND_MAX);
                    r += rrd[i];
                }
                for (int i = 0; i < num_of_babies; i++)
                    rrd[i] /= r;
                float ds = box[4] - box[2] - 8.0 * (num_of_babies + 1);
                float f = box[2] + 8.0;
                for (int i = 0; i < num_of_babies; i++, f += 8.0) {
                    r = float(rand()) / float(RAND_MAX);
                    Block bk;
                    bk.type = Block::Type(int(r * bk.NUM_OF_TYPES));
                    r = float(rand()) / float(RAND_MAX);
                    bk.angle = (r - 0.5) * 180.0;
                    f += rd[i] * ds;
                    bk.centerx = f;
                    bk.centery = birth_line;
                    int life = (i == num_of_babies - 1) ? sum_of_babies : int(rrd[i] * sum_of_babies);
                    if (life == 0)
                        continue;
                    sum_of_babies -= life;
                    bk.life = life;
                    blocks.push_back(bk);
                    if (sum_of_babies <= 0)
                        break;
                }
            }
            break;
            case 1:
            {
                // Easiest mode
                for (int i = 0; i < 5; i++) {
                    float r = float(rand()) / float(RAND_MAX);
                    if (r < 0.5) {
                        Block bk;
                        bk.angle = 0.0;
                        bk.centerx = easiest_new_x[i];
                        bk.centery = birth_line;
                        bk.type = Block::TRIANGLE;
                        r = float(rand()) / float(RAND_MAX);
                        bk.life = int(r * 6.0 + 4.0);
                        blocks.push_back(bk);
                    }
                }
            }
            }
            ball_left = num_of_balls;
            return 1;
        }

        return 0;
    }
};  // class GameBoard

// ======================================================
// 参数调试区

// 整个屏幕的大小
const float GameBoard::windowx = 90.0;
const float GameBoard::windowy = 160.0;
// 发射口的位置
const float GameBoard::shooterx = 45.0;
const float GameBoard::shootery = 140.0;
// 发射器最大角度
const float GameBoard::max_shooter_angle = 80.0;
// 墙的位置(用一个盒子表示，分别代表左上角、左下角、右下角、右上角)
const float GameBoard::box[8] = {
    10.0, 150.0, 10.0, 15.0, 80.0, 15.0, 80.0, 150.0
};
// 砖块的半径(中心点到顶点的距离)
const float GameBoard::Block::triangle_radius = 5.0;
const float GameBoard::Block::square_radius = 5.0;
const float GameBoard::Block::circle_radius = 4.0;
// 小球的半径
const float GameBoard::Ball::radius = 2.0;
// 小球的颜色
const float GameBoard::Ball::colorr = 0.4;
const float GameBoard::Ball::colorg = 0.4;
const float GameBoard::Ball::colorb = 0.4;
// 发射两个小球之间间隔的时间
const float GameBoard::shooter_freq = 0.3;
// 出生线和死亡线
const float GameBoard::birth_line = 25.0;
const float GameBoard::death_line = 130.0;
// 每回合上升的高度
const float GameBoard::delta_height = 13.0;
// 发射器给予小球的初始速度
const float GameBoard::init_v = 100.0;
// 小球死亡时间
const float GameBoard::time_death = 5.0;
const float GameBoard::valid_v = 5.0;
const float GameBoard::max_u = -0.01;
const float GameBoard::easiest_new_x[5] = { 15.0, 30.0, 45.0, 60.0, 75.0 };
const float GameBoard::init_ax = 0.0;
const float GameBoard::init_ay = -98.0;

// ======================================================

// 可视化游戏界面
namespace Viewer {
    void checkGL()
    {
#ifdef _DEBUGa
        glFinish();
        GLenum error = glGetError();
        assert(error == GL_NO_ERROR);
#endif
    }

    inline void init(int *argc, char **argv) {
        glutInit(argc, argv);
    }

    float time_prev;
    GameBoard *gb;
    bool grid;
    typedef void(*ai_func)(float&);
    ai_func ai = nullptr;
    ai_func learning = nullptr;
    void(*k_func)();
    float mousex, mousey;
    int mouseinit = 0, timeinit = 0, hintline = 1;
    int lock = 0;
    int window_width = 360, window_height = 640;
    const float max_t = 0.01;

    inline void drawString(const char* str) {
        for (; *str; str++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *str);
    }

    inline void drawCircle(float x, float y, float r) {
        const int num_of_segments = 500;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int i = 0; i <= num_of_segments; i++)
        {
            glVertex2f(x + r * cos((2.0 * M_PI * i) / num_of_segments),
                y + r * sin((2.0 * M_PI * i) / num_of_segments));
        }
        glEnd();
    }

    inline void step() {
        gb->shoot();
        lock = 1;
        time_prev = clock() / float(CLOCKS_PER_SEC);
    }

    void display() {
        // Initialization
        if (!timeinit) {
            timeinit = 1;
            time_prev = clock() / float(CLOCKS_PER_SEC);
        }
        if (lock == 1) {
            float time_now = clock() / float(CLOCKS_PER_SEC);
            float t = time_now - time_prev;
            if (t > max_t)
                t = max_t;
            int ans = gb->step(t);
            time_prev = time_now;
            if (ans == 1)
                lock = 0;
            else if (ans == 2)
                lock = 2;
        }

        // AI
        if (lock == 0 && ai != nullptr) {
            ai(gb->shooter_angle);
            if (learning == nullptr)
                step();
        }


        glClear(GL_COLOR_BUFFER_BIT);

        // 1. Draw shooter
        glColor3f(0.8, 0.5, 0.0);
        glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(gb->shooterx + 10.0 * sin((20.0 + gb->shooter_angle) / 180.0 * M_PI),
                   gb->shootery + 10.0 * cos((20.0 + gb->shooter_angle) / 180.0 * M_PI));
        glVertex2f(gb->shooterx, gb->shootery);
        glVertex2f(gb->shooterx + 7.0 * sin((gb->shooter_angle) / 180.0 * M_PI),
                   gb->shootery + 7.0 * cos((gb->shooter_angle) / 180.0 * M_PI));
        glVertex2f(gb->shooterx + 10.0 * sin((-20.0 + gb->shooter_angle) / 180.0 * M_PI),
                   gb->shootery + 10.0 * cos((-20.0 + gb->shooter_angle) / 180.0 * M_PI));
        glEnd();
        checkGL();

        // 2. Draw blocks and mark lives
        for (auto bk = gb->blocks.begin(); bk != gb->blocks.end(); bk++) {
            glColor3f(bk->colorr, bk->colorg, bk->colorb);
            float x1, y1, x2, y2, x3, y3, x4, y4;
            switch (bk->type) {
            case GameBoard::Block::TRIANGLE:
                gb->fillTriangle(*bk, x1, y1, x2, y2, x3, y3);
                glBegin(GL_TRIANGLES);
                glVertex2f(x1, y1);
                glVertex2f(x2, y2);
                glVertex2f(x3, y3);
                glEnd();
                break;
            case GameBoard::Block::SQUARE:
                gb->fillSquare(*bk, x1, y1, x2, y2, x3, y3, x4, y4);
                glBegin(GL_TRIANGLE_STRIP);
                glVertex2f(x1, y1);
                glVertex2f(x2, y2);
                glVertex2f(x4, y4);
                glVertex2f(x3, y3);
                glEnd();
                break;
            case GameBoard::Block::CIRCLE:
                drawCircle(bk->centerx, bk->centery, bk->circle_radius);
            }
            glColor3f(0.0, 0.0, 0.0);
            glRasterPos2f(bk->centerx - 2.0, bk->centery);
            char str[10];
            sprintf(str, "%d", bk->life);
            drawString(str);
        }
        checkGL();

        // 3. Draw balls
        glColor3f(GameBoard::Ball::colorr, GameBoard::Ball::colorg, GameBoard::Ball::colorb);
        for (auto bl = gb->balls.begin(); bl != gb->balls.end(); bl++)
            drawCircle(bl->centerx, bl->centery, bl->radius);
        checkGL();

        // 4. Draw walls and fill borders
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(0.0, 0.0);
        glVertex2f(GameBoard::windowx, 0.0);
        glVertex2f(0.0, gb->box[3]);
        glVertex2f(GameBoard::windowx, gb->box[3]);
        glEnd();
        glColor3f(0.0, 0.4, 0.2);
        glBegin(GL_LINES);
        glVertex2f(gb->box[0], gb->box[1]);
        glVertex2f(gb->box[0] * 0.05 + gb->shooterx * 0.95, gb->box[1] * 0.05 + gb->shootery * 0.95);
        glVertex2f(gb->box[0], gb->box[1]);
        glVertex2f(gb->box[2], gb->box[3]);
        glVertex2f(gb->box[6], gb->box[7]);
        glVertex2f(gb->box[6] * 0.05 + gb->shooterx * 0.95, gb->box[7] * 0.05 + gb->shootery * 0.95);
        glVertex2f(gb->box[6], gb->box[7]);
        glVertex2f(gb->box[4], gb->box[5]);
        glColor3f(0.8, 0.0, 0.4);
        glVertex2f(gb->box[2], gb->box[3]);
        glVertex2f(gb->box[4], gb->box[5]);
        glEnd();
        checkGL();

        // 5. Draw hint line
        if (!lock && mouseinit) {
            const float delta_t = 0.05;
            glColor3f(1.0, 0.0, 0.0);
            glLineStipple(2, 0x5555);
            glEnable(GL_LINE_STIPPLE);
            if (hintline == 0) {
                glBegin(GL_LINES);
                glVertex2f(GameBoard::shooterx, GameBoard::shootery);
                glVertex2f(mousex, mousey);
                glEnd();
            }
            else {
                float t = 0, tx = gb->shooterx, ty = gb->shootery;
                glBegin(GL_LINE_STRIP);
                while (ty > mousey) {
                    glVertex2f(tx, ty);
                    t += delta_t;
                    tx = gb->shooterx - gb->init_v * sin(gb->shooter_angle * M_PI / 180.0) * t
                        + 0.5 * gb->ax * t * t;
                    ty = gb->shootery - gb->init_v * cos(gb->shooter_angle * M_PI / 180.0) * t
                        + 0.5 * gb->ay * t * t;
                }
                glVertex2f(mousex, mousey);
                glEnd();
            }
            glDisable(GL_LINE_STIPPLE);
            glColor3f(0.5, 0.5, 0.5);
            drawCircle(float(mousex), float(mousey), GameBoard::Ball::radius);
        }
        checkGL();

        // 6. Show score and number of balls
        glColor3f(0.0, 0.0, 0.0);
        glRasterPos2f(GameBoard::windowx * 0.1, GameBoard::windowy - 8.0);
        char str[30];
        sprintf(str, "Score: %d", gb->score);
        drawString(str);
        glRasterPos2f(GameBoard::shooterx, GameBoard::shootery + 10.0);
        sprintf(str, "%d", gb->ball_left);
        drawString(str);
        checkGL();

        // 7. Game Over
        if (lock == 2) {
            float mx = GameBoard::windowx / 2, my = GameBoard::windowy / 2;
            glColor3f(0.5, 0.5, 0.5);
            glBegin(GL_TRIANGLE_STRIP);
            glVertex2f(mx - 20.0, my - 10.0);
            glVertex2f(mx - 20.0, my + 10.0);
            glVertex2f(mx + 20.0, my - 10.0);
            glVertex2f(mx + 20.0, my + 10.0);
            glEnd();
            glColor3f(1.0, 0.0, 0.0);
            glRasterPos2d(mx - 15.0, my + 3.0);
            drawString("Game Over!");
            glColor3f(0.0, 0.0, 0.0);
            glRasterPos2f(mx - 15.0, my - 3.0);
            sprintf(str, "Score: %d", gb->score);
            drawString(str);
        }

        // Termination
        glutSwapBuffers();
        glutPostRedisplay();
    }

    float calc(float px, float py) {
        float theta = -gb->max_shooter_angle;
        const float delta_theta = 0.5;
        const float delta_t = delta_theta * M_PI / 180.0;
        float k;
        float x = px - gb->shooterx;
        float y = py - gb->shootery;
        float mt = gb->max_shooter_angle + 1.0, mk = 0.5;
        while (theta <= gb->max_shooter_angle) {
            float t = theta * M_PI / 180.0;
            float vs = gb->init_v * sin(-t);
            float vc = gb->init_v * cos(-t);
            assert(gb->ay != 0);
            float ct = (vc - sqrt(vc * vc + 2 * y * gb->ay)) / gb->ay;
            k = fabs(vs * ct + 0.5 * gb->ax * ct * ct - x);
            if (k < mk) {
                mk = k;
                mt = theta;
            }
            theta += delta_theta;
        }
        return mt;
    }

    void motion(int x, int y) {
        if (lock)
            return;
        float px = float(x) / float(window_width) * GameBoard::windowx;
        float py = (1.0 - float(y) / float(window_height)) * GameBoard::windowy;
        mouseinit = 1;
        mousex = px;
        mousey = py;
        if (hintline == 0)
            gb->shooter_angle = atan((GameBoard::shooterx - px) / (GameBoard::shootery - py)) / M_PI * 180.0;
        else
            gb->shooter_angle = calc(px, py);
        if (grid)
            gb->shooter_angle = 5.0 * round(gb->shooter_angle / 5.0);
        if (py >= gb->shootery ||
            gb->shooter_angle > gb->max_shooter_angle || gb->shooter_angle < -gb->max_shooter_angle) {
            gb->shooter_angle = 0;
            mouseinit = 0;
        }
        glutPostRedisplay();
    }

    void reshape(int width, int height) {
        glViewport(0, 0, (GLsizei)width, (GLsizei)height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, gb->windowx, 0.0, gb->windowy);
        glClear(GL_COLOR_BUFFER_BIT);
        window_width = width;
        window_height = height;
    }

    void mouse(int button, int state, int x, int y) {
        if (lock || !mouseinit || state != GLUT_DOWN)
            return;
        if (ai != nullptr) {
            if (learning == nullptr)
                return;
            if (button == GLUT_LEFT_BUTTON) {
                float tmp = gb->shooter_angle;
                learning(tmp);
            }
        }
        if (button == GLUT_LEFT_BUTTON)
            step();
        else if (button == GLUT_RIGHT_BUTTON) {
            hintline = 1 - hintline;
            motion(x, y);
        }
    }

    void keyboard(unsigned char key, int x, int y) {
        if (lock == 1)
            return;
        if (key == 's')
            k_func();
    }

    // showWindow - 显示图形化界面
    // 注意，一个程序只能显示一个界面
    void showWindow(GameBoard &board, ai_func ai_function = nullptr,
        ai_func is_learning = nullptr, bool in_grid = false, void(*func)() = nullptr) {
        gb = &board;
        ai = ai_function;
        grid = in_grid;
        learning = is_learning;
        k_func = func;
        glutInitWindowPosition(100, 20);
        glutInitWindowSize(window_width, window_height);
        glutCreateWindow("Tan Yi Tan!");
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

        glClearColor(1.0, 1.0, 1.0, 0.0);
        glMatrixMode(GL_PROJECTION);
        gluOrtho2D(0.0, board.windowx, 0.0, board.windowy);

        glutPassiveMotionFunc(motion);
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutMouseFunc(mouse);
        if (k_func != nullptr)
            glutKeyboardFunc(keyboard);

        glutMainLoop();
    }
}  // namespace Viewer