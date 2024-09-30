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

	glutDisplayFunc(drawScene); //--- ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); //--- �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard); //--- Ű���� �Է� �ݹ��Լ� ����
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutTimerFunc(100, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	glutMainLoop(); //--- �̺�Ʈ ó�� ����

}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //--- �������� ����
	glClear(GL_COLOR_BUFFER_BIT); //--- ������ ������ ��ü�� ĥ�ϱ�

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

	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
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
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			// ���콺 Ŭ�� ����
			isDrag = true;

		}
		else if (state == GLUT_UP && isDrag) {
			// ���콺�� ������ �� ��ǥ ����
			isDrag = false;
			eraser = { -2,-2, 0.0f,0.0f,0.0f,0.1f };
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y) {
	if (isDrag) {
		GLfloat X, Y;
		ScreenToOpenGL(x, y, X, Y); // ��ǥ ��ȯ
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