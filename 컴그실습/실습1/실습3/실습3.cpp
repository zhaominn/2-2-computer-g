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
#include <cmath>
#include <algorithm>

struct rectangle {
	GLfloat startX;
	GLfloat startY;
	GLfloat endX;
	GLfloat endY;
	GLfloat r;
	GLfloat g;
	GLfloat b;
};

rectangle Rectangles[10]{
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}
};

bool isDrag = false; //마우스 드래그 중인지 확인
int now = -1; //현재 클릭한 사각형 저장

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
	glutMotionFunc(Motion);
	glutMainLoop(); //--- 이벤트 처리 시작

}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT); //--- 설정된 색으로 전체를 칠하기

	//-------------------

	for (int i = 0; i < 10; ++i) {
		glColor3f(Rectangles[i].r, Rectangles[i].g, Rectangles[i].b);
		glRectf(Rectangles[i].startX, Rectangles[i].startY, Rectangles[i].endX, Rectangles[i].endY);
	}

	//-------------------
	glutSwapBuffers(); //--- 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_real_distribution<> dis(-9.0, 9.0);
		for (int i = 0; i < 10; ++i) {
			GLfloat x = round(dis(gen) * 10) / 100.0;
			GLfloat y = round(dis(gen) * 10) / 100.0;
			GLfloat r = (rand() % 10) / 10.0f;
			GLfloat g = (rand() % 10) / 10.0f;
			GLfloat b = (rand() % 10) / 10.0f;
			Rectangles[i] = { x,y,x + 0.1f,y + 0.1f,r,g,b };
		}
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {

		GLfloat X, Y;
		ScreenToOpenGL(x, y, X, Y); // 좌표 변환

		if (state == GLUT_DOWN) {
			// 마우스 클릭 시작
			isDrag = true;
			for (int i = 0; i < 10; ++i) {
				if ((Rectangles[i].startX < X) && (Rectangles[i].endX > X)
					&& (Rectangles[i].startY < Y) && (Rectangles[i].endY > Y)) {
					now = i;
					break;
				}
			}
		}
		else if (state == GLUT_UP && isDrag) {
			// 마우스를 떼었을 때 좌표 저장
			isDrag = false;
			for (int i = 0; i < 10; ++i) {
				if (i != now) {
					//if ((Rectangles[i].startX < Rectangles[now].startX) && (Rectangles[i].endX > Rectangles[now].endX)
					//	&& (Rectangles[i].startY < Rectangles[now].startY) && (Rectangles[i].endY > Rectangles[now].endY)) 
					if ((Rectangles[i].startX < Rectangles[now].endX) && (Rectangles[i].startY < Rectangles[now].endY) && (Rectangles[i].endX > Rectangles[now].startX) &&
						(Rectangles[i].endY > Rectangles[now].startY)) {

						Rectangles[i].startX = std::min(Rectangles[i].startX, Rectangles[now].startX);
						Rectangles[i].endX = std::max(Rectangles[i].endX, Rectangles[now].endX);
						Rectangles[i].startY = std::min(Rectangles[i].startY, Rectangles[now].startY);
						Rectangles[i].endY = std::max(Rectangles[i].endY, Rectangles[now].endY);

						Rectangles[i].r = (rand() % 10) / 10.0f;
						Rectangles[i].g = (rand() % 10) / 10.0f;
						Rectangles[i].b = (rand() % 10) / 10.0f;

						Rectangles[now] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
					}
				}
			}


			now = -1;// now 초기화
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y) {
	//마우스가움직이는중일때
	if (isDrag) {
		GLfloat X, Y;
		ScreenToOpenGL(x, y, X, Y); // 좌표 변환

		GLfloat width = (Rectangles[now].endX - Rectangles[now].startX) / 2.0f;
		GLfloat height = (Rectangles[now].endY - Rectangles[now].startY) / 2.0f;

		Rectangles[now].startX = X - width;
		Rectangles[now].endX = X + width;
		Rectangles[now].startY = Y - height;
		Rectangles[now].endY = Y + height;
	}

	glutPostRedisplay();
}

void TimerFunction(int value)
{
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}