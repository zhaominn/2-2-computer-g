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
int mode = 0;
struct Shape {
	int shape;
	int mode;
	int moveNum;
};
Shape shapes[4]{
	{1,0,0},
	{2,0,0},
	{3,0,0},
	{4,0,0}
};
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
	//��
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	//1
	glm::vec3(0.2f,0.2f,1.0f),
	glm::vec3(0.2f,0.2f,1.0f),
	glm::vec3(0.2f,0.2f,1.0f),
	//2
	glm::vec3(1.0f,1.0f,0.3f),
	glm::vec3(1.0f,1.0f,0.3f),
	glm::vec3(1.0f,1.0f,0.3f),
	glm::vec3(1.0f,1.0f,0.3f),
	//3
	glm::vec3(0.4f,1.0f,0.4f),
	glm::vec3(0.4f,1.0f,0.4f),
	glm::vec3(0.4f,1.0f,0.4f),
	glm::vec3(0.4f,1.0f,0.4f),
	glm::vec3(0.4f,1.0f,0.4f),
	//4
	glm::vec3(1.0f,0.5f,0.5f),
	glm::vec3(1.0f,0.5f,0.5f),
	glm::vec3(1.0f,0.5f,0.5f),
	glm::vec3(1.0f,0.5f,0.5f),
	glm::vec3(1.0f,0.5f,0.5f),
	//1
	glm::vec3(0.2f,0.2f,1.0f),
	glm::vec3(0.2f,0.2f,1.0f),
	glm::vec3(0.2f,0.2f,1.0f),
	//2
	glm::vec3(1.0f,1.0f,0.3f),
	glm::vec3(1.0f,1.0f,0.3f),
	glm::vec3(1.0f,1.0f,0.3f),
	glm::vec3(1.0f,1.0f,0.3f),
	//3
	glm::vec3(0.4f,1.0f,0.4f),
	glm::vec3(0.4f,1.0f,0.4f),
	glm::vec3(0.4f,1.0f,0.4f),
	glm::vec3(0.4f,1.0f,0.4f),
	glm::vec3(0.4f,1.0f,0.4f),
	//4
	glm::vec3(1.0f,0.5f,0.5f),
	glm::vec3(1.0f,0.5f,0.5f),
	glm::vec3(1.0f,0.5f,0.5f),
	glm::vec3(1.0f,0.5f,0.5f),
	glm::vec3(1.0f,0.5f,0.5f),
};

std::vector<glm::vec3> vertexPosition = {
	glm::vec3(1.0f,0.0f,0.0f),
	glm::vec3(-1.0f,0.0f,0.0f),
	glm::vec3(0.0f,1.0f,0.0f),
	glm::vec3(0.0f,-1.0f,0.0f),
	//1
	glm::vec3(-0.5f, 0.8f, 0.0f),
	glm::vec3(-0.8f, 0.2f, 0.0f),
	glm::vec3(-0.2f, 0.2f, 0.0f),
	//2 �ﰢ�� 2��
	glm::vec3(0.2f,0.8f,0.0f),
	glm::vec3(0.8f,0.8f,0.0f),
	glm::vec3(0.2f,0.2f,0.0f),
	glm::vec3(0.8f,0.2f,0.0f),
	//3 �ﰢ�� 3��
	glm::vec3(-0.8f,-0.2f,0.0f),
	glm::vec3(-0.8f,-0.8f,0.0f),
	glm::vec3(-0.5f,-0.2f,0.0f),
	glm::vec3(-0.2,-0.8f,0.0f),
	glm::vec3(-0.2,-0.2f,0.0f),
	//4 �ﰢ�� 3��
	glm::vec3(0.2f,-0.45f,0.0f),
	glm::vec3(0.3f,-0.8f,0.0f),
	glm::vec3(0.5f,-0.2f,0.0f),
	glm::vec3(0.7f,-0.8f,0.0f),
	glm::vec3(0.8f,-0.45f,0.0f),

	//1
	glm::vec3(0.0f, 0.6f, 0.0f),
	glm::vec3(-0.6f, -0.6f, 0.0f),
	glm::vec3(0.6f, -0.6f, 0.0f),
	//2 �ﰢ�� 2��
	glm::vec3(-0.8f,0.8f,0.0f),
	glm::vec3(0.8f,0.8f,0.0f),
	glm::vec3(-0.8f,-0.8f,0.0f),
	glm::vec3(0.8f,-0.8f,0.0f),
	//3 �ﰢ�� 3��
	glm::vec3(-0.6f,0.6f,0.0f),
	glm::vec3(-0.6f,-0.6f,0.0f),
	glm::vec3(0.0f,0.6f,0.0f),
	glm::vec3(0.6,-0.6f,0.0f),
	glm::vec3(0.6,0.6f,0.0f),
	//4 �ﰢ�� 3��
	glm::vec3(-0.6f,0.1f,0.0f),
	glm::vec3(-0.4f,-0.6f,0.0f),
	glm::vec3(0.0f,0.6f,0.0f),
	glm::vec3(0.4f,-0.6f,0.0f),
	glm::vec3(0.6f,0.1f,0.0f),
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
	glClearColor(1.0f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID); // ���̴� ���α׷� Ȱ��ȭ
	glBindVertexArray(VAO);  // VAO ���ε�

	switch (mode) {
	case 0:
		//��
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		glDrawArrays(GL_LINE_STRIP, 2, 2);
		//1
		glDrawArrays(GL_TRIANGLES, 4, 3);
		//2
		glDrawArrays(GL_TRIANGLES, 7, 3);
		glDrawArrays(GL_TRIANGLES, 8, 3);
		//3
		glDrawArrays(GL_TRIANGLES, 11, 3);
		glDrawArrays(GL_TRIANGLES, 12, 3);
		glDrawArrays(GL_TRIANGLES, 13, 3);
		//4
		glDrawArrays(GL_TRIANGLES, 16, 3);
		glDrawArrays(GL_TRIANGLES, 17, 3);
		glDrawArrays(GL_TRIANGLES, 18, 3);
		break;
	case 1:
		glDrawArrays(GL_TRIANGLES, 21, 3);
		break;
	case 2:
		glDrawArrays(GL_TRIANGLES, 24, 3);
		glDrawArrays(GL_TRIANGLES, 25, 3);
		break;
	case 3:
		glDrawArrays(GL_TRIANGLES, 28, 3);
		glDrawArrays(GL_TRIANGLES, 29, 3);
		glDrawArrays(GL_TRIANGLES, 30, 3);
		break;
	case 4:
		glDrawArrays(GL_TRIANGLES, 33, 3);
		glDrawArrays(GL_TRIANGLES, 34, 3);
		glDrawArrays(GL_TRIANGLES, 35, 3);
		break;
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
	case 'l':
		mode = 1;
		for (int i = 0; i < 4; ++i) {
			shapes[i].moveNum = 0;
			shapes[i].mode = 0;
		}
		break;
	case't':
		mode = 2;
		for (int i = 0; i < 4; ++i) {
			shapes[i].moveNum = 0;
			shapes[i].mode = 0;
		}
		break;
	case 'r':
		mode = 3;
		for (int i = 0; i < 4; ++i) {
			shapes[i].moveNum = 0;
			shapes[i].mode = 0;
		}
		break;
	case 'p':
		mode = 4;
		for (int i = 0; i < 4; ++i) {
			shapes[i].moveNum = 0;
			shapes[i].mode = 0;
		}
		break;
	case 'a':
		mode = 0;
		for (int i = 0; i < 4; ++i) {
			shapes[i].moveNum = 0;
			shapes[i].mode = 0;
		}
		break;
	}
	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� ����
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

	}
	InitBuffer();
	glutPostRedisplay();
}


void TimerFunction(int value)
{
	switch (mode) {
	case 0:
		for (int i = 0; i < 4; ++i) {
			switch (shapes[i].shape) {
			case 1:
				if (shapes[i].mode == 0) {
					vertexPosition[4].x += 0.01f;
					vertexPosition[6].y += 0.02f;
					++shapes[i].moveNum;
					if (shapes[i].moveNum == 30) {
						shapes[i].moveNum = 0;
						shapes[i].mode = 1;
					}
				}
				else {
					vertexPosition[4].x -= 0.01f;
					vertexPosition[6].y -= 0.02f;
					++shapes[i].moveNum;
					if (shapes[i].moveNum == 30) {
						shapes[i].moveNum = 0;
						shapes[i].mode = 0;
					}
				}
				break;
			case 2:
				if (shapes[i].mode == 0) {
					vertexPosition[7].x += 0.01f;
					vertexPosition[8].x -= 0.01f;
					++shapes[i].moveNum;
					if (shapes[i].moveNum == 30) {
						shapes[i].moveNum = 0;
						shapes[i].mode = 1;
					}
				}
				else {
					vertexPosition[7].x -= 0.01f;
					vertexPosition[8].x += 0.01f;
					++shapes[i].moveNum;
					if (shapes[i].moveNum == 30) {
						shapes[i].moveNum = 0;
						shapes[i].mode = 0;
					}
				}
				break;
			case 3:
				if (shapes[i].mode == 0) {
					vertexPosition[11].y -= 0.01f;
					vertexPosition[15].y -= 0.01f;
					vertexPosition[12].x += 0.004f;
					vertexPosition[14].x -= 0.004f;
					++shapes[i].moveNum;
					if (shapes[i].moveNum == 25) {
						shapes[i].moveNum = 0;
						shapes[i].mode = 1;
					}
				}
				else {
					vertexPosition[11].y += 0.01f;
					vertexPosition[15].y += 0.01f;
					vertexPosition[12].x -= 0.004f;
					vertexPosition[14].x += 0.004f;
					++shapes[i].moveNum;
					if (shapes[i].moveNum == 25) {
						shapes[i].moveNum = 0;
						shapes[i].mode = 0;
					}
				}
				break;
			case 4:
				if (shapes[i].mode == 0) {
					vertexPosition[16].y += 0.01f;
					vertexPosition[20].y += 0.01f;
					vertexPosition[16].x += 0.024f;
					vertexPosition[17].x -= 0.004f;
					vertexPosition[18].x += 0.012f;
					vertexPosition[19].x -= 0.02f;
					++shapes[i].moveNum;
					if (shapes[i].moveNum == 25) {
						shapes[i].moveNum = 0;
						shapes[i].mode = 1;
					}
				}
				else {
					vertexPosition[16].y -= 0.01f;
					vertexPosition[20].y -= 0.01f;
					vertexPosition[16].x -= 0.024f;
					vertexPosition[17].x += 0.004f;
					vertexPosition[18].x -= 0.012f;
					vertexPosition[19].x += 0.02f;
					++shapes[i].moveNum;
					if (shapes[i].moveNum == 25) {
						shapes[i].moveNum = 0;
						shapes[i].mode = 0;
					}
				}
				break;
			}
		}
		break;
	case 1:
		if (shapes[0].mode == 0) {
			vertexPosition[21].x += 0.02f;
			vertexPosition[23].y += 0.04f;
			++shapes[0].moveNum;
			if (shapes[0].moveNum == 30) {
				shapes[0].moveNum = 0;
				shapes[0].mode = 1;
			}
		}
		else {
			vertexPosition[21].x -= 0.02f;
			vertexPosition[23].y -= 0.04f;
			++shapes[0].moveNum;
			if (shapes[0].moveNum == 30) {
				shapes[0].moveNum = 0;
				shapes[0].mode = 0;
			}
		}
		break;
	case 2:
		if (shapes[1].mode == 0) {
			vertexPosition[24].x += 0.02f;
			vertexPosition[25].x -= 0.02f;
			++shapes[1].moveNum;
			if (shapes[1].moveNum == 40) {
				shapes[1].moveNum = 0;
				shapes[1].mode = 1;
			}
		}
		else {
			vertexPosition[24].x -= 0.02f;
			vertexPosition[25].x += 0.02f;
			++shapes[1].moveNum;
			if (shapes[1].moveNum == 40) {
				shapes[1].moveNum = 0;
				shapes[1].mode = 0;
			}
		}
		break;
	case 3:
		if (shapes[2].mode == 0) {
			vertexPosition[28].y -= 0.02f;
			vertexPosition[32].y -= 0.02f;
			vertexPosition[29].x += 0.008f;
			vertexPosition[31].x -= 0.008f;
			++shapes[2].moveNum;
			if (shapes[2].moveNum == 25) {
				shapes[2].moveNum = 0;
				shapes[2].mode = 1;
			}
		}
		else {
			vertexPosition[28].y += 0.02f;
			vertexPosition[32].y += 0.02f;
			vertexPosition[29].x -= 0.008f;
			vertexPosition[31].x += 0.008f;
			++shapes[2].moveNum;
			if (shapes[2].moveNum == 25) {
				shapes[2].moveNum = 0;
				shapes[2].mode = 0;
			}
		}
		break;
	case 4:
		if (shapes[3].mode == 0) {
			vertexPosition[33].y += 0.02f;
			vertexPosition[37].y += 0.02f;
			vertexPosition[33].x += 0.048f;
			vertexPosition[34].x -= 0.008f;
			vertexPosition[35].x += 0.024f;
			vertexPosition[36].x -= 0.04f;
			++shapes[3].moveNum;
			if (shapes[3].moveNum == 25) {
				shapes[3].moveNum = 0;
				shapes[3].mode = 1;
			}
		}
		else {
			vertexPosition[33].y -= 0.02f;
			vertexPosition[37].y -= 0.02f;
			vertexPosition[33].x -= 0.048f;
			vertexPosition[34].x += 0.008f;
			vertexPosition[35].x -= 0.024f;
			vertexPosition[36].x += 0.04f;
			++shapes[3].moveNum;
			if (shapes[3].moveNum == 25) {
				shapes[3].moveNum = 0;
				shapes[3].mode = 0;
			}
		}
		break;
	}
	InitBuffer();  // ���� �ʱ�ȭ
	glutPostRedisplay();
	glutTimerFunc(60, TimerFunction, 1);
}