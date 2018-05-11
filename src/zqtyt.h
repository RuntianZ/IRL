// ��ǿ��һ��ģ�����
#include "gl_env.h"

class GameBoard {

public:
    struct Block {
        static const float triangle_radius, circle_radius, square_radius;
        enum Type {
            TRIANGLE, CIRCLE, SQUARE
        } type;
        float centerx, centery, angle;
        // ���ڽǶ�angle��˵��
        // һ��ȡ��ʱ��Ϊ��
        // ����������Σ����Ϸ�������center��һ��ֱ����Ϊ0��
        // ����������Σ��ı���������ƽ��ʱΪ0��
    };

    struct Ball {
        float centerx, centery, vx, vy;
    };

    static const float windowx, windowy;
    static const float shooterx, shootery;
    static const float box[8];
    float shooter_angle;              // �������ĽǶ�(ƫ�봹ֱ�ߣ���ʱ��Ϊ��)
    std::vector<Block> blocks;        // ����ש��
    std::vector<Ball> balls;          // ����С��
    float ax, ay;                     // ���ٶȳ�

    GameBoard() {
        shooter_angle = 10.0;

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
// ǽ��λ��(��һ�����ӱ�ʾ���ֱ�������Ͻǡ����½ǡ����Ͻǡ����½�)
const float GameBoard::box[8] = {
    5.0, 150.0, 5.0, 15.0, 85.0, 15.0, 85.0, 150.0
};
// ש��İ뾶(���ĵ㵽����ľ���)
const float GameBoard::Block::triangle_radius = 6.0;
const float GameBoard::Block::square_radius = 6.0;
const float GameBoard::Block::circle_radius = 6.0;

// ======================================================

// ���ӻ���Ϸ����
namespace Viewer {
    void init(int *argc, char **argv) {
        glutInit(argc, argv);
    }

    float time_prev;
    GameBoard *gb;

    void drawString(const char* str) {
        int len = strlen(str);
        for (int i = 0; i < len; i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
    }

    void drawCircle(float x, float y, float r) {
        const int num_of_segments = 200;
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= num_of_segments; i++)
        {
            glVertex2f(x + r * cos((2.0 * M_PI * i) / num_of_segments),
                y + r * sin((2.0 * M_PI * i) / num_of_segments));
        }
        glEnd();
    }

    void randColor() {
        float r = float(rand()) / float(RAND_MAX) * 0.4 + 0.45;
        float g = float(rand()) / float(RAND_MAX) * 0.4 + 0.45;
        float b = float(rand()) / float(RAND_MAX) * 0.4 + 0.45;
        glColor3f(r, g, b);
    }

    void display() {
        // Initialization
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(0.0, 0.4, 0.2);

        // 1. Draw walls
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

        // 2. Draw shooter
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

        // 3. Draw blocks
        for each (GameBoard::Block b in gb->blocks) {
            randColor();
            switch (b.type) {
            case GameBoard::Block::Type::TRIANGLE:
                glBegin(GL_TRIANGLES);
                glVertex2f(b.centerx + sin(b.angle / 180.0 * M_PI), 
                           b.centery + cos(b.angle / 180.0 * M_PI));
                glVertex2f(b.centerx + sin(b.angle / 180.0 * M_PI),
                           b.centery + cos(b.angle / 180.0 * M_PI));
                glEnd();
                break;
            case GameBoard::Block::Type::SQUARE:
                glBegin(GL_TRIANGLE_STRIP);

                glEnd();
                break;
            case GameBoard::Block::Type::CIRCLE:
                drawCircle(b.centerx, b.centery, b.circle_radius);
            }
        }

        glColor3f(1.0, 0.0, 0.0);
        glRasterPos2f(45.0, 140.0);
        drawString("Hello");

        // Termination
        glFlush();
    }

    // showWindow - ��ʾͼ�λ�����
    // ע�⣬һ������ֻ����ʾһ������
    void showWindow(GameBoard &board) {
        glutInitWindowPosition(100, 20);
        glutInitWindowSize(360, 640);
        glutCreateWindow("Tan Yi Tan!");
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

        glClearColor(1.0, 1.0, 1.0, 0.0);
        glMatrixMode(GL_PROJECTION);
        gluOrtho2D(0.0, board.windowx, 0.0, board.windowy);
        gb = &board;

        glutDisplayFunc(display);
        glutMainLoop();
    }
}  // namespace Viewer