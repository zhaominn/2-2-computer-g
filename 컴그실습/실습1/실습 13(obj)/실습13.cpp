#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <gl/glew.h>
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
//----------------------------------
int faceNum = 1;
int Num1, Num2;
//----------------------------------
#define MAX_LINE_LENGTH 1024

typedef struct {
	float x, y, z;
} Vertex;

typedef struct {
	unsigned int v1, v2, v3;
} Face;

typedef struct {
	std::vector<Vertex> vertices;
	std::vector<Face> faces;
} Model;

Model objModel; // �� �����͸� ������ ���� ����

// OBJ ���Ͽ��� ���ؽ��� ���� �о���� �Լ�
void read_obj_file(const char* filename, Model* model) {
	FILE* file;
	fopen_s(&file, filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	char line[MAX_LINE_LENGTH];
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'v' && line[1] == ' ') {
			// ���ؽ� ��ǥ �б�
			Vertex vertex;
			sscanf_s(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			model->vertices.push_back(vertex);
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			// �� ������ �б� (v//vn ����)
			unsigned int v1, v2, v3, vn1, vn2, vn3;
			int matches = sscanf_s(line + 2, "%u//%u %u//%u %u//%u", &v1, &vn1, &v2, &vn2, &v3, &vn3);

			if (matches == 6) {
				// v1, v2, v3�� 1-based �ε����̹Ƿ� 0-based �ε����� ����
				Face face;
				face.v1 = v1 - 1;
				face.v2 = v2 - 1;
				face.v3 = v3 - 1;
				model->faces.push_back(face);
			}
		}
	}
	fclose(file);

	// �Ľ̵� ���ؽ��� �� �����͸� �ֿܼ� ���
	for (const auto& vertex : model->vertices) {
		std::cout << "Vertex: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;
	}

	for (const auto& face : model->faces) {
		std::cout << "Face: (" << face.v1 << ", " << face.v2 << ", " << face.v3 << ")" << std::endl;
	}
}

GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü

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

void make_vertexShaders() {
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
	}
	else {
		std::cout << "Vertex shader ������ ����!" << std::endl;
	}
}

void make_fragmentShaders() {
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
	}
	else {
		std::cout << "Fragment shader ������ ����!" << std::endl;
	}
}

GLuint make_shaderProgram() {
	GLint result;
	GLchar errorLog[512];
	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);
	glLinkProgram(shaderID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program ���� ����\n" << errorLog << std::endl;
		return false;
	}
	else {
		std::cout << "Shader program ���� ����!" << std::endl;
	}

	glUseProgram(shaderID);
	return shaderID;
}

GLuint cubeVAO, cubeVBO_position, cubeVBO_color, cubeEBO;
GLuint pyramidVAO, pyramidVBO_position, pyramidVBO_color, pyramidEBO;
GLuint shaderProgram;

Model cubeModel, pyramidModel;

void InitBufferForCube() {
	// �ε��� ������
	std::vector<unsigned int> indices;

	for (const auto& face : cubeModel.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	// ���� ������ ����
	std::vector<glm::vec3> vertexColor = {
		glm::vec3(1.0f, 0.2f, 0.2f), glm::vec3(1.0f, 0.2f, 0.2f), glm::vec3(1.0f, 0.2f, 0.2f), glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.4f, 0.4f), glm::vec3(1.0f, 0.4f, 0.4f), glm::vec3(1.0f, 0.4f, 0.4f), glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f), glm::vec3(1.0f, 0.5f, 0.5f), glm::vec3(1.0f, 0.5f, 0.5f), glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f), glm::vec3(1.0f, 0.6f, 0.6f), glm::vec3(1.0f, 0.6f, 0.6f), glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.8f, 0.8f), glm::vec3(1.0f, 0.8f, 0.8f), glm::vec3(1.0f, 0.8f, 0.8f), glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 0.8f), glm::vec3(1.0f, 1.0f, 0.8f), glm::vec3(1.0f, 1.0f, 0.8f), glm::vec3(1.0f, 1.0f, 0.8f),
	};

	// VAO ����
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &cubeVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position);
	glBufferData(GL_ARRAY_BUFFER, cubeModel.vertices.size() * sizeof(Vertex), &cubeModel.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);


	glGenBuffers(1, &cubeVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(glm::vec3), &vertexColor[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO ����
	glGenBuffers(1, &cubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void InitBufferForPyramid() {
	// �ε��� ������
	std::vector<unsigned int> indices;

	for (const auto& face : pyramidModel.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	// ���� ������ ����
	std::vector<glm::vec3> vertexColor = {
		glm::vec3(1.0f, 0.2f, 0.2f),glm::vec3(1.0f, 0.2f, 0.2f),glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.4f, 0.4f),glm::vec3(1.0f, 0.4f, 0.4f),glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),glm::vec3(1.0f, 0.5f, 0.5f),glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),glm::vec3(1.0f, 0.6f, 0.6f),glm::vec3(1.0f, 0.6f, 0.6f),
	};

	// VAO ����
	glGenVertexArrays(1, &pyramidVAO);
	glBindVertexArray(pyramidVAO);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &pyramidVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO_position);
	glBufferData(GL_ARRAY_BUFFER, pyramidModel.vertices.size() * sizeof(Vertex), &pyramidModel.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);


	glGenBuffers(1, &pyramidVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(glm::vec3), &vertexColor[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO ����
	glGenBuffers(1, &pyramidEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void InitOpenGL() {
	// GLEW �ʱ�ȭ
	glewExperimental = GL_TRUE;
	glewInit();

	// ���� �׽�Ʈ Ȱ��ȭ
	glEnable(GL_DEPTH_TEST);

	// ���̴� ������
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgram = make_shaderProgram();

	// OBJ ���� �ε�
	read_obj_file("cube.obj", &cubeModel);
	read_obj_file("pyramid.obj", &pyramidModel);

	// ���� �ʱ�ȭ
	InitBufferForCube();
	InitBufferForPyramid();
}

GLvoid drawScene() {
	// �÷� ���ۿ� ���� ���۸� ����ϴ�
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glm::mat4 Ry = glm::mat4(1.0f); //--- zȸ�� ��� ����
	glm::mat4 Rx = glm::mat4(1.0f); //--- xȸ�� ��� ����
	glm::mat4 RR = glm::mat4(1.0f); //--- �ռ� ��ȯ ���
	Ry = glm::rotate(Ry, glm::radians(10.0f), glm::vec3(0.0, 1.0, 0.0)); //--- y�࿡ ���Ͽ� ȸ�� ���
	Rx = glm::rotate(Rx, glm::radians(10.0f), glm::vec3(1.0, 0.0, 0.0)); //--- x�࿡ ���Ͽ� ȸ�� ���
	RR = Rx * Ry;
	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(RR)); //--- modelTransform ������ ��ȯ �� �����ϱ�

	std::cout << "Drawing face number: " << faceNum << std::endl;

	// VAO ���ε� �� �׸���
	if (faceNum >= 1 && faceNum <= 6) {
		glBindVertexArray(cubeVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * (faceNum - 1)));
	}
	else if (faceNum >= 7 && faceNum <= 10) {
		glBindVertexArray(pyramidVAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 3 * (faceNum - 7)));
	}
	else if (faceNum == 11) {
		glBindVertexArray(cubeVAO);
		Num1 = rand() % 6 + 1;
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * (Num1 - 1)));
		do
			Num2 = rand() % 6 + 1;
		while (Num1 == Num2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * (Num2 - 1)));
	}
	else if (faceNum == 12) {
		glBindVertexArray(pyramidVAO);
		Num1 = rand() % 4 + 1;
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 3 * (Num1 - 1)));
		do
			Num2 = rand() % 4 + 1;
		while (Num1 == Num2);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 3 * (Num2 - 1)));
	}
	glBindVertexArray(0);

	// ���۸� ȭ�鿡 ǥ��
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void TimerFunction(int value)
{
	glutPostRedisplay(); // ȭ�� ����

	//glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸� �Լ� ȣ��
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1': faceNum = 1; break;
	case '2': faceNum = 2; break;
	case '3': faceNum = 3; break;
	case '4': faceNum = 4; break;
	case '5': faceNum = 5; break;
	case '6': faceNum = 6; break;
	case '7': faceNum = 7; break;
	case '8': faceNum = 8; break;
	case '9': faceNum = 9; break;
	case '0': faceNum = 10; break;
	case 'c': faceNum = 11; break;
	case 't': faceNum = 12; break;
	}

	InitOpenGL();
	glutPostRedisplay(); //ȭ�� ����
}

void main(int argc, char** argv) {
	// ������ ����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("������");

	glEnable(GL_DEPTH_TEST);

	InitOpenGL();

	glutDisplayFunc(drawScene); //--- ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); //--- �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard); //--- Ű���� �Է� �ݹ��Լ� ����
	glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	glutMainLoop(); //--- �̺�Ʈ ó�� ����
}