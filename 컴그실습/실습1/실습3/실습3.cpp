#include <iostream>
#include <gl/glew.h>
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);

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
		glRectf(Rectangles[i].startX,Rectangles[i].startY,Rectangles[i].endX,Rectangles[i].endY);
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
			Rectangles[i] = {x,y,x+0.1f,y+0.1f,r,g,b };
		}
		break;
	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

void TimerFunction(int value)
{

	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}