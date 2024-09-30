#include <iostream>
#include <gl/glew.h>
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);


#include <random>

bool one = false;
bool two = false;
bool three = false;
bool four = false;
bool s = true;
bool m = false;

struct rectangle {
	GLfloat startX;
	GLfloat startY;
	GLfloat endX;
	GLfloat endY;
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat dx;
	GLfloat dy;
};

rectangle Rectangles[5]{
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}
};

rectangle Rectangles_save[5]{
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}
};

// 스크린 좌표를 OpenGL 좌표로 변환하는 함수
void ScreenToOpenGL(int x, int y, GLfloat& X, GLfloat& Y) {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int windowWidth = viewport[2];
	int windowHeight = viewport[3];

	// X 좌표 변환: 0 ~ windowWidth -> -1.0 ~ 1.0
	X = (static_cast<float>(x) / static_cast<float>(windowWidth)) * 2.0f - 1.0f;

	// Y 좌표 변환: 0 ~ windowHeight -> 1.0 ~ -1.0 (OpenGL의 Y축은 위쪽이 +1)
	Y = 1.0f - (static_cast<float>(y) / static_cast<float>(windowHeight)) * 2.0f;
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); //--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); //--- 윈도우의 위치 지정
	glutInitWindowSize(800, 800); //--- 윈도우의 크기 지정
	glutCreateWindow("Example1"); //--- 윈도우 생성(윈도우 이름)
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //--- glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";



	glutDisplayFunc(drawScene); //--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); //--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutTimerFunc(100, TimerFunction, 1); // 타이머 함수 설정
	glutMouseFunc(Mouse);
	glutMainLoop(); //--- 이벤트 처리 시작

}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f); //--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT); //--- 설정된 색으로 전체를 칠하기

	for (int i = 0; i < 5; ++i) {
		glColor3f(Rectangles[i].r, Rectangles[i].g, Rectangles[i].b);
		glRectf(Rectangles[i].startX, Rectangles[i].startY, Rectangles[i].endX, Rectangles[i].endY);
	}

	glutSwapBuffers(); //--- 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1':
		one = !one;
		break;
	case '2':
		two = !two;
		break;
	case '3':
		three = !three;
		break;
	case '4':
		four = !four;
		break;
	case's':
		s = !s;
		break;
	case'm':
		for (int i = 0; i < 5; ++i) {
			Rectangles[i] = Rectangles_save[i];
		}
		break;
	case'r':
		m = false;
		break;
	case'q':
		exit(EXIT_FAILURE);
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

void Mouse(int button, int state, int x, int y) {

	if (!m) {
		if (state == GLUT_DOWN) {
			GLfloat X, Y;
			ScreenToOpenGL(x, y, X, Y); // 좌표 변환

			std::random_device rd;
			std::mt19937 gen(rd());

			std::uniform_real_distribution<> dis(-4.0, 4.0);
			for (int i = 0; i < 5; ++i) {
				GLfloat x = X + round(dis(gen) * 10) / 100.0;
				GLfloat y = Y + round(dis(gen) * 10) / 100.0;
				GLfloat r = (rand() % 10) / 10.0f;
				GLfloat g = (rand() % 10) / 10.0f;
				GLfloat b = (rand() % 10) / 10.0f;
				Rectangles[i] = { x,y,x + 0.1f,y + 0.1f,r,g,b,0.01f,0.01f };
				Rectangles_save[i] = Rectangles[i];
			}
		}
		else if (state == GLUT_UP) {
			glutPostRedisplay();
		}
		m = true;
	}
}

void TimerFunction(int value)
{
	if (s) {
		if (one) {
			for (int i = 0; i < 5; ++i) {
				if ((Rectangles[i].dx > 0 && Rectangles[i].endX >= 1)
					|| (Rectangles[i].dx < 0 && Rectangles[i].startX <= -1)) {
					Rectangles[i].dx *= -1;
				}
				else if ((Rectangles[i].dy < 0 && Rectangles[i].startY <= -1)
					|| (Rectangles[i].dy > 0 && Rectangles[i].endY >= 1)) {
					Rectangles[i].dy *= -1;
				}
				else {
					Rectangles[i].startX += Rectangles[i].dx;
					Rectangles[i].endX += Rectangles[i].dx;
					Rectangles[i].startY += Rectangles[i].dy;
					Rectangles[i].endY += Rectangles[i].dy;
				}
			}
		}
		else if (two) {
			for (int i = 0; i < 5; ++i) {
				if ((Rectangles[i].dx > 0 && Rectangles[i].endX >= 1)
					|| (Rectangles[i].dx < 0 && Rectangles[i].startX <= -1)) {
					Rectangles[i].dx *= -1;
				}
				else {
					Rectangles[i].startX += Rectangles[i].dx;
					Rectangles[i].endX += Rectangles[i].dx;
				}
			}
		}
		else if (three) {
			std::random_device rd;
			std::mt19937 gen(rd());

			std::uniform_real_distribution<> dis(-3.0, 3.0);
			for (int i = 0; i < 5; ++i) {
				GLfloat Dx = round(dis(gen)) / 100.0;
				GLfloat Dy = round(dis(gen)) / 100.0;
				Rectangles[i].startX -= Dx;
				Rectangles[i].endX += Dx;
				Rectangles[i].startY += Dy;
				Rectangles[i].endY -= Dy;
			}
		}
		else if (four) {
			for (int i = 0; i < 5; ++i) {
				Rectangles[i].r = (rand() % 10) / 10.0f;
				Rectangles[i].g = (rand() % 10) / 10.0f;
				Rectangles[i].b = (rand() % 10) / 10.0f;
			}
		}
	}
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}