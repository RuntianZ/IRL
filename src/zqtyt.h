// 最强弹一弹模拟程序
#include "gl_env.h"

class GameBoard {

public:
	struct Block {
		enum Type {
			TRIANGLE, CIRCLE, SQUARE
		};
		float centerx, centery, angle;
	};

	static const float windowx, windowy;
	static const float shooterx, shootery;
	static const float box[8];
	float shooter_angle;              // 发射器的角度(偏离垂直线，逆时针为正)
	std::vector<Block> blocks;        // 所有砖块

};  // class GameBoard

// ======================================================
// 参数调试区
// 整个屏幕的大小
const float GameBoard::windowx = 90.0;
const float GameBoard::windowy = 160.0;
// 发射口的位置
const float GameBoard::shooterx = 45.0;
const float GameBoard::shootery = 140.0;
// 墙的位置(用一个盒子表示，分别代表左上角、左下角、右上角、右下角)
const float GameBoard::box[8] = {
	5.0, 150.0, 5.0, 15.0, 85.0, 15.0, 85.0, 150.0
};

// ======================================================

// 可视化游戏界面
namespace Viewer {
	void init(int *argc, char **argv) {
		glutInit(argc, argv);
	}

	GameBoard *gb;

	void drawString(const char* str) {
		int len = strlen(str);
		for (int i = 0; i < len; i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
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
		gb->shooter_angle = 10.0;
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

		glColor3f(1.0, 0.0, 0.0);
		glRasterPos2f(45.0, 140.0);
		drawString("Hello");

		// Termination
		glFlush();
	}

	// showWindow - 显示图形化界面
	// 注意，一个程序只能显示一个界面
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