#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <gl/glew.h>
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
GLvoid Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);


#include <vector>
#include <string>
bool isDrag = true;
//----------------------------------
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);

GLuint shaderProgram; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü
//-----------------------------------
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

void make_vertexShaders()
{
	GLchar* vertexSource;
	//--- ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
	//--- filetobuf: ��������� �Լ��� �ؽ�Ʈ�� �о ���ڿ��� �����ϴ� �Լ�
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentSource;
	//--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
	fragmentSource = filetobuf("fragment.glsl"); // �����׼��̴� �о����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

GLuint make_shaderProgram()
{
	GLchar errorLog[512];
	GLint result;

	GLuint shaderID;
	shaderID = glCreateProgram(); //--- ���̴� ���α׷� �����

	glAttachShader(shaderID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(shaderID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�

	glLinkProgram(shaderID); //--- ���̴� ���α׷� ��ũ�ϱ�
	glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����

	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program ���� ����\n" << errorLog << std::endl;
		return false;
	}

	glUseProgram(shaderID); //--- ������� ���̴� ���α׷� ����ϱ�
	//--- ���� ���� ���̴����α׷� ���� �� �ְ�, �� �� �Ѱ��� ���α׷��� ����Ϸ���
	//--- glUseProgram �Լ��� ȣ���Ͽ� ��� �� Ư�� ���α׷��� �����Ѵ�.
	//--- ����ϱ� ������ ȣ���� �� �ִ�.
	return shaderID;
}

std::vector<glm::vec3> vertexColor = {
	//��������
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),

	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),

	glm::vec3(1.0f, 0.2f, 0.2f),
	glm::vec3(1.0f, 0.2f, 0.2f),
	glm::vec3(1.0f, 0.2f, 0.2f),
	glm::vec3(1.0f, 0.2f, 0.2f),

	glm::vec3(1.0f, 0.8f, 0.5f),
	glm::vec3(1.0f, 0.8f, 0.5f),
	glm::vec3(1.0f, 0.8f, 0.5f),
	glm::vec3(1.0f, 0.8f, 0.5f),

	glm::vec3(1.0f, 0.2f, 0.8f),
	glm::vec3(1.0f, 0.2f, 0.8f),
	glm::vec3(1.0f, 0.2f, 0.8f),
	glm::vec3(1.0f, 0.2f, 0.8f),

	glm::vec3(1.0f, 0.5f, 0.8f),
	glm::vec3(1.0f, 0.5f, 0.8f),
	glm::vec3(1.0f, 0.5f, 0.8f),
	glm::vec3(1.0f, 0.5f, 0.8f),
	//�����ü
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),

	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),

	glm::vec3(1.0f, 0.2f, 0.2f),
	glm::vec3(1.0f, 0.2f, 0.2f),
	glm::vec3(1.0f, 0.2f, 0.2f),

	glm::vec3(1.0f, 0.8f, 0.5f),
	glm::vec3(1.0f, 0.8f, 0.5f),
	glm::vec3(1.0f, 0.8f, 0.5f),
};

std::vector<glm::vec3> vertexPosition = {
	//������ü
	glm::vec3(0.5f,0.5f,0.5f),
	glm::vec3(0.5f,-0.5f,0.5f),
	glm::vec3(-0.5f,0.5f,0.5f),
	glm::vec3(-0.5f,-0.5f,0.5f),

	glm::vec3(0.5f,0.5f,0.5f),
	glm::vec3(0.5f,0.5f,-0.5f),
	glm::vec3(-0.5f,0.5f,0.5f),
	glm::vec3(-0.5f,0.5f,-0.5f),

	glm::vec3(-0.5f,-0.5f,0.5f),
	glm::vec3(-0.5f,-0.5f,-0.5f),
	glm::vec3(-0.5f,0.5f,0.5f),
	glm::vec3(-0.5f,0.5f,-0.5f),

	glm::vec3(0.5f,-0.5f,0.5f),
	glm::vec3(0.5f,-0.5f,-0.5f),
	glm::vec3(0.5f,0.5f,0.5f),
	glm::vec3(0.5f,0.5f,-0.5f),

	glm::vec3(0.5f,-0.5f,0.5f),
	glm::vec3(0.5f,-0.5f,-0.5f),
	glm::vec3(-0.5f,-0.5f,0.5f),
	glm::vec3(-0.5f,-0.5f,-0.5f),

	glm::vec3(0.5f,0.5f,-0.5f),
	glm::vec3(0.5f,-0.5f,-0.5f),
	glm::vec3(-0.5f,0.5f,-0.5f),
	glm::vec3(-0.5f,-0.5f,-0.5f),
	//�����ü
	glm::vec3(0.5f,-0.5f,0.0f),
	glm::vec3(-0.5f,-0.5f,-0.5f),
	glm::vec3(0.0f,0.5f,0.0f),

	glm::vec3(0.5f,-0.5f,0.0f),
	glm::vec3(-0.5f,-0.5f,-0.5f),
	glm::vec3(-0.5f,-0.5f,0.5f),

	glm::vec3(-0.5f,-0.5f,0.5f),
	glm::vec3(-0.5f,-0.5f,-0.5f),
	glm::vec3(0.0f,+0.5f,0.0f),

	glm::vec3(-0.5f,-0.5f,0.5f),
	glm::vec3(0.5f,-0.5f,0.0f),
	glm::vec3(0.0f,+0.5f,0.0f),

};

GLuint VAO, VBO_position, VBO_color;
//-----------------------------------


void InitBuffer()
{
	//--- Vertex Array Object ����
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//--- ��ġ �Ӽ�
	glGenBuffers(1, &VBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glBufferData(GL_ARRAY_BUFFER, vertexPosition.size() * sizeof(glm::vec3), glm::value_ptr(vertexPosition[0]), GL_STATIC_DRAW);

	//--- ���� �Ӽ�
	glGenBuffers(1, &VBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(glm::vec3), glm::value_ptr(vertexColor[0]), GL_STATIC_DRAW);

	//--- vPos �Ӽ� ������ ���� ����
	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(pAttribute);

	//--- vColor �Ӽ� ������ ���� ����
	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);
}


GLUquadricObj* qobj;
GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	glUseProgram(shaderProgram);
	glm::mat4 Ry = glm::mat4(1.0f); //--- zȸ�� ��� ����
	glm::mat4 Rx = glm::mat4(1.0f); //--- xȸ�� ��� ����
	glm::mat4 RR = glm::mat4(1.0f); //--- �ռ� ��ȯ ���
	Ry = glm::rotate(Ry, glm::radians(10.0f), glm::vec3(0.0, 1.0, 0.0)); //--- y�࿡ ���Ͽ� ȸ�� ���
	Rx = glm::rotate(Rx, glm::radians(10.0f), glm::vec3(1.0, 0.0, 0.0)); //--- x�࿡ ���Ͽ� ȸ�� ���
	RR = Rx*Ry;
	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(RR)); //--- modelTransform ������ ��ȯ �� �����ϱ�


	//---------- ���� �׸���
	glBindVertexArray(VAO);
	/*// ������ü
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawArrays(GL_TRIANGLES, 1, 3);

	glDrawArrays(GL_TRIANGLES, 4, 3);
	glDrawArrays(GL_TRIANGLES, 5, 3);

	glDrawArrays(GL_TRIANGLES, 8, 3);
	glDrawArrays(GL_TRIANGLES, 9, 3);

	glDrawArrays(GL_TRIANGLES, 12, 3);
	glDrawArrays(GL_TRIANGLES, 13, 3);

	glDrawArrays(GL_TRIANGLES, 16, 3);
	glDrawArrays(GL_TRIANGLES, 17, 3);

	glDrawArrays(GL_TRIANGLES, 20, 3);
	glDrawArrays(GL_TRIANGLES, 21, 3);
	*/
	//�����ü
	
	glDrawArrays(GL_TRIANGLES, 24, 3);

	glDrawArrays(GL_TRIANGLES, 27, 3);

	glDrawArrays(GL_TRIANGLES, 30, 3);

	glDrawArrays(GL_TRIANGLES, 33, 3);
	
	glutSwapBuffers();
}

//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}
// ��ũ�� ��ǥ�� OpenGL ��ǥ�� ��ȯ�ϴ� �Լ�
void ScreenToOpenGL(int x, int y, GLfloat& X, GLfloat& Y) {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int windowWidth = viewport[2];
	int windowHeight = viewport[3];

	X = (static_cast<float>(x) / static_cast<float>(windowWidth)) * 2.0f - 1.0f;
	Y = 1.0f - (static_cast<float>(y) / static_cast<float>(windowHeight)) * 2.0f;
}

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv); //--- glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- ���÷��� ��� ����
	glutInitWindowPosition(100, 100); //--- �������� ��ġ ����
	glutInitWindowSize(800, 800); //--- �������� ũ�� ����
	glutCreateWindow("¯�̵���^^"); //--- ������ ����(������ �̸�)

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgram = make_shaderProgram();

	InitBuffer(); // ���� �ʱ�ȭ



	glutDisplayFunc(drawScene); //--- ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); //--- �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard); //--- Ű���� �Է� �ݹ��Լ� ����
	glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMainLoop(); //--- �̺�Ʈ ó�� ����

}

GLvoid Keyboard(unsigned char key, int x, int y)
{

	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� ����
}

void Mouse(int button, int state, int x, int y) {
	GLfloat X, Y;
	ScreenToOpenGL(x, y, X, Y); // ��ǥ ��ȯ

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			isDrag = true;

		}
		else if (state == GLUT_UP && isDrag) {
			isDrag = false;

		}


	}
	InitBuffer(); // ���� �ʱ�ȭ
	glutPostRedisplay(); // ȭ�� ����
}

void Motion(int x, int y) {
	//���콺�������̴����϶�
	if (isDrag) {
		GLfloat X, Y;
		ScreenToOpenGL(x, y, X, Y); // ��ǥ ��ȯ
	}
	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� ����
}

void TimerFunction(int value)
{
	InitBuffer(); // ���� �ʱ�ȭ
	glutPostRedisplay(); // ȭ�� ����

	//glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸� �Լ� ȣ��
}