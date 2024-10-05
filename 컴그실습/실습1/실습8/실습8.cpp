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
bool a = true;//������ �׸���
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

std::vector<glm::vec3> vertexColor = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	//1
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	//2
	glm::vec3(1.0f, 0.3f, 0.3f),
	glm::vec3(1.0f, 0.3f, 0.3f),
	glm::vec3(1.0f, 0.3f, 0.3f),
	//3
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),
	//4
	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),
};

std::vector<glm::vec3> vertexPosition = {
	glm::vec3(1.0f,0.0f,0.0f),
	glm::vec3(-1.0f,0.0f,0.0f),
	glm::vec3(0.0f,1.0f,0.0f),
	glm::vec3(0.0f,-1.0f,0.0f),
	//1
	glm::vec3(-0.5f,0.6f,0.0f),
	glm::vec3(-0.4f,0.4f,0.0f),
	glm::vec3(-0.6f,0.4f,0.0f),
	//2
	glm::vec3(0.5f,0.6f,0.0f),
	glm::vec3(0.4f,0.4f,0.0f),
	glm::vec3(0.6f,0.4f,0.0f),
	//3
	glm::vec3(-0.5f,-0.4f,0.0f),
	glm::vec3(-0.4f,-0.6f,0.0f),
	glm::vec3(-0.6f,-0.6f,0.0f),
	//4
	glm::vec3(0.5f,-0.4f,0.0f),
	glm::vec3(0.4f,-0.6f,0.0f),
	glm::vec3(0.6f,-0.6f,0.0f),
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

bool valid[4][3];
int rightNum = 0;

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID); // ���̴� ���α׷� Ȱ��ȭ
	glBindVertexArray(VAO);  // VAO ���ε�

	glUseProgram(shaderProgramID);

	if(a){
		glDrawArrays(GL_LINES, 0, 2);
		glDrawArrays(GL_LINES, 2, 2);
		glDrawArrays(GL_TRIANGLES, 4, 3);
		glDrawArrays(GL_TRIANGLES, 7, 3);
		glDrawArrays(GL_TRIANGLES, 10, 3);
		glDrawArrays(GL_TRIANGLES, 13, 3);

		for (int i = 0; i < rightNum; ++i) {
			glDrawArrays(GL_TRIANGLES, 16 + 3 * i, 3);
		}
	}
	else {
		glDrawArrays(GL_LINES, 0, 2);
		glDrawArrays(GL_LINES, 2, 2);
		glDrawArrays(GL_LINE_LOOP, 4, 3);
		glDrawArrays(GL_LINE_LOOP, 7, 3);
		glDrawArrays(GL_LINE_LOOP, 10, 3);
		glDrawArrays(GL_LINE_LOOP, 13, 3);

		for (int i = 0; i < rightNum; ++i) {
			glDrawArrays(GL_LINE_LOOP, 16 + 3 * i, 3);
		}
	}

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
	glutInitWindowSize(800, 600); //--- �������� ũ�� ����
	glutCreateWindow("¯�̵���^^"); //--- ������ ����(������ �̸�)

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgramID = make_shaderProgram();

	InitBuffer(); // ���� �ʱ�ȭ

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 3; ++j)
			valid[i][j] = false;
	}

	glutDisplayFunc(drawScene); //--- ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); //--- �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard); //--- Ű���� �Է� �ݹ��Լ� ����
	glutTimerFunc(100, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	glutMouseFunc(Mouse);
	glutMainLoop(); //--- �̺�Ʈ ó�� ����

}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		a = true;
		break;
	case 'b':
		a = false;
		break;
	}
	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� ����
}

void Mouse(int button, int state, int x, int y) {
	GLfloat X, Y;
	ScreenToOpenGL(x, y, X, Y); // ��ǥ ��ȯ
	GLfloat r;
	GLfloat g;
	GLfloat b;
	glm::vec3 color;
	GLfloat size = (rand() % 20+1) / 100.0f;
	if (button == GLUT_LEFT_BUTTON) {	
		if (state == GLUT_DOWN) {
			if (X < 0 && Y>0) {
				vertexPosition[4] = { X,Y + size,0.0f };
				vertexPosition[5] = { X - size,Y - size,0.0f };
				vertexPosition[6] = { X + size,Y - size,0.0f };
				r = (rand() % 10) / 10.0f;
				g = (rand() % 10) / 10.0f;
				b = (rand() % 10) / 10.0f;
				vertexColor[4] = {r,g,b};
				vertexColor[5] = { r,g,b };
				vertexColor[6] = { r,g,b };
			}
			else if (X > 0 && Y > 0) {
				vertexPosition[7] = { X,Y + size,0.0f };
				vertexPosition[8] = { X - size,Y - size,0.0f };
				vertexPosition[9] = { X + size,Y - size,0.0f };
				r = (rand() % 10) / 10.0f;
				g = (rand() % 10) / 10.0f;
				b = (rand() % 10) / 10.0f;
				vertexColor[7] = { r,g,b };
				vertexColor[8] = { r,g,b };
				vertexColor[9] = { r,g,b };
			}
			else if (X < 0 && Y < 0) {
				vertexPosition[10] = { X,Y + size,0.0f };
				vertexPosition[11] = { X - size,Y - size,0.0f };
				vertexPosition[12] = { X + size,Y - size,0.0f };
				r = (rand() % 10) / 10.0f;
				g = (rand() % 10) / 10.0f;
				b = (rand() % 10) / 10.0f;
				vertexColor[10] = { r,g,b };
				vertexColor[11] = { r,g,b };
				vertexColor[12] = { r,g,b };
			}
			else if (X > 0 && Y < 0) {
				vertexPosition[13] = { X,Y + size,0.0f };
				vertexPosition[14] = { X - size,Y - size,0.0f };
				vertexPosition[15] = { X + size,Y - size,0.0f };
				r = (rand() % 10) / 10.0f;
				g = (rand() % 10) / 10.0f;
				b = (rand() % 10) / 10.0f;
				vertexColor[13] = { r,g,b };
				vertexColor[14] = { r,g,b };
				vertexColor[15] = { r,g,b };
			}
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		glm::vec3 vertex1;
		glm::vec3 vertex2;
		glm::vec3 vertex3;
		if (state == GLUT_DOWN) {
			if (X < 0 && Y>0) {
				for (int i = 0; i < 3; ++i) {
					if (!valid[0][i]) {
						vertex1 = { X,Y + size,0.0f };
						vertexPosition.push_back(vertex1);
						vertex2 = { X - size,Y - size,0.0f };
						vertexPosition.push_back(vertex2);
						vertex3 = { X + size,Y - size,0.0f };
						vertexPosition.push_back(vertex3);
						r = (rand() % 10) / 10.0f;
						g = (rand() % 10) / 10.0f;
						b = (rand() % 10) / 10.0f;
						color = {r,g,b};
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						valid[0][i] = true;
						++rightNum;
						break;
					}
				}
			}
			else if (X > 0 && Y > 0) {
				for (int i = 0; i < 3; ++i) {
					if (!valid[1][i]) {
						vertex1 = { X,Y + size,0.0f };
						vertexPosition.push_back(vertex1);
						vertex2 = { X - size,Y - size,0.0f };
						vertexPosition.push_back(vertex2);
						vertex3 = { X + size,Y - size,0.0f };
						vertexPosition.push_back(vertex3);
						r = (rand() % 10) / 10.0f;
						g = (rand() % 10) / 10.0f;
						b = (rand() % 10) / 10.0f;
						color = { r,g,b };
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						valid[1][i] = true;
						++rightNum;
						break;
					}
				}
			}
			else if (X < 0 && Y < 0) {
				for (int i = 0; i < 3; ++i) {
					if (!valid[2][i]) {
						vertex1 = { X,Y + size,0.0f };
						vertexPosition.push_back(vertex1);
						vertex2 = { X - size,Y - size,0.0f };
						vertexPosition.push_back(vertex2);
						vertex3 = { X + size,Y - size,0.0f };
						vertexPosition.push_back(vertex3);
						r = (rand() % 10) / 10.0f;
						g = (rand() % 10) / 10.0f;
						b = (rand() % 10) / 10.0f;
						color = { r,g,b };
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						valid[2][i] = true;
						++rightNum;
						break;
					}
				}
			}
			else if (X > 0 && Y < 0) {
				for (int i = 0; i < 3; ++i) {
					if (!valid[3][i]) {
						vertex1 = { X,Y + size,0.0f };
						vertexPosition.push_back(vertex1);
						vertex2 = { X - size,Y - size,0.0f };
						vertexPosition.push_back(vertex2);
						vertex3 = { X + size,Y - size,0.0f };
						vertexPosition.push_back(vertex3);
						r = (rand() % 10) / 10.0f;
						g = (rand() % 10) / 10.0f;
						b = (rand() % 10) / 10.0f;
						color = { r,g,b };
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						valid[3][i] = true;
						++rightNum;
						break;
					}
				}
			}
		}
	}
	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� ����
}

void TimerFunction(int value)
{
	//glutTimerFunc(100, TimerFunction, 1); // Ÿ�̸� �Լ� ȣ��
}