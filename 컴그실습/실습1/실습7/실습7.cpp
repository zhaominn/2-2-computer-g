#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <gl/glew.h>
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <vector>
#include <random>

GLvoid Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);

//----------------------------------
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);

GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü

int rememberShape[10]; //0�� 1�� 2�ﰢ�� 3�簢��
int now = 0;
int shapeIndex = -1;

int selectindex = -1;
int moveindex = -1;
int movenum = 0;
int moveShapenum;

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

GLuint VAO, VBO;

std::vector<float> vertexData;

void makeVertexData(int numVertices, GLfloat x, GLfloat y) {
	float r, g, b;
	switch (numVertices) {
	case 0:
		vertexData.push_back(x);
		vertexData.push_back(y);
		vertexData.push_back(0);
		r = (rand() % 10) / 10.0f;
		g = (rand() % 10) / 10.0f;
		b = (rand() % 10) / 10.0f;
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);
		break;
	case 1:
		//����
		vertexData.push_back(x - 0.1);
		vertexData.push_back(y);
		vertexData.push_back(0);
		r = (rand() % 10) / 10.0f;
		g = (rand() % 10) / 10.0f;
		b = (rand() % 10) / 10.0f;
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);

		vertexData.push_back(x + 0.1);
		vertexData.push_back(y);
		vertexData.push_back(0);
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);
		break;
	case 2:
		//����
		vertexData.push_back(x);
		vertexData.push_back(y);
		vertexData.push_back(0);
		r = (rand() % 10) / 10.0f;
		g = (rand() % 10) / 10.0f;
		b = (rand() % 10) / 10.0f;
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);

		vertexData.push_back(x - 0.1);
		vertexData.push_back(y - 0.1);
		vertexData.push_back(0);
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);

		vertexData.push_back(x + 0.1);
		vertexData.push_back(y - 0.1);
		vertexData.push_back(0);
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);
		break;
	case 3:
		vertexData.push_back(x - 0.1);
		vertexData.push_back(y);
		vertexData.push_back(0);
		r = (rand() % 10) / 10.0f;
		g = (rand() % 10) / 10.0f;
		b = (rand() % 10) / 10.0f;
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);

		vertexData.push_back(x - 0.1);
		vertexData.push_back(y - 0.2);
		vertexData.push_back(0);
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);

		vertexData.push_back(x + 0.1);
		vertexData.push_back(y);
		vertexData.push_back(0);
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);

		vertexData.push_back(x + 0.1);
		vertexData.push_back(y - 0.2);
		vertexData.push_back(0);
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);
		break;
	}
}

void InitBuffer()
{
	//--- VAO ��ü ���� �� ���ε�
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//--- vertex data ������ ���� VBO ���� �� ���ε�.
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//--- vertex data ������ �Է�.
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

	//--- ��ġ �Ӽ�: �Ӽ� ��ġ 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//--- ���� �Ӽ�: �Ӽ� ��ġ 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // ������ ������� ����
	glClear(GL_COLOR_BUFFER_BIT); // ȭ���� ����

	glUseProgram(shaderProgramID); // ���̴� ���α׷� Ȱ��ȭ
	glBindVertexArray(VAO);  // VAO ���ε�

	int vertexOffset = 0;
	for (int i = 0; i < now; ++i) {
		switch (rememberShape[i]) {
		case 0: // ��
			glPointSize(5.0); // �� ũ�� ����
			glDrawArrays(GL_POINTS, vertexOffset, 1);
			vertexOffset += 1;
			break;
		case 1: // ��
			glDrawArrays(GL_LINES, vertexOffset, 2);
			vertexOffset += 2;
			break;
		case 2: // �ﰢ��
			glDrawArrays(GL_TRIANGLES, vertexOffset, 3);
			vertexOffset += 3;
			break;
		case 3: // �簢�� (�� ���� �ﰢ������ �׸���)
			glDrawArrays(GL_TRIANGLE_STRIP, vertexOffset, 4);
			vertexOffset += 4;
			break;
		default:
			break;
		}
	}
	glutSwapBuffers(); // ���� ���۸��� ����� ȭ���� ����
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
	glutInitWindowPosition(100, 100); //--- �������� ��ġ ����
	glutInitWindowSize(800, 600); //--- �������� ũ�� ����
	glutCreateWindow("¯�����¯¯¯¯¯�̵ǰ�ʹ�"); //--- ������ ����(������ �̸�)

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgramID = make_shaderProgram();


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
	case 'p': // ��
		shapeIndex = 0;
		break;
	case 'l':
		shapeIndex = 1;
		break;
	case 't':
		shapeIndex = 2;
		break;
	case 'r':
		shapeIndex = 3;
		break;
	case 'w':
		selectindex = rand() % now;
		moveindex = 0;
		movenum = 0;
		moveShapenum = 0;
		for (int i = 0; i < selectindex; ++i) {
			moveShapenum += 6 * (rememberShape[i] + 1);
		}
		break;
	case 'a':
		selectindex = rand() % now;
		moveindex = 1;
		movenum = 0;
		moveShapenum = 0;
		for (int i = 0; i < selectindex; ++i) {
			moveShapenum += 6 * (rememberShape[i] + 1);
		}
		break;
	case 's':
		selectindex = rand() % now;
		moveindex = 2;
		movenum = 0;
		moveShapenum = 0;
		for (int i = 0; i < selectindex; ++i) {
			moveShapenum += 6 * (rememberShape[i] + 1);
		}
		break;
	case 'd':
		selectindex = rand() % now;
		moveindex = 3;
		movenum = 0;
		moveShapenum = 0;
		for (int i = 0; i < selectindex; ++i) {
			moveShapenum += 6 * (rememberShape[i] + 1);
		}
		break;
	case 'c':
		vertexData.clear();
		now = 0;
		break;
	}
	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� refresh
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			GLfloat X, Y;
			ScreenToOpenGL(x, y, X, Y); // ��ǥ ��ȯ	

			makeVertexData(shapeIndex, X, Y);
			if (now != 10) {
				rememberShape[now] = shapeIndex; // ��
				++now;
			}
		}
	}
	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� ����
}

void TimerFunction(int value)
{
	switch (moveindex) {
	case -1: break;
	case 0: // ���� �̵�
		for (int i = 0; i < (rememberShape[selectindex] + 1); ++i) {
			vertexData[moveShapenum + 1 + 6 * i] += 0.01f; // Y�� ��ǥ�� ������Ŵ
		}
		++movenum;
		if (movenum == 10)
			moveindex = -1;
		break;
	case 1: // �������� �̵�
		for (int i = 0; i < (rememberShape[selectindex] + 1); ++i) {
			vertexData[moveShapenum + 0 + 6 * i] -= 0.01f; // X�� ��ǥ�� ���ҽ�Ŵ
		}
		++movenum;
		if (movenum == 10)
			moveindex = -1;
		break;
	case 2: // �Ʒ��� �̵�
		for (int i = 0; i < (rememberShape[selectindex] + 1); ++i) {
			vertexData[moveShapenum + 1 + 6 * i] -= 0.01f; // Y�� ��ǥ�� ���ҽ�Ŵ
		}
		++movenum;
		if (movenum == 10)
			moveindex = -1;
		break;
	case 3: // ���������� �̵�
		for (int i = 0; i < (rememberShape[selectindex] + 1); ++i) {
			vertexData[moveShapenum + 0 + 6 * i] += 0.01f; // X�� ��ǥ�� ������Ŵ
		}
		++movenum;
		if (movenum == 10)
			moveindex = -1;
		break;
	}

	InitBuffer(); // ���� �ʱ�ȭ
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}