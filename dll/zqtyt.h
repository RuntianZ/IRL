#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <ctime>
#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

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

    // check - ��ײ���
    // ����ֵ:
    // 0 - ����ײ
    // 1 - ��Ч��ײ
    // 2 - ��Ч��ײ
    // 3 - ��Ч��ײ����������
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
        // ���ڽǶ�angle��˵��
        // һ��ȡ��ʱ��Ϊ��
        // ����������Σ����Ϸ�������center��һ��ֱ����Ϊ0��
        // ����������Σ��ı���������ƽ��ʱΪ0��

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
    float shooter_angle;              // �������ĽǶ�(ƫ�봹ֱ�ߣ���ʱ��Ϊ��)
    std::vector<Block> blocks;        // ����ש��
    std::vector<Ball> balls;          // ����С��
    float ax, ay;                     // ���ٶȳ�
    float friction;                   // Ħ��ϵ��
    int testval;
    int mode;
    static const float init_ax, init_ay;

    GameBoard(int game_mode = 0, bool init_blocks = true) :
        score(0), num_of_balls(3), friction(0.8), shooter_angle(0.0),
        ax(init_ax), ay(init_ay), turn_cnt(0), mode(game_mode) {
        srand((unsigned int)time(0));
        ball_left = num_of_balls;
        if (!init_blocks) {
            return;
        }
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

    // shoot - ���յ�ǰ�Ƕȷ���
    void shoot() {
        time_turn = shooter_freq;
        time_prev = 0;
        assert(balls.size() == 0);
        // std::cout << shooter_angle << std::endl;
    }

    // step - ʱ���ȥt����
    // Ҫ��t�㹻С
    // ����ֵ:
    // 0 - ��ǰ�غϻ��ڼ�������
    // 1 - ��ǰ�غ��Ѿ���������Ϸ����
    // 2 - ��Ϸ����
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
    // ����������

    // ������Ļ�Ĵ�С
const float GameBoard::windowx = 90.0;
const float GameBoard::windowy = 160.0;
// ����ڵ�λ��
const float GameBoard::shooterx = 45.0;
const float GameBoard::shootery = 140.0;
// ���������Ƕ�
const float GameBoard::max_shooter_angle = 80.0;
// ǽ��λ��(��һ�����ӱ�ʾ���ֱ�������Ͻǡ����½ǡ����½ǡ����Ͻ�)
const float GameBoard::box[8] = {
    10.0, 150.0, 10.0, 15.0, 80.0, 15.0, 80.0, 150.0
};
// ש��İ뾶(���ĵ㵽����ľ���)
const float GameBoard::Block::triangle_radius = 5.0;
const float GameBoard::Block::square_radius = 5.0;
const float GameBoard::Block::circle_radius = 4.0;
// С��İ뾶
const float GameBoard::Ball::radius = 2.0;
// С�����ɫ
const float GameBoard::Ball::colorr = 0.4;
const float GameBoard::Ball::colorg = 0.4;
const float GameBoard::Ball::colorb = 0.4;
// ��������С��֮������ʱ��
const float GameBoard::shooter_freq = 0.3;
// �����ߺ�������
const float GameBoard::birth_line = 25.0;
const float GameBoard::death_line = 130.0;
// ÿ�غ������ĸ߶�
const float GameBoard::delta_height = 13.0;
// ����������С��ĳ�ʼ�ٶ�
const float GameBoard::init_v = 100.0;
// С������ʱ��
const float GameBoard::time_death = 5.0;
const float GameBoard::valid_v = 5.0;
const float GameBoard::max_u = -0.01;
const float GameBoard::easiest_new_x[5] = { 15.0, 30.0, 45.0, 60.0, 75.0 };
const float GameBoard::init_ax = 0.0;
const float GameBoard::init_ay = -98.0;