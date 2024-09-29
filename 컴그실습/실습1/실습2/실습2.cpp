#include <iostream>
#include <gl/glew.h>
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);

struct colorSet {
	GLfloat r;
	GLfloat g;
	GLfloat b;
	int size;
};

colorSet background = { 1.0f, 0.5f, 0.5f, 0 };
colorSet Rectangles[5] = {
	{1.0, 0.2, 0.2,0},
	{1.0, 0.4, 0.4,0},
	{1.0, 0.6, 0.6,0},
	{1.0, 0.8, 0.8,0},
	{1.0, 0.5, 0.5, 1.0} };

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); //--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); //--- 윈도우의 위치 지정
	glutInitWindowSize(800, 600); //--- 윈도우의 크기 지정
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
	glutMouseFunc(Mouse);
	glutMainLoop(); //--- 이벤트 처리 시작

}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(Rectangles[4].r, Rectangles[4].g, Rectangles[4].b, 1.0f); //--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT); //--- 설정된 색으로 전체를 칠하기


	//--------------------------
	glColor3f(Rectangles[0].r, Rectangles[0].g, Rectangles[0].b);
	glRectf(-1.0f + (Rectangles[0].size / 10.0f), 1.0f - (Rectangles[0].size / 10.0f), 0.0f - (Rectangles[0].size / 10.0f), 0.0f + (Rectangles[0].size / 10.0f));
	glColor3f(Rectangles[1].r, Rectangles[1].g, Rectangles[1].b);
	glRectf(0.0f + (Rectangles[1].size / 10.0f), 1.0f - (Rectangles[1].size / 10.0f), 1.0f - (Rectangles[1].size / 10.0f), 0.0f + (Rectangles[1].size / 10.0f));
	glColor3f(Rectangles[2].r, Rectangles[2].g, Rectangles[2].b);
	glRectf(-1.0f + (Rectangles[2].size / 10.0f), 0.0f - (Rectangles[2].size / 10.0f), 0.0f - (Rectangles[2].size / 10.0f), -1.0f + (Rectangles[2].size / 10.0f));
	glColor3f(Rectangles[3].r, Rectangles[3].g, Rectangles[3].b);
	glRectf(0.0f + (Rectangles[3].size / 10.0f), 0.0f - (Rectangles[3].size / 10.0f), 1.0f - (Rectangles[3].size / 10.0f), -1.0f + (Rectangles[3].size / 10.0f));

	glutSwapBuffers(); //--- 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {


	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

void Mouse(int button, int state, int x, int y)
{
	GLfloat X, Y;
	if (x <= 400)
		X = -(x / 400.0f);
	else
		X = -((400.0f - x) / 400.0f);

	if (y <= 300)
		Y = y / 300.0f;
	else
		Y = (300.0f - y) / 300.0f;

	if (button == GLUT_LEFT_BUTTON) { //색상 바꾸기
		if ((0.0f - (Rectangles[0].size / 10.0f) > X) && (-1.0f + (Rectangles[0].size / 10.0f) < X) //0
			&& (1.0f - (Rectangles[0].size / 10.0f) > Y) && (0.0f + (Rectangles[0].size / 10.0f) < Y)) {
			Rectangles[0].r = (rand() % 10) / 10.0f;
			Rectangles[0].g = (rand() % 10) / 10.0f;
			Rectangles[0].b = (rand() % 10) / 10.0f;
		}
		else if ((1.0f + (Rectangles[1].size / 10.0f) > X) && (0.0f - (Rectangles[1].size / 10.0f) < X) // 1
			&& (1.0f - (Rectangles[1].size / 10.0f) > Y) && (0.0f + (Rectangles[1].size / 10.0f) < Y)) {
			Rectangles[1].r = (rand() % 10) / 10.0f;
			Rectangles[1].g = (rand() % 10) / 10.0f;
			Rectangles[1].b = (rand() % 10) / 10.0f;
		}
		else if ((0.0f + (Rectangles[2].size / 10.0f) > X) && (-1.0f - (Rectangles[2].size / 10.0f) < X) //2
			&& (-1.0f - (Rectangles[2].size / 10.0f) < Y) && (0.0f + (Rectangles[2].size / 10.0f) > Y)) {
			Rectangles[2].r = (rand() % 10) / 10.0f;
			Rectangles[2].g = (rand() % 10) / 10.0f;
			Rectangles[2].b = (rand() % 10) / 10.0f;
		}
		else if ((1.0f + (Rectangles[3].size / 10.0f) > X) && (0.0f - (Rectangles[3].size / 10.0f) < X) //3
			&& (-1.0f - (Rectangles[3].size / 10.0f) < Y) && (0.0f + (Rectangles[3].size / 10.0f) > Y)) {
			Rectangles[3].r = (rand() % 10) / 10.0f;
			Rectangles[3].g = (rand() % 10) / 10.0f;
			Rectangles[3].b = (rand() % 10) / 10.0f;
		}
		else {
			Rectangles[4].r = (rand() % 10) / 10.0f;
			Rectangles[4].g = (rand() % 10) / 10.0f;
			Rectangles[4].b = (rand() % 10) / 10.0f;
		}

	}
	else if (button == GLUT_RIGHT_BUTTON) { //크기 바꾸기
		if ((0.0f > X) && (-1.0f < X)		//0
			&& (1.0f > Y) && (0.0f < Y)) {
			if ((0.0f - (Rectangles[0].size / 10.0f) > X) && (-1.0f + (Rectangles[0].size / 10.0f) < X)
				&& (1.0f - (Rectangles[0].size / 10.0f) > Y) && (0.0f + (Rectangles[0].size / 10.0f) < Y)) {

				if (Rectangles[0].size < 10)
					Rectangles[0].size++;
			}
			else {
				Rectangles[0].size--;
			}
		}
		else if ((1.0f > X) && (0.0f < X)	//1
			&& (1.0f > Y) && (0.0f < Y)) { //1
			if ((1.0f + (Rectangles[1].size / 10.0f) > X) && (0.0f - (Rectangles[1].size / 10.0f) < X)
				&& (1.0f - (Rectangles[1].size / 10.0f) > Y) && (0.0f + (Rectangles[1].size / 10.0f) < Y)) {

				if (Rectangles[1].size < 10)
					Rectangles[1].size++;
			}
			else {
				Rectangles[1].size--;
			}
		}
		else if ((0.0f > X) && (-1.0f < X)	//2
			&& (-1.0f < Y) && (0.0f > Y)) {
			if ((0.0f + (Rectangles[2].size / 10.0f) > X) && (-1.0f - (Rectangles[2].size / 10.0f) < X)
				&& (-1.0f - (Rectangles[2].size / 10.0f) < Y) && (0.0f + (Rectangles[2].size / 10.0f) > Y)) {
				if (Rectangles[2].size < 10)
					Rectangles[2].size++;
			}
			else {
				Rectangles[2].size--;
			}
		}
		else if ((1.0f > X) && (0.0f < X)	//3
			&& (-1.0f < Y) && (0.0f > Y)) {
			if ((1.0f + (Rectangles[3].size / 10.0f) > X) && (0.0f - (Rectangles[3].size / 10.0f) < X)
				&& (-1.0f - (Rectangles[3].size / 10.0f) < Y) && (0.0f + (Rectangles[3].size / 10.0f) > Y)) {
				if (Rectangles[3].size < 10)
					Rectangles[3].size++;
			}
			else {
				Rectangles[3].size--;
			}
		}
	}

	glutPostRedisplay();
}
