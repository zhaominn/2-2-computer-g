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


struct D {
	GLfloat dx;
	GLfloat dy;
	int moveNum;
};

float Sign(float x1, float y1, float x2, float y2, float x3, float y3);
int mode[15] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }; //������
D move[15] = {
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
};
int shape[15] = { 1,1,1,2,2,2, 3, 3, 3, 4, 4, 4, 5, 5, 5 }; //������ ����
int select;
#include <vector>
#include <random>
bool isDrag = true;//������ �׸���
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
	glm::vec3(0.0f, 0.0f, 0.0f)
};

std::vector<glm::vec3> vertexPosition = {
	glm::vec3(1.0f,0.0f,0.0f),
	glm::vec3(1.0f,0.0f,0.0f)
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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID); // ���̴� ���α׷� Ȱ��ȭ
	glBindVertexArray(VAO);  // VAO ���ε�

	glDrawArrays(GL_LINES, 0, 2);

	for (int i = 0; i < 15; ++i) {
		switch (shape[i]) {
		case 1:
			glDrawArrays(GL_TRIANGLES, 2 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 3 + 6 * i, 3);
			break;
		case 2:
			glDrawArrays(GL_LINES, 2 + 6 * i, 2);
			break;
		case 3:
			glDrawArrays(GL_TRIANGLES, 2 + 6 * i, 3);
			break;
		case 4:
			glDrawArrays(GL_TRIANGLES, 2 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 3 + 6 * i, 3);
			break;
		case 5:
			glDrawArrays(GL_TRIANGLES, 2 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 3 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 4 + 6 * i, 3);
			break;
		case 6:
			glDrawArrays(GL_TRIANGLES, 2 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 3 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 4 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 5 + 6 * i, 3);
			break;
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

void makeShapes() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-9.0, 9.0);

	for (int i = 0; i < 3; ++i) { // ��
		GLfloat x = round(dis(gen) * 10) / 100.0f;
		GLfloat y = round(dis(gen) * 10) / 100.0f;
		GLfloat r = (rand() % 10) / 10.0f;
		GLfloat g = (rand() % 10) / 10.0f;
		GLfloat b = (rand() % 10) / 10.0f;

		glm::vec3 temp = { x - 0.01f,y + 0.01f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.01f,y + 0.01f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.01f,y - 0.01f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.01f,y - 0.01f,0.0f };
		vertexPosition.push_back(temp);

		temp = { x - 0.01f,y - 0.01f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.01f,y - 0.01f,0.0f };
		vertexPosition.push_back(temp);

		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
	}

	for (int i = 0; i < 3; ++i) { // ��
		GLfloat x = round(dis(gen) * 10) / 100.0f;
		GLfloat y = round(dis(gen) * 10) / 100.0f;
		GLfloat r = (rand() % 10) / 10.0f;
		GLfloat g = (rand() % 10) / 10.0f;
		GLfloat b = (rand() % 10) / 10.0f;

		glm::vec3 temp = { x,y,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.2f,y,0.0f };
		vertexPosition.push_back(temp);

		temp = { x + 0.2f,y,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.2f,y,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.2f,y,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.2f,y,0.0f };
		vertexPosition.push_back(temp);

		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
	}

	for (int i = 0; i < 3; ++i) { // �ﰢ��
		GLfloat x = round(dis(gen) * 10) / 100.0f;
		GLfloat y = round(dis(gen) * 10) / 100.0f;
		GLfloat r = (rand() % 10) / 10.0f;
		GLfloat g = (rand() % 10) / 10.0f;
		GLfloat b = (rand() % 10) / 10.0f;

		glm::vec3 temp = { x,y + 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);

		temp = { x - 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);

		temp = { r,g,b };
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
	}

	for (int i = 0; i < 3; ++i) { // �簢��
		GLfloat x = round(dis(gen) * 10) / 100.0f;
		GLfloat y = round(dis(gen) * 10) / 100.0f;
		GLfloat r = (rand() % 10) / 10.0f;
		GLfloat g = (rand() % 10) / 10.0f;
		GLfloat b = (rand() % 10) / 10.0f;

		glm::vec3 temp = { x - 0.1f,y + 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.1f,y + 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);

		temp = { x + 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);

		temp = { r,g,b };
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
	}

	for (int i = 0; i < 3; ++i) { // ������
		GLfloat x = round(dis(gen) * 10) / 100.0f;
		GLfloat y = round(dis(gen) * 10) / 100.0f;
		GLfloat r = (rand() % 10) / 10.0f;
		GLfloat g = (rand() % 10) / 10.0f;
		GLfloat b = (rand() % 10) / 10.0f;

		glm::vec3 temp = { x - 0.1f, y + 0.01f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.06f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x, y + 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.06f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.1f, y + 0.01f,0.0f };
		vertexPosition.push_back(temp);

		temp = { x + 0.1f, y + 0.01f,0.0f };
		vertexPosition.push_back(temp);

		temp = { r,g,b };
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
	}

	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� ����
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

	makeShapes();

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

float Sign(float x1, float y1, float x2, float y2, float x3, float y3) {
	return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
}

bool PointInTriangle(float x, float y, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
	// (x, y)�� �ﰢ�� �ȿ� �ִ��� �Ǵ�
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = Sign(x, y, p0.x, p0.y, p1.x, p1.y);
	d2 = Sign(x, y, p1.x, p1.y, p2.x, p2.y);
	d3 = Sign(x, y, p2.x, p2.y, p0.x, p0.y);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

bool PointInRectangle(float x, float y, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	// ���� �簢���� �� �� �ȿ� �ִ��� Ȯ��
	return PointInTriangle(x, y, p0, p1, p2) || PointInTriangle(x, y, p2, p3, p0);
}

// ������ �浹 üũ �Լ�
bool PointInPentagon(float x, float y, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
	return PointInTriangle(x, y, p0, p1, p2) || PointInTriangle(x, y, p1, p2, p3) || PointInTriangle(x, y, p2, p3, p4);
}

// ������ �浹 üũ �Լ�
bool PointInHexagon(float x, float y, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 p5) {
	return PointInTriangle(x, y, p0, p1, p2) || PointInTriangle(x, y, p1, p2, p3) || PointInTriangle(x, y, p2, p3, p4) || PointInTriangle(x, y, p3, p4, p5);
}

void Mouse(int button, int state, int x, int y) {
	GLfloat X, Y;
	ScreenToOpenGL(x, y, X, Y); // ��ǥ ��ȯ

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			isDrag = true;
			for (int i = 0; i < 15; ++i) {
				switch (shape[i]) {
					// ���õ� ���� ����
				case 1:
					if ((X < vertexPosition[5 + 6 * i].x) && (vertexPosition[4 + 6 * i].x < X)
						&& (Y < vertexPosition[2 + 6 * i].y) && (vertexPosition[4 + 6 * i].y < Y)) {
						select = i;
					}
					break;
				case 2:
					if (vertexPosition[2 + 6 * i].y == Y
						&& vertexPosition[2 + 6 * i].x < X && vertexPosition[3 + 6 * i].x > X) {
						select = i;
					}
					break;
				case 3:
					if (PointInTriangle(X, Y, vertexPosition[2 + 6 * i], vertexPosition[3 + 6 * i], vertexPosition[4 + 6 * i])) {
						select = i;
					}
					break;
				case 4:
					if ((X < vertexPosition[5 + 6 * i].x) && (vertexPosition[4 + 6 * i].x < X)
						&& (Y < vertexPosition[2 + 6 * i].y) && (vertexPosition[4 + 6 * i].y < Y)) {
						select = i;
					}
					break;
				case 5:
					if (PointInPentagon(X, Y, vertexPosition[2 + 6 * i], vertexPosition[3 + 6 * i], vertexPosition[4 + 6 * i], vertexPosition[5 + 6 * i], vertexPosition[6 + 6 * i])) {
						select = i;
					}
					break;
				case 6:
					if (PointInHexagon(X, Y, vertexPosition[2 + 6 * i], vertexPosition[3 + 6 * i], vertexPosition[4 + 6 * i], vertexPosition[5 + 6 * i], vertexPosition[6 + 6 * i], vertexPosition[7 + 6 * i])) {
						select = i;
					}
					break;
				}
			}
		}
		else if (state == GLUT_UP && isDrag) {
			isDrag = false;
			int selected = -1;

			for (int i = 0; i < 15; ++i) {
				if (i != select) {
					switch (shape[i]) {
						// ���õ� ���� ����
					case 1:
						if ((X < vertexPosition[5 + 6 * i].x) && (vertexPosition[4 + 6 * i].x < X)
							&& (Y < vertexPosition[2 + 6 * i].y) && (vertexPosition[4 + 6 * i].y < Y)) {
							selected = i;
						}
						break;
					case 2:
						switch (shape[select]) {
						case 1:
							if (PointInRectangle(vertexPosition[2 + 6 * i].x, vertexPosition[2 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * i])
								|| PointInRectangle(vertexPosition[3 + 6 * i].x, vertexPosition[3 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * i])) {
								selected = i;
							}
							break;
						case 3:
							if (PointInTriangle(vertexPosition[2 + 6 * i].x, vertexPosition[2 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select])
								|| PointInTriangle(vertexPosition[3 + 6 * i].x, vertexPosition[3 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select])) {
								selected = i;
							}
							break;
						case 4:
							if (PointInRectangle(vertexPosition[2 + 6 * i].x, vertexPosition[2 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * select])
								|| PointInRectangle(vertexPosition[3 + 6 * i].x, vertexPosition[3 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * select])) {
								selected = i;
							}
							break;
						case 5:
							if (PointInPentagon(vertexPosition[2 + 6 * i].x, vertexPosition[2 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * select], vertexPosition[6 + 6 * select])
								|| PointInPentagon(vertexPosition[3 + 6 * i].x, vertexPosition[3 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * select], vertexPosition[6 + 6 * select])) {
								selected = i;
							}
							break;
						case 6:
							if (PointInHexagon(vertexPosition[2 + 6 * i].x, vertexPosition[2 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * select], vertexPosition[6 + 6 * select], vertexPosition[7 + 6 * select])
								|| PointInHexagon(vertexPosition[3 + 6 * i].x, vertexPosition[3 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * select], vertexPosition[6 + 6 * select], vertexPosition[7 + 6 * select])) {
								selected = i;
							}
							break;
						}
						break;
					case 3:
						if (PointInTriangle(X, Y, vertexPosition[2 + 6 * i], vertexPosition[3 + 6 * i], vertexPosition[4 + 6 * i])) {
							selected = i;
						}
						break;
					case 4:
						if ((X < vertexPosition[5 + 6 * i].x) && (vertexPosition[4 + 6 * i].x < X)
							&& (Y < vertexPosition[2 + 6 * i].y) && (vertexPosition[4 + 6 * i].y < Y)) {
							selected = i;
						}
						break;
					case 5:
						if (PointInPentagon(X, Y, vertexPosition[2 + 6 * i], vertexPosition[3 + 6 * i], vertexPosition[4 + 6 * i], vertexPosition[5 + 6 * i], vertexPosition[6 + 6 * i])) {
							selected = i;
						}
						break;
					case 6:
						if (PointInHexagon(X, Y, vertexPosition[2 + 6 * i], vertexPosition[3 + 6 * i], vertexPosition[4 + 6 * i], vertexPosition[5 + 6 * i], vertexPosition[6 + 6 * i], vertexPosition[7 + 6 * i])) {
							selected = i;
						}
						break;
					}
				}
			}

			if (selected != -1) {
				shape[selected] = (shape[select] + shape[selected]) % 7;
				if (shape[selected] == 0)
					shape[selected] = 1;
				shape[select] = -1;

				GLfloat r = (rand() % 10) / 10.0f;
				GLfloat g = (rand() % 10) / 10.0f;
				GLfloat b = (rand() % 10) / 10.0f;
				vertexColor[2 + 6 * selected] = { r,g,b };
				vertexColor[3 + 6 * selected] = { r,g,b };
				vertexColor[4 + 6 * selected] = { r,g,b };
				vertexColor[5 + 6 * selected] = { r,g,b };
				vertexColor[6 + 6 * selected] = { r,g,b };
				vertexColor[7 + 6 * selected] = { r,g,b };

				switch (shape[selected]) {
				case 1:
					vertexPosition[2 + 6 * selected] = { X - 0.01f,Y + 0.01f,0.0f };
					vertexPosition[3 + 6 * selected] = { X + 0.01f,Y + 0.01f,0.0f };
					vertexPosition[4 + 6 * selected] = { X - 0.01f,Y - 0.01f,0.0f };
					vertexPosition[5 + 6 * selected] = { X + 0.01f,Y - 0.01f,0.0f };
					break;
				case 2:
					vertexPosition[2 + 6 * selected] = { X - 0.1f,Y,0.0f };
					vertexPosition[3 + 6 * selected] = { X + 0.1f,Y,0.0f };
					break;
				case 3:
					vertexPosition[2 + 6 * selected] = { X,Y + 0.1f,0.0f };
					vertexPosition[3 + 6 * selected] = { X + 0.1f,Y - 0.1f,0.0f };
					vertexPosition[4 + 6 * selected] = { X - 0.1f,Y - 0.1f,0.0f };
					break;
				case 4:
					vertexPosition[2 + 6 * selected] = { X - 0.1f,Y + 0.1f,0.0f };
					vertexPosition[3 + 6 * selected] = { X + 0.1f,Y + 0.1f,0.0f };
					vertexPosition[4 + 6 * selected] = { X - 0.1f,Y - 0.1f,0.0f };
					vertexPosition[5 + 6 * selected] = { X + 0.1f,Y - 0.1f,0.0f };
					break;
				case 5:
					vertexPosition[2 + 6 * selected] = { X - 0.1f, Y + 0.01f,0.0f };
					vertexPosition[3 + 6 * selected] = { X - 0.06f,Y - 0.1f,0.0f };
					vertexPosition[4 + 6 * selected] = { X, Y + 0.1f,0.0f };
					vertexPosition[5 + 6 * selected] = { X + 0.06f,Y - 0.1f,0.0f };
					vertexPosition[6 + 6 * selected] = { X + 0.1f, Y + 0.01f,0.0f };
					break;
				case 6:
					vertexPosition[2 + 6 * selected] = { X - 0.05f,Y + 0.1f,0.0f };
					vertexPosition[3 + 6 * selected] = { X + 0.05f,Y + 0.1f,0.0f };
					vertexPosition[4 + 6 * selected] = { X - 0.1f,Y,0.0f };
					vertexPosition[5 + 6 * selected] = { X + 0.1f,Y,0.0f };
					vertexPosition[6 + 6 * selected] = { X - 0.05f,Y - 0.1f,0.0f };
					vertexPosition[7 + 6 * selected] = { X + 0.05f,Y - 0.1f,0.0f };
					break;
				}
				mode[selected] = rand() % 2 + 1;
			}
			select = -1;
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
		switch (shape[select]) {
		case 1:
			vertexPosition[2 + 6 * select] = { X - 0.01f,Y + 0.01f,0.0f };
			vertexPosition[3 + 6 * select] = { X + 0.01f,Y + 0.01f,0.0f };
			vertexPosition[4 + 6 * select] = { X - 0.01f,Y - 0.01f,0.0f };
			vertexPosition[5 + 6 * select] = { X + 0.01f,Y - 0.01f,0.0f };
			break;
		case 2:
			vertexPosition[2 + 6 * select] = { X - 0.1f,Y,0.0f };
			vertexPosition[3 + 6 * select] = { X + 0.1f,Y,0.0f };
			break;
		case 3:
			vertexPosition[2 + 6 * select] = { X ,Y + 0.1f,0.0f };
			vertexPosition[3 + 6 * select] = { X + 0.1f,Y - 0.1f,0.0f };
			vertexPosition[4 + 6 * select] = { X - 0.1f,Y - 0.1f,0.0f };
			break;
		case 4:
			vertexPosition[2 + 6 * select] = { X - 0.1f,Y + 0.1f,0.0f };
			vertexPosition[3 + 6 * select] = { X + 0.1f,Y + 0.1f,0.0f };
			vertexPosition[4 + 6 * select] = { X - 0.1f,Y - 0.1f,0.0f };
			vertexPosition[5 + 6 * select] = { X + 0.1f,Y - 0.1f,0.0f };
			break;
		case 5:
			vertexPosition[2 + 6 * select] = { X - 0.1f, Y + 0.01f,0.0f };
			vertexPosition[3 + 6 * select] = { X - 0.06f,Y - 0.1f,0.0f };
			vertexPosition[4 + 6 * select] = { X , Y + 0.1f,0.0f };
			vertexPosition[5 + 6 * select] = { X + 0.06f,Y - 0.1f,0.0f };
			vertexPosition[6 + 6 * select] = { X + 0.1f, Y + 0.01f,0.0f };
			break;
		case 6:
			vertexPosition[2 + 6 * select] = { X - 0.05f, Y + 0.1f,0.0f };
			vertexPosition[3 + 6 * select] = { X + 0.05f, Y + 0.1f,0.0f };
			vertexPosition[4 + 6 * select] = { X - 0.1f, Y ,0.0f };
			vertexPosition[5 + 6 * select] = { X + 0.1f, Y ,0.0f };
			vertexPosition[6 + 6 * select] = { X - 0.05f, Y - 0.1f ,0.0f };
			vertexPosition[7 + 6 * select] = { X + 0.05f, Y - 0.1f,0.0f };
			break;
		}
	}
	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� ����
}


void TimerFunction(int value)
{
	for (int i = 0; i < 15; ++i) {
		if (mode[i] == 1) { // ������� �翷
			if (move[i].moveNum == 0) {
				vertexPosition[2 + 6 * i].x += move[i].dx;
				vertexPosition[3 + 6 * i].x += move[i].dx;
				vertexPosition[4 + 6 * i].x += move[i].dx;
				vertexPosition[5 + 6 * i].x += move[i].dx;
				vertexPosition[6 + 6 * i].x += move[i].dx;
				vertexPosition[7 + 6 * i].x += move[i].dx;

				GLfloat M = vertexPosition[2 + 6 * i].x; // �ʱⰪ ����
				GLfloat m = vertexPosition[2 + 6 * i].x; // �ʱⰪ ����

				// ��ǥ�� �߿��� �ִ񰪰� �ּڰ��� ã��
				for (int j = 2; j <= 7; ++j) {
					GLfloat x = vertexPosition[j + 6 * i].x;
					if (x > M) {
						M = x; // �ִ� ����
					}
					if (x < m) {
						m = x; // �ּڰ� ����
					}
				}
				if (M >= 1.0 || m <= -1) {
					++move[i].moveNum;
					move[i].dx *= -1;
				}
			}
			else {
				vertexPosition[2 + 6 * i].y += move[i].dy;
				vertexPosition[3 + 6 * i].y += move[i].dy;
				vertexPosition[4 + 6 * i].y += move[i].dy;
				vertexPosition[5 + 6 * i].y += move[i].dy;
				vertexPosition[6 + 6 * i].y += move[i].dy;
				vertexPosition[7 + 6 * i].y += move[i].dy;

				GLfloat M = vertexPosition[2 + 6 * i].y; // �ʱⰪ ����
				GLfloat m = vertexPosition[2 + 6 * i].y; // �ʱⰪ ����

				// ��ǥ�� �߿��� �ִ񰪰� �ּڰ��� ã��
				for (int j = 2; j <= 7; ++j) {
					GLfloat y = vertexPosition[j + 6 * i].y;
					if (y > M) {
						M = y; // �ִ� ����
					}
					if (y < m) {
						m = y; // �ּڰ� ����
					}
				}
				if (M >= 1.0 || m <= -1) {
					++move[i].moveNum;
					move[i].dy *= -1;
				}

				++move[i].moveNum;
				if (move[i].moveNum == 15)
					move[i].moveNum = 0;
			}
		}
		else if (mode[i] == 2) {//
			vertexPosition[2 + 6 * i].x += move[i].dx;
			vertexPosition[3 + 6 * i].x += move[i].dx;
			vertexPosition[4 + 6 * i].x += move[i].dx;
			vertexPosition[5 + 6 * i].x += move[i].dx;
			vertexPosition[6 + 6 * i].x += move[i].dx;
			vertexPosition[7 + 6 * i].x += move[i].dx;
			vertexPosition[2 + 6 * i].y += move[i].dy;
			vertexPosition[3 + 6 * i].y += move[i].dy;
			vertexPosition[4 + 6 * i].y += move[i].dy;
			vertexPosition[5 + 6 * i].y += move[i].dy;
			vertexPosition[6 + 6 * i].y += move[i].dy;
			vertexPosition[7 + 6 * i].y += move[i].dy;

			GLfloat X = vertexPosition[2 + 6 * i].x; // �ʱⰪ ����
			GLfloat x = vertexPosition[2 + 6 * i].x; // �ʱⰪ ����
			GLfloat Y = vertexPosition[2 + 6 * i].y; // �ʱⰪ ����
			GLfloat y = vertexPosition[2 + 6 * i].y; // �ʱⰪ ����

			// ��ǥ�� �߿��� �ִ񰪰� �ּڰ��� ã��
			for (int j = 2; j <= 7; ++j) {
				GLfloat xx = vertexPosition[j + 6 * i].x;
				GLfloat yy = vertexPosition[j + 6 * i].y;
				if (xx > X) {
					X = xx; // �ִ� ����
				}
				if (xx < x) {
					x = xx; // �ּڰ� ����
				}
				if (yy > Y) {
					Y = yy; // �ִ� ����
				}
				if (yy < y) {
					y = yy; // �ּڰ� ����
				}
			}

			if (X >= 1.0 || x <= -1) {
				move[i].dx *= -1;
			}
			if (Y >= 1.0 || y <= -1) {
				move[i].dy *= -1;
			}
		}

	}
	InitBuffer(); // ���� �ʱ�ȭ
	glutPostRedisplay(); // ȭ�� ����

	glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸� �Լ� ȣ��
}