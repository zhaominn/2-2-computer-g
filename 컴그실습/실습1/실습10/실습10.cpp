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

#include <vector>
int mode = -1;
bool line = false;
bool Draw = false;
int ShapeNum = 0;
struct spiral {
	GLfloat x;
	GLfloat y;
	int DotNum;
};
spiral spirals[5];
GLfloat r = 1.0f;
GLfloat g = 0.8f;
GLfloat b = 0.8f;
//----------------------------------
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);

GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
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

std::vector<glm::vec3> vertexColor={
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f)
};

std::vector<glm::vec3> vertexPosition = {
	glm::vec3(1.0f,1.0f,0.0f),
	glm::vec3(1.0f,1.0f,0.0f),
};

GLuint VAO, VBO_position, VBO_color;

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
	GLint pAttribute = glGetAttribLocation(shaderProgramID, "vPos");
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(pAttribute);

	//--- vColor �Ӽ� ������ ���� ����
	GLint cAttribute = glGetAttribLocation(shaderProgramID, "vColor");
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID); // ���̴� ���α׷� Ȱ��ȭ
	glBindVertexArray(VAO);  // VAO ���ε�

	glUseProgram(shaderProgramID);

		if (line) {
			for (int i = 0; i < ShapeNum; ++i) {
				glDrawArrays(GL_LINE_STRIP, 4 + 220 * i, spirals[i].DotNum);
			}
		}
		else {
			glPointSize(2.0f); // �� ũ�⸦ 5�� ����
			for (int i = 0; i < ShapeNum; ++i) {
				glDrawArrays(GL_POINTS, 4 + 220 * i, spirals[i].DotNum);
			}
		}
	//----

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
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
	shaderProgramID = make_shaderProgram();

	InitBuffer(); // ���� �ʱ�ȭ

	//---

	//---

	glutDisplayFunc(drawScene); //--- ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); //--- �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard); //--- Ű���� �Է� �ݹ��Լ� ����
	glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	glutMouseFunc(Mouse);
	glutMainLoop(); //--- �̺�Ʈ ó�� ����

}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'p':
		line = false;
		break;
	case 'l':
		line = true;
		break;
	case '1':
		ShapeNum = 1;
		break;
	case '2':
		ShapeNum = 2;
		break;
	case '3':
		ShapeNum = 3;
		break;
	case '4':
		ShapeNum = 4;
		break;
	case '5':
		ShapeNum = 5;
		break;
	}
	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� ����
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && ShapeNum <= 5) {
		vertexPosition.clear();
		glm::vec3 tempPoint = { 0.0f, 0.0f, 0.0f };
		vertexPosition.push_back(tempPoint);
		tempPoint = { 0.0f, 0.0f, 0.0f };
		vertexPosition.push_back(tempPoint);

		vertexColor.clear();
		glm::vec3 tempColor = { 1.0f,1.0f,0.0f };
		vertexPosition.push_back(tempColor);
		tempColor = { 1.0f,1.0f,0.0f };
		vertexPosition.push_back(tempColor);

		GLfloat X, Y;
		ScreenToOpenGL(x, y, X, Y);

		const float pi = 3.14159265359f;
		GLfloat angle;
		int numPoints = 110;
		GLfloat radiusIncrement = 0.005f;

		GLfloat YY = Y;

		for (int j = 0; j < ShapeNum; ++j) {
			spirals[j].x = X;
			spirals[j].y = YY + (0.15f)*j;
			X = spirals[j].x;
			Y = spirals[j].y;
			spirals[j].DotNum = 0;

			for (int i = 0; i < numPoints; ++i) {
				angle = 10 * i * pi / 180.0f;
				GLfloat radius = i * radiusIncrement / 8;
				glm::vec3 tempPoint = { X + radius * cos(angle), Y + radius * sin(angle), 0.0f };
				vertexPosition.push_back(tempPoint);

				glm::vec3 tempColor = { 0.0f, 0.0f, 0.0f };
				vertexColor.push_back(tempColor);
			}
			for (int i = numPoints-1; i > 0; --i) {
				angle = 10 * (i + 88) * pi / 180.0f;
				GLfloat radius = i * radiusIncrement / 8;
				X = vertexPosition[4+220 * j].x + 2 * (vertexPosition[4 + numPoints - 1].x - vertexPosition[4].x);
				glm::vec3 tempPoint = { X + radius * cos(angle), Y + radius * sin(angle), 0.0f };
				vertexPosition.push_back(tempPoint);

				glm::vec3 tempColor = { 0.0f, 0.0f, 0.0f };
				vertexColor.push_back(tempColor);
			}
		}
		mode = 1;
		Draw = true;
		r = (rand() % 10) / 10.0f;
		g = (rand() % 10) / 10.0f;
		b = (rand() % 10) / 10.0f;
	}
	InitBuffer();
	glutPostRedisplay();
}


void TimerFunction(int value)
{
	if (Draw) {
		for (int i = 0; i < ShapeNum; ++i) {
			// vertexPosition ũ�� �ʰ��� ����
			int maxDots = std::min(static_cast<int>(vertexPosition.size()), 360);

			if (mode == 1) {
				if (spirals[i].DotNum <= maxDots) {
					spirals[i].DotNum++;
				}
			}
		}
	}

	InitBuffer();  // ���� �ʱ�ȭ
	glutPostRedisplay();
	glutTimerFunc(60, TimerFunction, 1);
}