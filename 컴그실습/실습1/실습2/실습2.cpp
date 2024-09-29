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

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv); //--- glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- ���÷��� ��� ����
	glutInitWindowPosition(0, 0); //--- �������� ��ġ ����
	glutInitWindowSize(800, 600); //--- �������� ũ�� ����
	glutCreateWindow("Example1"); //--- ������ ����(������ �̸�)
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //--- glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";



	glutDisplayFunc(drawScene); //--- ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); //--- �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard); //--- Ű���� �Է� �ݹ��Լ� ����
	glutMouseFunc(Mouse);
	glutMainLoop(); //--- �̺�Ʈ ó�� ����

}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(Rectangles[4].r, Rectangles[4].g, Rectangles[4].b, 1.0f); //--- �������� ����
	glClear(GL_COLOR_BUFFER_BIT); //--- ������ ������ ��ü�� ĥ�ϱ�


	//--------------------------
	glColor3f(Rectangles[0].r, Rectangles[0].g, Rectangles[0].b);
	glRectf(-1.0f + (Rectangles[0].size / 10.0f), 1.0f - (Rectangles[0].size / 10.0f), 0.0f - (Rectangles[0].size / 10.0f), 0.0f + (Rectangles[0].size / 10.0f));
	glColor3f(Rectangles[1].r, Rectangles[1].g, Rectangles[1].b);
	glRectf(0.0f + (Rectangles[1].size / 10.0f), 1.0f - (Rectangles[1].size / 10.0f), 1.0f - (Rectangles[1].size / 10.0f), 0.0f + (Rectangles[1].size / 10.0f));
	glColor3f(Rectangles[2].r, Rectangles[2].g, Rectangles[2].b);
	glRectf(-1.0f + (Rectangles[2].size / 10.0f), 0.0f - (Rectangles[2].size / 10.0f), 0.0f - (Rectangles[2].size / 10.0f), -1.0f + (Rectangles[2].size / 10.0f));
	glColor3f(Rectangles[3].r, Rectangles[3].g, Rectangles[3].b);
	glRectf(0.0f + (Rectangles[3].size / 10.0f), 0.0f - (Rectangles[3].size / 10.0f), 1.0f - (Rectangles[3].size / 10.0f), -1.0f + (Rectangles[3].size / 10.0f));

	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {


	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
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

	if (button == GLUT_LEFT_BUTTON) { //���� �ٲٱ�
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
	else if (button == GLUT_RIGHT_BUTTON) { //ũ�� �ٲٱ�
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
