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
struct move {
	GLfloat dx;
	GLfloat dy;
	int moveNum;
	int mode;
	int num;
	int n;
};
move triangle[4];
const float pi = 3.14159265359f; // pi �� ����
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
	glm::vec3(-0.6f,0.4f,0.0f),
	glm::vec3(-0.4f,0.4f,0.0f),
	//2
	glm::vec3(0.5f,0.6f,0.0f),
	glm::vec3(0.4f,0.4f,0.0f),
	glm::vec3(0.6f,0.4f,0.0f),
	//3
	glm::vec3(-0.5f,-0.4f,0.0f),
	glm::vec3(-0.6f,-0.6f,0.0f),
	glm::vec3(-0.4f,-0.6f,0.0f),
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

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID); // ���̴� ���α׷� Ȱ��ȭ
	glBindVertexArray(VAO);  // VAO ���ε�

	glUseProgram(shaderProgramID);


	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_LINES, 2, 2);
	glDrawArrays(GL_TRIANGLES, 4, 3);
	glDrawArrays(GL_TRIANGLES, 7, 3);
	glDrawArrays(GL_TRIANGLES, 10, 3);
	glDrawArrays(GL_TRIANGLES, 13, 3);

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

	for (int i = 0; i < 4; ++i) {
		triangle[i].dx = -0.01f;
		triangle[i].dy = 0.01f;
		triangle[i].moveNum = 0;
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
	case '1':
		mode = 1;
		for (int i = 0; i < 4; ++i) {
			triangle[i] = { 0.01f,0.01f,0,0 };
		}
		{
			vertexPosition[4] = { -0.5f, 0.6f, 0.0f };
			vertexPosition[5] = { -0.6f, 0.4f, 0.0f };
			vertexPosition[6] = { -0.4f, 0.4f, 0.0f };
			vertexPosition[7] = { 0.5f, 0.6f, 0.0f };
			vertexPosition[8] = { 0.4f, 0.4f, 0.0f };
			vertexPosition[9] = { 0.6f, 0.4f, 0.0f };
			vertexPosition[10] = { -0.5f, -0.4f, 0.0f };
			vertexPosition[11] = { -0.6f, -0.6f, 0.0f };
			vertexPosition[12] = { -0.4f, -0.6f, 0.0f };
			vertexPosition[13] = { 0.5f, -0.4f, 0.0f };
			vertexPosition[14] = { 0.4f, -0.6f, 0.0f };
			vertexPosition[15] = { 0.6f, -0.6f, 0.0f };
		}
		break;
	case '2':
		mode = 2;
		for (int i = 0; i < 4; ++i) {
			triangle[i] = { 0.01f,0.01f,0,0 };
		}
		{
			vertexPosition[4] = { -0.5f, 0.6f, 0.0f };
			vertexPosition[5] = { -0.6f, 0.4f, 0.0f };
			vertexPosition[6] = { -0.4f, 0.4f, 0.0f };
			vertexPosition[7] = { 0.5f, 0.6f, 0.0f };
			vertexPosition[8] = { 0.4f, 0.4f, 0.0f };
			vertexPosition[9] = { 0.6f, 0.4f, 0.0f };
			vertexPosition[10] = { -0.5f, -0.4f, 0.0f };
			vertexPosition[11] = { -0.6f, -0.6f, 0.0f };
			vertexPosition[12] = { -0.4f, -0.6f, 0.0f };
			vertexPosition[13] = { 0.5f, -0.4f, 0.0f };
			vertexPosition[14] = { 0.4f, -0.6f, 0.0f };
			vertexPosition[15] = { 0.6f, -0.6f, 0.0f };
		}
		break;
	case '3':
		mode = 3;
		for (int i = 0; i < 4; ++i) {
			triangle[i] = { 0.01f,0.01f,0,0 - (i * 20) ,1,0 };
			vertexPosition[4 + 3 * i] = { 0.0f, 0.0f + 0.1f, 0.0f };
			vertexPosition[5 + 3 * i] = { 0.0f - 0.1f, 0.0f - 0.1f, 0.0f };
			vertexPosition[6 + 3 * i] = { 0.0f + 0.1f, 0.0f - 0.1f, 0.0f };
		}
		break;
	case '4':
		mode = 4;
		for (int i = 0; i < 4; ++i) {
			triangle[i] = { 0.01f,0.01f,0-i,0 };
			vertexPosition[4 + 3 * i] = { 0.0f, 0.0f + 0.1f, 0.0f };
			vertexPosition[5 + 3 * i] = { 0.0f - 0.1f, 0.0f - 0.1f, 0.0f };
			vertexPosition[6 + 3 * i] = { 0.0f + 0.1f, 0.0f - 0.1f, 0.0f };
		}
		break;
	}
	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� ����
}

void Mouse(int button, int state, int x, int y) {
	GLfloat X, Y;
	ScreenToOpenGL(x, y, X, Y); // ��ǥ ��ȯ


	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� ����
}

void TimerFunction(int value)
{ //4~15
	switch (mode) {
	case 1:
		for (int i = 0; i < 4; ++i) {
			vertexPosition[4 + 3 * i].x += triangle[i].dx;
			vertexPosition[4 + 3 * i].y += triangle[i].dy;
			vertexPosition[5 + 3 * i].x += triangle[i].dx;
			vertexPosition[5 + 3 * i].y += triangle[i].dy;
			vertexPosition[6 + 3 * i].x += triangle[i].dx;
			vertexPosition[6 + 3 * i].y += triangle[i].dy;
			if ((vertexPosition[4 + 3 * i].y > 1) || vertexPosition[6 + 3 * i].y < -1) {
				triangle[i].dy *= -1;
				if (vertexPosition[5 + 3 * i].y == vertexPosition[6 + 3 * i].y) {
					vertexPosition[4 + 3 * i].x += 0.1f;
					vertexPosition[5 + 3 * i].y += 0.1f;
				}
				else {
					vertexPosition[4 + 3 * i].x -= 0.1f;
					vertexPosition[5 + 3 * i].y -= 0.1f;
				}
			}
			if ((vertexPosition[5 + 3 * i].x < -1) || (vertexPosition[6 + 3 * i].x > 1)) {
				triangle[i].dx *= -1;
				if (vertexPosition[5 + 3 * i].y == vertexPosition[6 + 3 * i].y) {
					vertexPosition[4 + 3 * i].x += 0.1f;
					vertexPosition[5 + 3 * i].y += 0.1f;
				}
				else {
					vertexPosition[4 + 3 * i].x -= 0.1f;
					vertexPosition[5 + 3 * i].y -= 0.1f;
				}
			}
		}
		break;
	case 2:
		for (int i = 0; i < 4; ++i) {
			if ((vertexPosition[5 + 3 * i].x <= -1) || (vertexPosition[6 + 3 * i].x >= 1)) {
				if (triangle[i].moveNum < 10) {
					vertexPosition[4 + 3 * i].y += triangle[i].dy;
					vertexPosition[5 + 3 * i].y += triangle[i].dy;
					vertexPosition[6 + 3 * i].y += triangle[i].dy;
					++triangle[i].moveNum;
				}
				else {
					if (vertexPosition[5 + 3 * i].y == vertexPosition[6 + 3 * i].y) {
						vertexPosition[4 + 3 * i].x += 0.1f;
						vertexPosition[5 + 3 * i].y += 0.1f;
					}
					else {
						vertexPosition[4 + 3 * i].x -= 0.1f;
						vertexPosition[5 + 3 * i].y -= 0.1f;
					}

					triangle[i].moveNum = 0;
					triangle[i].dx *= -1;
					vertexPosition[4 + 3 * i].x += 2 * triangle[i].dx;
					vertexPosition[5 + 3 * i].x += 2 * triangle[i].dx;
					vertexPosition[6 + 3 * i].x += 2 * triangle[i].dx;
				}
			}
			else {
				vertexPosition[4 + 3 * i].x += triangle[i].dx;
				vertexPosition[5 + 3 * i].x += triangle[i].dx;
				vertexPosition[6 + 3 * i].x += triangle[i].dx;
			}

			if ((vertexPosition[4 + 3 * i].y >= 1) || (vertexPosition[6 + 3 * i].y <= -1)) {
				if ((vertexPosition[5 + 3 * i].x <= -1) || (vertexPosition[6 + 3 * i].x >= 1)) {
					triangle[i].dy *= -1;
					if (vertexPosition[5 + 3 * i].y == vertexPosition[6 + 3 * i].y) {
						vertexPosition[4 + 3 * i].x += 0.1f;
						vertexPosition[5 + 3 * i].y += 0.1f;
					}
					else {
						vertexPosition[4 + 3 * i].x -= 0.1f;
						vertexPosition[5 + 3 * i].y -= 0.1f;
					}
				}
			}
		}
		break;
	case 3:
		for (int i = 0; i < 4; ++i) {
			switch (triangle[i].mode) {
			case 0:
				vertexPosition[4 + 3 * i].x += triangle[i].dx;
				vertexPosition[5 + 3 * i].x += triangle[i].dx;
				vertexPosition[6 + 3 * i].x += triangle[i].dx;
				++triangle[i].moveNum;
				break;
			case 1:
				vertexPosition[4 + 3 * i].y -= triangle[i].dy;
				vertexPosition[5 + 3 * i].y -= triangle[i].dy;
				vertexPosition[6 + 3 * i].y -= triangle[i].dy;
				++triangle[i].moveNum;
				break;
			case 2:
				vertexPosition[4 + 3 * i].x -= triangle[i].dx;
				vertexPosition[5 + 3 * i].x -= triangle[i].dx;
				vertexPosition[6 + 3 * i].x -= triangle[i].dx;
				++triangle[i].moveNum;
				break;
			case 3:
				vertexPosition[4 + 3 * i].y += triangle[i].dy;
				vertexPosition[5 + 3 * i].y += triangle[i].dy;
				vertexPosition[6 + 3 * i].y += triangle[i].dy;
				++triangle[i].moveNum;
				break;
			}

			if (triangle[i].moveNum % (15 * triangle[i].num) == 0) {
				if (triangle[i].mode == 1 || triangle[i].mode == 3)
					triangle[i].num++;
				triangle[i].mode++;
				if (triangle[i].mode == 4)
					triangle[i].mode = 0;

				

				if (triangle[i].mode >= 0) {
					if (triangle[i].n % 4 == 0) {
						vertexPosition[4 + 3 * i].x += 0.1f;
						vertexPosition[5 + 3 * i].y += 0.1f;
					}
					else if (triangle[i].n % 4 == 1) {
						vertexPosition[5 + 3 * i].y += 0.1f;
						vertexPosition[6 + 3 * i].x -= 0.1f;
					}
					else if (triangle[i].n % 4 == 2) {
						vertexPosition[4 + 3 * i].x -= 0.2f;
						vertexPosition[5 + 3 * i].y -= 0.2f;
						vertexPosition[6 + 3 * i].x += 0.1f;
						vertexPosition[6 + 3 * i].y += 0.1f;
					}
					else if (triangle[i].n % 4 == 3) {
						vertexPosition[4 + 3 * i].x += 0.1f;
						vertexPosition[6 + 3 * i].y -= 0.1f;
					}
					++triangle[i].n;
				}
			}
		}
		break;
	case 4:
		for (int i = 0; i < 4; ++i) {
			if(triangle[i].moveNum>0) {
				// �ﰢ���� �߽��� ��ǥ (�̵� ������ ����)
				GLfloat x = (triangle[i].moveNum / 500.0f) * cos(triangle[i].moveNum * pi / 180.0f);
				GLfloat y = (triangle[i].moveNum / 500.0f) * sin(triangle[i].moveNum * pi / 180.0f);
				triangle[i].moveNum += 5; // �ﰢ���� �̵��� ���� ���� ����

				glm::vec3 center = glm::vec3(x, y, 0.0f); // �߽��� ������Ʈ

				// ȸ�� ���� ��� (�����̴� ������ŭ ȸ��)
				float angle = triangle[i].moveNum * pi / 180.0f; // ���� ���� �������� ��ȯ

				// ȸ�� ��ȯ ��� ���� (Z���� �������� ȸ��)
				glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

				// �ﰢ���� �� ������ ȸ�� ��ȯ ���� (�߽����� ��������)
				glm::vec4 p1 = glm::vec4(glm::vec3(0.0f, 0.1f, 0.0f) - center, 1.0f); // �߽��� �������� �̵�
				glm::vec4 p2 = glm::vec4(glm::vec3(-0.1f, -0.1f, 0.0f) - center, 1.0f);
				glm::vec4 p3 = glm::vec4(glm::vec3(0.1f, -0.1f, 0.0f) - center, 1.0f);

				// ȸ�� ��ȯ �� �ٽ� �߽��� ��ġ�� �̵�
				vertexPosition[4 + 3 * i] = glm::vec3(rotationMatrix * p1) + center;
				vertexPosition[5 + 3 * i] = glm::vec3(rotationMatrix * p2) + center;
				vertexPosition[6 + 3 * i] = glm::vec3(rotationMatrix * p3) + center;
			}
			++triangle[i].moveNum;
		}
		break;
	}
	InitBuffer(); //���� �ʱ�ȭ
	glutPostRedisplay(); //ȭ�� ����
	glutTimerFunc(100, TimerFunction, 1); // Ÿ�̸� �Լ� ȣ��
}