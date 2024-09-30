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
	glutMainLoop(); //--- �̺�Ʈ ó�� ����

}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f); //--- �������� ����
	glClear(GL_COLOR_BUFFER_BIT); //--- ������ ������ ��ü�� ĥ�ϱ�

	for (int i = 0; i < 5; ++i) {
		glColor3f(Rectangles[i].r, Rectangles[i].g, Rectangles[i].b);
		glRectf(Rectangles[i].startX, Rectangles[i].startY, Rectangles[i].endX, Rectangles[i].endY);
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
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

void Mouse(int button, int state, int x, int y) {

	if (!m) {
		if (state == GLUT_DOWN) {
			GLfloat X, Y;
			ScreenToOpenGL(x, y, X, Y); // ��ǥ ��ȯ

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