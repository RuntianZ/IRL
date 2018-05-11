// 最强弹一弹模拟程序
#include "gl_env.h"

class GameBoard {
private:
    inline float dot(float ax, float ay, float bx, float by) {
        return ax * bx + ay * by;
    }

    inline std::pair<float, float> symp(float px, float py, float ax, float ay, float bx, float by) {
        float dx = ((2 * ax - px) * (by - ay) * (by - ay)
            + (2 * py - 2 * ay) * (bx - ax) * (by - ay)
            + px * (bx - ax) * (bx - ax))
            / ((by - ay) * (by - ay) + (bx - ax) * (bx - ax));
        float dy = (px - dx) * (bx - ax) / (by - ay) + py;
        return std::make_pair(dx, dy);
    }

    inline std::pair<float, float> norm(float cx, float cy, float ax, float ay, float bx, float by) {
        auto sp = symp(cx, cy, ax, ay, bx, by);
        return std::make_pair(sp.first - cx, sp.second - cy);
    }

    inline float dist(float px, float py, float ax, float ay, float bx, float by) {
        auto sp = symp(px, py, ax, ay, bx, by);
        double dx = sp.first, dy = sp.second;
        return sqrt((dx - px) * (dx - px) + (dy - py) * (dy - py)) / 2.0;
    }

    inline std::pair<float, float> collide(float vx, float vy, float nx, float ny) {
        auto sp = symp(vx, vy, 0.0, 0.0, nx, ny);
        return std::make_pair(-sp.first, -sp.second);
    }

    void check(float px, float py, float& vx, float& vy, float r,
        float cx, float cy, float ax, float ay, float bx, float by, float fric) {
        if (dist(px, py, ax, ay, bx, by) > r)
            return;
        auto nm = norm(cx, cy, ax, ay, bx, by);
        double nx = nm.first, ny = nm.second;
        if (dot(vx, vy, nx, ny) >= 0)
            return;
        auto nv = collide(vx, vy, nx, ny);
        vx = fric * nv.first;
        vy = fric * nv.second;
    }
public:
    struct Block {
        static const float triangle_radius, circle_radius, square_radius;
        enum Type {
            TRIANGLE, CIRCLE, SQUARE
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
        float centerx, centery, vx, vy;
    };

    static const float windowx, windowy;
    static const float shooterx, shootery;
    static const float box[8];
    static const float max_shooter_angle;
    static const float shooter_freq;
    static const float birth_line, death_line;
    static const float delta_height;
    static const float init_v;
    int score, num_of_balls, ball_left;
    float time_turn, time_prev;
    float shooter_angle;              // 发射器的角度(偏离垂直线，逆时针为正)
    std::vector<Block> blocks;        // 所有砖块
    std::vector<Ball> balls;          // 所有小球
    float ax, ay;                     // 加速度场
    float friction;                   // 摩擦系数

    GameBoard():
        score(0), num_of_balls(3), friction(0.5), ax(0.0), ay(0.0) {
        shooter_angle = 10.0;
        Block t, s, c;
        t.type = Block::TRIANGLE;
        t.centerx = 40.0;
        t.centery = 40.0;
        t.angle = 20.0;
        t.life = 5;
        s.type = Block::SQUARE;
        s.centerx = 40.0;
        s.centery = 80.0;
        s.angle = -10.0;
        s.life = 50;
        c.type = Block::CIRCLE;
        c.centerx = 70.0;
        c.centery = 40.0;
        c.life = 500;
        blocks.push_back(t);
        blocks.push_back(s);
        blocks.push_back(c);
        Ball b1, b2;
        b1.centerx = 70.0;
        b1.centery = 80.0;
        b1.vx = 0.0;
        b1.vy = 0.0;
        b2.centerx = 55.0;
        b2.centery = 80.0;
        b2.vx = 0.0;
        b2.vy = 0.0;
        balls.push_back(b1);
        balls.push_back(b2);
        time_turn = 0;
        ball_left = 0;
    }

    // shoot - 按照当前角度发射
    void shoot() {
        time_turn = 0;
        ball_left = num_of_balls;
    }

    // step - 时间过去t秒钟
    // 要求t足够小
    // 返回值:
    // 0 - 当前回合还在继续进行
    // 1 - 当前回合已经结束，游戏继续
    // 2 - 游戏结束
    int step(float t) {
        time_turn += t;

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
            bl.vx = init_v * sin(shooter_angle * M_PI / 180.0);
            bl.vy = -init_v * cos(shooter_angle * M_PI / 180.0);
            float t0 = time_turn - time_prev;
            bl.centerx = shooterx + bl.vx * t0;
            bl.centery = shootery + bl.vy * t0;
            bl.vx += ax * t0;
            bl.vy += ay * t0;
            balls.push_back(bl);
        }

        // 3. Balls and walls
        for (auto bl = balls.begin(); bl != balls.end(); bl++) {
            check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                0.0, 0.0, box[0], box[1], box[2], box[3], friction);
            check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                windowx, 0.0, box[4], box[5], box[6], box[7], friction);
            double mx = 0.5 * (box[0] + box[6]);
            double my = 0.5 * (box[1] + box[7]);
            check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                mx, my, box[0], box[1], shooterx, shootery, friction);
            check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                mx, my, box[6], box[7], shooterx, shootery, friction);
            check(bl->centerx, bl->centery, bl->vx, bl->vy, bl->radius,
                0.0, 0.0, box[4], box[5], box[2], box[3], friction);
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
// 墙的位置(用一个盒子表示，分别代表左上角、左下角、右上角、右下角)
const float GameBoard::box[8] = {
    5.0, 150.0, 5.0, 15.0, 85.0, 15.0, 85.0, 150.0
};
// 砖块的半径(中心点到顶点的距离)
const float GameBoard::Block::triangle_radius = 6.0;
const float GameBoard::Block::square_radius = 6.0;
const float GameBoard::Block::circle_radius = 5.0;
// 小球的半径
const float GameBoard::Ball::radius = 2.0;
// 小球的颜色
const float GameBoard::Ball::colorr = 0.4;
const float GameBoard::Ball::colorg = 0.4;
const float GameBoard::Ball::colorb = 0.4;
// 发射两个小球之间间隔的时间
const float GameBoard::shooter_freq = 0.3;
// 出生线和死亡线
const float GameBoard::birth_line = 20.0;
const float GameBoard::death_line = 130.0;
// 每回合上升的高度
const float GameBoard::delta_height = 15.0;
// 发射器给予小球的初始速度
const float GameBoard::init_v = 5.0;

// ======================================================

// 可视化游戏界面
namespace Viewer {
    void init(int *argc, char **argv) {
        glutInit(argc, argv);
    }

    float time_prev;
    GameBoard *gb;
    float mousex, mousey;
    int mouseinit = 0, timeinit = 0;
    int lock = 1;
    int window_width = 360, window_height = 640;
    const float max_t = 0.001;

    void drawString(const char* str) {
        for (; *str; str++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *str);
    }

    void drawCircle(float x, float y, float r) {
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

    void display_main() {
        // Initialization
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

        // 2. Draw blocks and mark lives
        for (auto bk = gb->blocks.begin(); bk != gb->blocks.end(); bk++) {
            glColor3f(bk->colorr, bk->colorg, bk->colorb);
            switch (bk->type) {
            case GameBoard::Block::TRIANGLE:
                glBegin(GL_TRIANGLES);
                glVertex2f(bk->centerx + bk->triangle_radius * sin(-bk->angle / 180.0 * M_PI), 
                           bk->centery + bk->triangle_radius * cos(-bk->angle / 180.0 * M_PI));
                glVertex2f(bk->centerx + bk->triangle_radius * sin((-bk->angle + 120.0) / 180.0 * M_PI),
                           bk->centery + bk->triangle_radius * cos((-bk->angle + 120.0) / 180.0 * M_PI));
                glVertex2f(bk->centerx + bk->triangle_radius * sin((-bk->angle - 120.0) / 180.0 * M_PI),
                           bk->centery + bk->triangle_radius * cos((-bk->angle - 120.0) / 180.0 * M_PI));
                glEnd();
                break;
            case GameBoard::Block::SQUARE:
                glBegin(GL_TRIANGLE_STRIP);
                glVertex2f(bk->centerx + bk->square_radius * sin((-bk->angle + 45.0) / 180.0 * M_PI),
                           bk->centery + bk->square_radius * cos((-bk->angle + 45.0) / 180.0 * M_PI));
                glVertex2f(bk->centerx + bk->square_radius * sin((-bk->angle - 45.0) / 180.0 * M_PI),
                           bk->centery + bk->square_radius * cos((-bk->angle - 45.0) / 180.0 * M_PI));
                glVertex2f(bk->centerx + bk->square_radius * sin((-bk->angle + 135.0) / 180.0 * M_PI),
                           bk->centery + bk->square_radius * cos((-bk->angle + 135.0) / 180.0 * M_PI));
                glVertex2f(bk->centerx + bk->square_radius * sin((-bk->angle - 135.0) / 180.0 * M_PI),
                           bk->centery + bk->square_radius * cos((-bk->angle - 135.0) / 180.0 * M_PI));
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

        // 3. Draw balls
        glColor3f(GameBoard::Ball::colorr, GameBoard::Ball::colorg, GameBoard::Ball::colorb);
        for (auto bl = gb->balls.begin(); bl != gb->balls.end(); bl++)
            drawCircle(bl->centerx, bl->centery, bl->radius);

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

        // 5. Draw hint line
        if (!lock && mouseinit) {
            glColor3f(1.0, 0.0, 0.0);
            glLineStipple(2, 0x5555);
            glEnable(GL_LINE_STIPPLE);
            glBegin(GL_LINES);
            glVertex2f(GameBoard::shooterx, GameBoard::shootery);
            glVertex2f(mousex, mousey);
            glEnd();
            glDisable(GL_LINE_STIPPLE);
            glColor3f(0.5, 0.5, 0.5);
            drawCircle(float(mousex), float(mousey), GameBoard::Ball::radius);
        }

        // 6. Show score and number of balls
        glColor3f(0.0, 0.0, 0.0);
        glRasterPos2f(GameBoard::windowx * 0.1, GameBoard::windowy - 8.0);
        char str[30];
        sprintf(str, "Score: %d", gb->score);
        drawString(str);
        glRasterPos2f(GameBoard::shooterx, GameBoard::shootery + 10.0);
        sprintf(str, "%d", gb->ball_left);
        drawString(str);

        // Termination
        glFlush();
    }

    void display() {
        if (!timeinit) {
            timeinit = 1;
            time_prev = clock() / float(CLOCKS_PER_SEC);
        }
        do {
            display_main();
            if (lock) {
                float time_now;
                double t = 0;
                do {
                    time_now = clock() / float(CLOCKS_PER_SEC);
                    t = time_now - time_prev;
                } while (t < max_t);
                if (t > max_t)
                    t = max_t;
                gb->step(t);
                time_prev = time_now;
            }
        } while (lock);
    }

    void motion(int x, int y) {
        if (lock)
            return;
        float px = float(x) / float(window_width) * GameBoard::windowx;
        float py = (1.0 - float(y) / float(window_height)) * GameBoard::windowy;
        mouseinit = 1;
        mousex = px;
        mousey = py;
        gb->shooter_angle = atan((GameBoard::shooterx - px) / (GameBoard::shootery - py)) / M_PI * 180.0;
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
        if (lock || !mouseinit || button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
            return;
        lock = 1;
        time_prev = clock() / float(CLOCKS_PER_SEC);
    }

    // showWindow - 显示图形化界面
    // 注意，一个程序只能显示一个界面
    void showWindow(GameBoard &board) {
        gb = &board;
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

        glutMainLoop();
    }
}  // namespace Viewer