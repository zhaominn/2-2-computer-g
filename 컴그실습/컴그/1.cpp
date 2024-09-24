#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

/*
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
*/

GLvoid drawScene(GLvoid);
GLvoid Reshape
(int w, int h);
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
{ //--- ������ �����ϱ�
glutInit(&argc, argv); // glut �ʱ�ȭ
glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
glutInitWindowPosition(100, 100); // �������� ��ġ ����
glutInitWindowSize(250, 250); // �������� ũ�� ����
glutCreateWindow("Example1"); // ������ ����(������ �̸�)
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
{
	std::cerr << "Unable to initialize GLEW" << std::endl;
	exit(EXIT_FAILURE);
}
else
std::cout << "GLEW Initialized\n";
glutDisplayFunc(drawScene); // ��� �Լ��� ����
glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
glutMainLoop(); // �̺�Ʈ ó�� ����
}
GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // �������� ��blue���� ����
glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.
glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
glViewport(0, 0, w, h);
}