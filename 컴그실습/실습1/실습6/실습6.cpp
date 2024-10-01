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
struct rectangle {
	GLfloat X1;
	GLfloat Y1;
	GLfloat X2;
	GLfloat Y2;
	GLfloat X3;
	GLfloat Y3;
	GLfloat X4;
	GLfloat Y4;
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat size;
	int mode;
};

rectangle Rectangles[10];

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

	//--------------------
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<> dis(-8.0, 8.0);

	for (int i = 0; i < 10; ++i) {
		GLfloat x = round(dis(gen) * 10) / 100.0;
		GLfloat y = round(dis(gen) * 10) / 100.0;
		GLfloat r = (rand() % 10) / 10.0f;
		GLfloat g = (rand() % 10) / 10.0f;
		GLfloat b = (rand() % 10) / 10.0f;
		Rectangles[i] = { x,y,x + 0.1f,y,x,y - 0.1f,x + 0.1f,y - 0.1f,r,g,b ,0.1f,0 };
	}
	//--------------------

	glutDisplayFunc(drawScene); //--- ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); //--- �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard); //--- Ű���� �Է� �ݹ��Լ� ����
	glutTimerFunc(100, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	glutMouseFunc(Mouse);
	glutMainLoop(); //--- �̺�Ʈ ó�� ����

}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //--- �������� ����
	glClear(GL_COLOR_BUFFER_BIT); //--- ������ ������ ��ü�� ĥ�ϱ�

	for (int i = 0; i < 10; ++i) {
		switch (Rectangles[i].mode) {
		case -1: break;
		case 0:
			glColor3f(Rectangles[i].r, Rectangles[i].g, Rectangles[i].b);
			glRectf(Rectangles[i].X1, Rectangles[i].Y1, Rectangles[i].X1 + 0.2f, Rectangles[i].Y1 - 0.2f);
			break;
		case 4:
			glColor3f(Rectangles[i].r, Rectangles[i].g, Rectangles[i].b);
			glRectf(Rectangles[i].X3, Rectangles[i].Y1, Rectangles[i].X3 + Rectangles[i].size, Rectangles[i].Y1 - Rectangles[i].size);
			glRectf(Rectangles[i].X1, Rectangles[i].Y1, Rectangles[i].X1 + Rectangles[i].size, Rectangles[i].Y1 - Rectangles[i].size);
			glRectf(Rectangles[i].X2, Rectangles[i].Y1, Rectangles[i].X2 + Rectangles[i].size, Rectangles[i].Y1 - Rectangles[i].size);
			glRectf(Rectangles[i].X2, Rectangles[i].Y2, Rectangles[i].X2 + Rectangles[i].size, Rectangles[i].Y2 - Rectangles[i].size);
			glRectf(Rectangles[i].X2, Rectangles[i].Y4, Rectangles[i].X2 + Rectangles[i].size, Rectangles[i].Y4 - Rectangles[i].size);
			glRectf(Rectangles[i].X3, Rectangles[i].Y3, Rectangles[i].X3 + Rectangles[i].size, Rectangles[i].Y3 - Rectangles[i].size);
			glRectf(Rectangles[i].X3, Rectangles[i].Y4, Rectangles[i].X3 + Rectangles[i].size, Rectangles[i].Y4 - Rectangles[i].size);
			glRectf(Rectangles[i].X4, Rectangles[i].Y4, Rectangles[i].X4 + Rectangles[i].size, Rectangles[i].Y4 - Rectangles[i].size);
			break;
		default:
			glColor3f(Rectangles[i].r, Rectangles[i].g, Rectangles[i].b);
			glRectf(Rectangles[i].X1, Rectangles[i].Y1, Rectangles[i].X1 +Rectangles[i].size, Rectangles[i].Y1 -Rectangles[i].size);
			glRectf(Rectangles[i].X2, Rectangles[i].Y2, Rectangles[i].X2 + Rectangles[i].size, Rectangles[i].Y2 - Rectangles[i].size);
			glRectf(Rectangles[i].X3, Rectangles[i].Y3, Rectangles[i].X3 + Rectangles[i].size, Rectangles[i].Y3 - Rectangles[i].size);
			glRectf(Rectangles[i].X4, Rectangles[i].Y4, Rectangles[i].X4 + Rectangles[i].size, Rectangles[i].Y4 - Rectangles[i].size);
			break;
		}
	}

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

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			GLfloat X, Y;
			ScreenToOpenGL(x, y, X, Y); // ��ǥ ��ȯ
			// ���콺 Ŭ�� ����
			for (int i = 0; i < 10; ++i) {
				if (Rectangles[i].mode == 0 && (X < Rectangles[i].X1 + 0.2f) && (Rectangles[i].X1 < X)
					&& (Y < Rectangles[i].Y1) && (Rectangles[i].Y1 - 0.2f < Y)) {

					Rectangles[i].mode = rand() % 4 + 1;
					//Rectangles[i].mode = 4;
				}
			}

		}
	}
	glutPostRedisplay();
}

void TimerFunction(int value)
{
	for (int i = 0; i < 10; ++i) {
		switch (Rectangles[i].mode) {
		case -1: break;
		case 0: break;
		case 2:
			Rectangles[i].X1 -= 0.02f;
			Rectangles[i].Y1 += 0.02f;
			Rectangles[i].X2 += 0.02f;
			Rectangles[i].Y2 += 0.02f;
			Rectangles[i].X3 -= 0.02f;
			Rectangles[i].Y3 -= 0.02f;
			Rectangles[i].X4 += 0.02f;
			Rectangles[i].Y4 -= 0.02f;
			Rectangles[i].size -= 0.01f;
			if (Rectangles[i].size <= 0.01f)
				Rectangles[i].mode = -1;
			break;
		case 3:
			Rectangles[i].X1 -= 0.02f;
			Rectangles[i].X2 -= 0.02f;
			Rectangles[i].X3 -= 0.02f;
			Rectangles[i].X4 -= 0.02f;
			Rectangles[i].size -= 0.01f;
			if (Rectangles[i].size <= 0.01f)
				Rectangles[i].mode = -1;
			break;
		default:
			Rectangles[i].Y1 += 0.02f;
			Rectangles[i].X2 += 0.02f;
			Rectangles[i].X3 -= 0.02f;
			Rectangles[i].Y4 -= 0.02f;
			Rectangles[i].size -= 0.01f;
			if (Rectangles[i].size <= 0.01f)
				Rectangles[i].mode = -1;
			break;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}