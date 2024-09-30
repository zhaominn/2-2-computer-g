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

bool isDrag = false; //���콺 �巡�� ������ Ȯ��
int now = -1; //���� Ŭ���� �簢�� ����

// ��ũ�� ��ǥ�� OpenGL ��ǥ�� ��ȯ�ϴ� �Լ�
void ScreenToOpenGL(int x, int y, GLfloat& X, GLfloat& Y) {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int windowWidth = viewport[2];
	int windowHeight = viewport[3];

	// X ��ǥ ��ȯ: 0 ~ windowWidth -> -1.0 ~ 1.0
	X = (static_cast<float>(x) / static_cast<float>(windowWidth)) * 2.0f - 1.0f;

	// Y ��ǥ ��ȯ: 0 ~ windowHeight -> 1.0 ~ -1.0 (OpenGL�� Y���� ������ +1)
	Y = 1.0f - (static_cast<float>(y) / static_cast<float>(windowHeight)) * 2.0f;
}

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv); //--- glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- ���÷��� ��� ����
	glutInitWindowPosition(0, 0); //--- �������� ��ġ ����
	glutInitWindowSize(800, 800); //--- �������� ũ�� ����
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
	glutTimerFunc(100, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMainLoop(); //--- �̺�Ʈ ó�� ����

}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //--- �������� ����
	glClear(GL_COLOR_BUFFER_BIT); //--- ������ ������ ��ü�� ĥ�ϱ�

	//-------------------

	for (int i = 0; i < 10; ++i) {
		glColor3f(Rectangles[i].r, Rectangles[i].g, Rectangles[i].b);
		glRectf(Rectangles[i].startX, Rectangles[i].startY, Rectangles[i].endX, Rectangles[i].endY);
	}

	//-------------------
	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
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
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {

		GLfloat X, Y;
		ScreenToOpenGL(x, y, X, Y); // ��ǥ ��ȯ

		if (state == GLUT_DOWN) {
			// ���콺 Ŭ�� ����
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
			// ���콺�� ������ �� ��ǥ ����
			isDrag = false;
			for (int i = 0; i < 10; ++i) {
				if (i != now) {
					if ((Rectangles[i].startX > X) && (Rectangles[i].endX < X)
						&& (Rectangles[i].startY > Y) && (Rectangles[i].endY < Y)) {

					}
				}
			}


			now = -1;// now �ʱ�ȭ
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y) {
	//���콺�������̴����϶�
	if (isDrag) {
		GLfloat X, Y;
		ScreenToOpenGL(x, y, X, Y); // ��ǥ ��ȯ

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