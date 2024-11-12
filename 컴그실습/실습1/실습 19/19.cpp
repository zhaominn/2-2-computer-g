#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <gl/glew.h>
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
//---------------------
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
bool isAnimating = false;
bool b = false; bool B = false;
bool m = false; bool M = false;
bool f = false; bool F = false;
bool e = false; bool E = false;
bool t = false;
GLfloat Tx = 0.0f; GLfloat ry = 0.0f; int ry34 = 0; int dy34 = 5;
GLfloat tx34 = 1.0f; int rz12 = 0; int dz12 = 5;
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
				std::cout << "Face added: (" << face.v1 << ", " << face.v2 << ", " << face.v3 << ")\n";
			}
		}
	}
	std::cout << "complete file " << filename << '\n';
	fclose(file);
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

GLuint axisVAO, axisVBO_position, axisVBO_color;
GLuint shaderProgram;

void InitBufferForAxes() {
	// ���� ���� ������
	std::vector<glm::vec3> axisVertices = {
		// X��
		glm::vec3(-10.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.0f, 0.0f),
		// Y��
		glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(0.0f, 10.0f, 0.0f),
		// Z��
		glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 10.0f)
	};

	// ���� ���� ������
	std::vector<glm::vec3> axisColors = {
		// X���� ������
		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
		// Y���� �ʷϻ�
		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		// Z���� �Ķ���
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)
	};

	// VAO ����
	glGenVertexArrays(1, &axisVAO);
	glBindVertexArray(axisVAO);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &axisVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, axisVBO_position);
	glBufferData(GL_ARRAY_BUFFER, axisVertices.size() * sizeof(glm::vec3), &axisVertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(pAttribute);

	// VBO ���� (���� ������)
	glGenBuffers(1, &axisVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, axisVBO_color);
	glBufferData(GL_ARRAY_BUFFER, axisColors.size() * sizeof(glm::vec3), &axisColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);

	glBindVertexArray(0);
}

void drawAxes() {
	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 6);  // 6���� ������ �׸��� (�� ��� 2���� ����)
	glBindVertexArray(0);
}

GLuint cubeVAO1, cubeVBO_position1, cubeVBO_color1, cubeEBO1;
Model cubeModel1;

void InitBufferForCube1() {
	// �ε��� ������
	std::vector<unsigned int> indices;

	for (const auto& face : cubeModel1.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	// ���� ������ ����
	std::vector<glm::vec3> vertexColor = {
		glm::vec3(0.2f, 1.0f, 0.4f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.2f, 0.6f),
		glm::vec3(0.6f, 0.2f, 1.0f),
		glm::vec3(0.2f, 0.4f, 1.0f),
		glm::vec3(1.0f, 1.0f, 0.2f),
	};

	// VAO ����
	glGenVertexArrays(1, &cubeVAO1);
	glBindVertexArray(cubeVAO1);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &cubeVBO_position1);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position1);
	glBufferData(GL_ARRAY_BUFFER, cubeModel1.vertices.size() * sizeof(Vertex), &cubeModel1.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);


	glGenBuffers(1, &cubeVBO_color1);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_color1);
	glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(glm::vec3), &vertexColor[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO ����
	glGenBuffers(1, &cubeEBO1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

GLuint cubeVAO2, cubeVBO_position2, cubeVBO_color2, cubeEBO2;
Model cubeModel2;

void InitBufferForCube2() {
	// �ε��� ������
	std::vector<unsigned int> indices;

	for (const auto& face : cubeModel2.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	// ���� ������ ����
	std::vector<glm::vec3> vertexColor = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(0.5f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.6f, 0.3f, 0.0f),

	};

	// VAO ����
	glGenVertexArrays(1, &cubeVAO2);
	glBindVertexArray(cubeVAO2);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &cubeVBO_position2);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position2);
	glBufferData(GL_ARRAY_BUFFER, cubeModel2.vertices.size() * sizeof(Vertex), &cubeModel2.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);


	glGenBuffers(1, &cubeVBO_color2);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_color2);
	glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(glm::vec3), &vertexColor[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO ����
	glGenBuffers(1, &cubeEBO2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

GLuint cylinderVAO, cylinderVBO_position, cylinderVBO_color, cylinderEBO;
Model cylinderModel;

void InitBufferForCylinder() {
	// �ε��� ������
	std::vector<unsigned int> indices;

	for (const auto& face : cylinderModel.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	// ���� ������ ����
	std::vector<glm::vec3> rainbowColors = {
	glm::vec3(1.0f, 0.6f, 0.6f),glm::vec3(1.0f, 0.6f, 0.6f),glm::vec3(1.0f, 0.6f, 0.6f),
	glm::vec3(1.0f, 0.7f, 0.7f),glm::vec3(1.0f, 0.7f, 0.7f),glm::vec3(1.0f, 0.7f, 0.7f),
	glm::vec3(1.0f, 0.8f, 0.8f),glm::vec3(1.0f, 0.8f, 0.8f),glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.9f, 0.9f),glm::vec3(1.0f, 0.9f, 0.9f),glm::vec3(1.0f, 0.9f, 0.9f),
	glm::vec3(1.0f, 0.8f, 0.8f),glm::vec3(1.0f, 0.8f, 0.8f),glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.7f, 0.7f),glm::vec3(1.0f, 0.7f, 0.7f),glm::vec3(1.0f, 0.7f, 0.7f),
	glm::vec3(1.0f, 0.6f, 0.6f),glm::vec3(1.0f, 0.6f, 0.6f),glm::vec3(1.0f, 0.6f, 0.6f),
	};

	// �� ������ ������ �������� �Ҵ�
	std::vector<glm::vec3> vertexColor;
	for (size_t i = 0; i < cylinderModel.vertices.size(); ++i) {
		int colorIndex = i % rainbowColors.size();  // �������� ��ȯ�� �� �ֵ��� �ε��� ����
		vertexColor.push_back(rainbowColors[colorIndex]);
	}

	// VAO ����
	glGenVertexArrays(1, &cylinderVAO);
	glBindVertexArray(cylinderVAO);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &cylinderVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO_position);
	glBufferData(GL_ARRAY_BUFFER, cylinderModel.vertices.size() * sizeof(Vertex), &cylinderModel.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);

	// VBO ���� (���� ������)
	glGenBuffers(1, &cylinderVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(glm::vec3), &vertexColor[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO ����
	glGenBuffers(1, &cylinderEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}



void InitOpenGL() {
	// GLEW �ʱ�ȭ
	glewExperimental = GL_TRUE;
	glewInit();

	// ���� �׽�Ʈ Ȱ��ȭ
	glEnable(GL_DEPTH_TEST);

	// ������ ������� ���� (R, G, B, A: ���� 1.0f�� �����ϸ� ���)
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // ��� ���

	// ���̴� ������
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgram = make_shaderProgram();

	// OBJ ���� �ε�
	read_obj_file("cylinder.obj", &cylinderModel);
	read_obj_file("cube.obj", &cubeModel1);
	read_obj_file("cube.obj", &cubeModel2);

	// ���� �ʱ�ȭ
	InitBufferForCylinder();
	InitBufferForCube1();
	InitBufferForCube2();
	InitBufferForAxes();  // �� ���� �ʱ�ȭ �߰�
}

GLvoid drawScene() {
	// �÷� ���ۿ� ���� ���۸� ����ϴ�
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	//---------------------------------------------------------------
	glUseProgram(shaderProgram);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform"); //--- ���� ��ȯ ����
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	//---------------------------------------------------------------

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- ���� ���� ����: fovy, aspect, near, far
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0)); //--- ������ z�� �̵�
	unsigned int projectionLocation = glGetUniformLocation(shaderProgram, "projectionTransform"); //--- ���� ��ȯ �� ����
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	//---------------------------------------------------------------

	//�⺻
	glm::mat4 Rx = glm::mat4(1.0f);
	glm::mat4 T = glm::mat4(1.0f);
	glm::mat4 Default = glm::mat4(1.0f);
	Rx = glm::rotate(Rx, glm::radians(10.0f), glm::vec3(1.0, 0.0, 0.0));
	T = glm::translate(T, glm::vec3(Tx, 0.0, 0.0));
	Default = Rx * T;
	//�߰������ ���� ��
	glm::mat4 Ry = glm::mat4(1.0f);
	glm::mat4 Middle = glm::mat4(1.0f);
	Ry = glm::rotate(Rx, glm::radians(ry), glm::vec3(0.0, 1.0, 0.0));
	Middle = Ry;
	//Cube1
	glm::mat4 S_c1 = glm::mat4(1.0f);
	glm::mat4 Cube1 = glm::mat4(1.0f);
	S_c1 = glm::scale(S_c1, glm::vec3(1.2, 0.5, 1.5));
	Cube1 = Default * S_c1;
	//Cube2
	glm::mat4 S_c2 = glm::mat4(1.0f);
	glm::mat4 T_c2 = glm::mat4(1.0f);
	glm::mat4 Cube2 = glm::mat4(1.0f);
	S_c2 = glm::scale(S_c2, glm::vec3(0.6, 0.4, 0.75));
	T_c2 = glm::translate(T_c2, glm::vec3(0.0, 0.5, 0.0));
	Cube2 = Default * S_c2 * Middle * T_c2;
	//Cylinder1
	glm::mat4 S_s1 = glm::mat4(1.0f);
	glm::mat4 Tx_s1 = glm::mat4(1.0f);
	glm::mat4 Ty_s1 = glm::mat4(1.0f);
	glm::mat4 Rz_s1 = glm::mat4(1.0f);
	glm::mat4 Cylinder1 = glm::mat4(1.0f);
	S_s1 = glm::scale(S_s1, glm::vec3(0.25, 0.6, 0.25));
	Tx_s1 = glm::translate(Tx_s1, glm::vec3(0.1, 0.3, 0.0));
	Ty_s1 = glm::translate(Ty_s1, glm::vec3(0.0, 0.1, 0.0));
	Rz_s1 = glm::rotate(Rz_s1, glm::radians(GLfloat(rz12)), glm::vec3(0.0, 0.0, 1.0));
	Cylinder1 = Default * Middle * Tx_s1 * Rz_s1 * Ty_s1 * S_s1;
	//Cylinder2
	glm::mat4 S_s2 = glm::mat4(1.0f);
	glm::mat4 Tx_s2 = glm::mat4(1.0f);
	glm::mat4 Ty_s2 = glm::mat4(1.0f);
	glm::mat4 Rz_s2 = glm::mat4(1.0f);
	glm::mat4 Cylinder2 = glm::mat4(1.0f);
	S_s2 = glm::scale(S_s2, glm::vec3(0.25, 0.6, 0.25));
	Tx_s2 = glm::translate(Tx_s2, glm::vec3(-0.1, 0.3, 0.0));
	Ty_s2 = glm::translate(Ty_s2, glm::vec3(0.0, 0.1, 0.0));
	Rz_s2 = glm::rotate(Rz_s2, glm::radians(GLfloat(-rz12)), glm::vec3(0.0, 0.0, 1.0));
	Cylinder2 = Default * Middle * Tx_s2 * Rz_s2 * Ty_s2 * S_s2;
	//Cylinder3
	glm::mat4 Rx_s3 = glm::mat4(1.0f);
	glm::mat4 S_s3 = glm::mat4(1.0f);
	glm::mat4 T_s3 = glm::mat4(1.0f);
	glm::mat4 Ry_s3 = glm::mat4(1.0f);
	glm::mat4 Cylinder3 = glm::mat4(1.0f);
	Rx_s3 = glm::rotate(Rx_s3, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	S_s3 = glm::scale(S_s3, glm::vec3(0.25, 1.2, 0.25));
	T_s3 = glm::translate(T_s3, glm::vec3(tx34, 0.4, 0.2));
	Ry_s3 = glm::rotate(Ry_s3, glm::radians(GLfloat(ry34)), glm::vec3(0.0, 1.0, 0.0));
	Cylinder3 = Default * Ry_s3 * Rx_s3 * S_s3 * T_s3;
	//Cylinder4
	glm::mat4 Rx_s4 = glm::mat4(1.0f);
	glm::mat4 S_s4 = glm::mat4(1.0f);
	glm::mat4 T_s4 = glm::mat4(1.0f);
	glm::mat4 Ry_s4 = glm::mat4(1.0f);
	glm::mat4 Cylinder4 = glm::mat4(1.0f);
	Rx_s4 = glm::rotate(Rx_s4, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	S_s4 = glm::scale(S_s4, glm::vec3(0.25, 1.2, 0.25));
	T_s4 = glm::translate(T_s4, glm::vec3(-tx34, 0.4, 0.2));
	Ry_s4 = glm::rotate(Ry_s4, glm::radians(GLfloat(-ry34)), glm::vec3(0.0, 1.0, 0.0));
	Cylinder4 = Default * Ry_s4 * Rx_s4 * S_s4 * T_s4;

	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Rx));

	drawAxes();

	glBindVertexArray(cubeVAO1);

	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Cube1));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glBindVertexArray(cubeVAO2);

	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Cube2));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	glBindVertexArray(0);

	glBindVertexArray(cylinderVAO);

	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Cylinder1));
	glDrawElements(GL_TRIANGLES, 204, GL_UNSIGNED_INT, 0);

	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Cylinder2));
	glDrawElements(GL_TRIANGLES, 204, GL_UNSIGNED_INT, 0);

	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Cylinder3));
	glDrawElements(GL_TRIANGLES, 204, GL_UNSIGNED_INT, 0);

	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Cylinder4));
	glDrawElements(GL_TRIANGLES, 204, GL_UNSIGNED_INT, 0);

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
	if (isAnimating) {
		cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPos, 1.0f);
		glutPostRedisplay();
	}
	if (b)
		Tx += 0.05f;
	else if (B)
		Tx -= 0.05f;
	if (m)
		ry += 5;
	else if (M)
		ry -= 5;
	if (f) {
		ry34 += dy34;
		if (ry34 >= 360)
			ry34 = ry34 % 360;
	}
	else if (F) {
		ry34 -= dy34;
		if (ry34 <= -360)
			ry34 = ry34 % 360;
	}
	if (e) {
		if (ry34 != 0) {
			ry34 += dy34;
			if (ry34 >= 360)
				ry34 = ry34 % 360;
		}
		else {
			if (int(tx34) != 0) {
				tx34 -= 0.05f;
			}
			else {
				tx34 = 0;
				e = false;
			}
		}
	}
	else if (E) {
		if (ry34 != 0) {
			ry34 += dy34;
			if (ry34 >= 360)
				ry34 = ry34 % 360;
		}
		else {
			if (int(tx34) != 1) {
				tx34 += 0.05f;
			}
			else {
				tx34 = 1;
				E = false;
			}
		}
	}
	if (t) {
		rz12 += dz12;
		if (rz12 == 90 || rz12 == -90)
			dz12 *= -1;
	}

	glutPostRedisplay();
	glutTimerFunc(60, TimerFunction, 1);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'z':
		cameraPos.z += 0.1;
		break;
	case 'Z':
		cameraPos.z -= 0.1;
		break;
	case 'x':
		cameraPos.x += 0.1f;
		break;
	case 'X':
		cameraPos.x -= 0.1f;
		break;
	case 'y':
		cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPos, 1.0f);
		break;
	case 'Y':
		cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPos, 1.0f);
		break;
	case 'r':
		cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPos, 1.0f);
		break;
	case 'R':
		cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPos, 1.0f);
		break;
	case 'a':
	case 'A':
		isAnimating = !isAnimating;  // �ִϸ��̼� ���� ����
		break;
	case'b':
		b = !b;
		break;
	case'B':
		B = !B;
		break;
	case'm':
		m = !m;
		break;
	case'M':
		M = !M;
		break;
	case'f':
		f = !f;
		break;
	case 'F':
		F = !F;
		break;
	case 'e':
		e = !e;
		break;
	case 'E':
		E = !E;
		break;
	case 't':
	case'T':
		t = !t;
		break;
	case'c':
	case'C':
		 Tx = 0.0f;  ry = 0.0f;  ry34 = 0;  dy34 = 5;
		 tx34 = 1.0f;  rz12 = 0;  dz12 = 5;
	case 's':
	case'S':
		isAnimating = false;
		b = false;  B = false;
		m = false;  M = false;
		f = false;  F = false;
		e = false;  E = false;
		t = false;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}


	//InitOpenGL();
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