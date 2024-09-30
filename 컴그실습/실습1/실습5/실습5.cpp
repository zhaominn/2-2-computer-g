#include <iostream>
#include <gl/glew.h>
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);

#include <random>
struct rectangle {
	GLfloat X;
	GLfloat Y;
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat size;
};

rectangle Rectangles[30];
rectangle eraser = { -2,-2, 0.0f,0.0f,0.0f,0.1f };
bool isDrag = false;

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

	//--------------------
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<> dis(-9.0, 9.0);

	for (int i = 0; i < 30; ++i) {
		GLfloat x = round(dis(gen) * 10) / 100.0;
		GLfloat y = round(dis(gen) * 10) / 100.0;
		GLfloat r = (rand() % 10) / 10.0f;
		GLfloat g = (rand() % 10) / 10.0f;
		GLfloat b = (rand() % 10) / 10.0f;
		Rectangles[i] = { x,y,r,g,b ,1 };
	}
	//--------------------

	glutDisplayFunc(drawScene); //--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); //--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutTimerFunc(100, TimerFunction, 1); // 타이머 함수 설정
	glutMainLoop(); //--- 이벤트 처리 시작

}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT); //--- 설정된 색으로 전체를 칠하기

	for (int i = 0; i < 30; ++i) {
		if (Rectangles[i].size > 0) {
			glColor3f(Rectangles[i].r, Rectangles[i].g, Rectangles[i].b);
			glRectf(Rectangles[i].X, Rectangles[i].Y, Rectangles[i].X + 0.1f, Rectangles[i].Y + 0.1f);
		}
	}
	if (isDrag) {
		glColor3f(eraser.r, eraser.g, eraser.b);
		glRectf(eraser.X - eraser.size, eraser.Y - eraser.size, eraser.X + eraser.size, eraser.Y + eraser.size);
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
	case 'r':
		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_real_distribution<> dis(-9.0, 9.0);

		for (int i = 0; i < 30; ++i) {
			GLfloat x = round(dis(gen) * 10) / 100.0;
			GLfloat y = round(dis(gen) * 10) / 100.0;
			GLfloat r = (rand() % 10) / 10.0f;
			GLfloat g = (rand() % 10) / 10.0f;
			GLfloat b = (rand() % 10) / 10.0f;
			Rectangles[i] = { x,y,r,g,b,1 };
		}
		break;

	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			// 마우스 클릭 시작
			isDrag = true;

		}
		else if (state == GLUT_UP && isDrag) {
			// 마우스를 떼었을 때 좌표 저장
			isDrag = false;
			eraser = { -2,-2, 0.0f,0.0f,0.0f,0.1f };
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y) {
	if (isDrag) {
		GLfloat X, Y;
		ScreenToOpenGL(x, y, X, Y); // 좌표 변환
		eraser.X = X;
		eraser.Y = Y;
		for (int i = 0; i < 30; ++i) {
			if ((Rectangles[i].size > 0) && (Rectangles[i].X < eraser.X + eraser.size) && (Rectangles[i].Y < eraser.Y + eraser.size)
				&& (Rectangles[i].X + 0.1f > eraser.X - eraser.size) && (Rectangles[i].Y + 0.1f > eraser.Y - eraser.size)) {
				eraser.size += 0.025f;
				eraser.r = Rectangles[i].r;
				eraser.g = Rectangles[i].g;
				eraser.b = Rectangles[i].b;
				Rectangles[i].size = -1;
			}
		}
	}
}

void TimerFunction(int value)
{

	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}