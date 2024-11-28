#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <gl/glew.h>
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
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


GLfloat lightR = 1.0f; GLfloat lightG = 1.0f; GLfloat lightB = 1.0f;
bool LightOn = true; bool lightMove = false;
GLfloat lightX = 0.0f; GLfloat lightZ = 1.0f; int lightangle = 0;
//----------------------------------
#define MAX_LINE_LENGTH 1024
#define Pi 3.141592f

typedef struct {
	float x, y, z;   // ��ġ
	float nx, ny, nz; // ���� ���� �߰�
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
	std::vector<glm::vec3> normals; // ���� ���� �ӽ� ����
	std::vector<glm::vec2> texCoords; // �ؽ�ó ��ǥ �ӽ� ����

	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'v' && line[1] == ' ') {
			// ���ؽ� ��ǥ �б�
			Vertex vertex;
			sscanf_s(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			model->vertices.push_back(vertex);
		}
		else if (line[0] == 'v' && line[1] == 'n') {
			// ���� ���� �б�
			glm::vec3 normal;
			sscanf_s(line + 3, "%f %f %f", &normal.x, &normal.y, &normal.z);
			normals.push_back(normal);
		}
		else if (line[0] == 'v' && line[1] == 't') {
			// �ؽ�ó ��ǥ �б�
			glm::vec2 texCoord;
			sscanf_s(line + 3, "%f %f", &texCoord.x, &texCoord.y);
			texCoords.push_back(texCoord);
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			// �� ������ �б� (v/vt/vn ���� ó��)
			unsigned int v[4], vt[4], vn[4];
			int matches = sscanf_s(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u",
				&v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2], &vn[2], &v[3], &vt[3], &vn[3]);

			if (matches == 9 || matches == 12) {
				// �ﰢ�� Ȥ�� �簢�� ó��
				Face face;
				face.v1 = v[0] - 1;
				face.v2 = v[1] - 1;
				face.v3 = v[2] - 1;
				model->faces.push_back(face);

				Face face2;
				if (matches == 12) {
					// �簢���� ���, �� ��° �ﰢ�� ����
					face2.v1 = v[0] - 1;
					face2.v2 = v[2] - 1;
					face2.v3 = v[3] - 1;
					model->faces.push_back(face2);
				}

				// ���� ���� ����
				model->vertices[face.v1].nx = normals[vn[0] - 1].x;
				model->vertices[face.v1].ny = normals[vn[0] - 1].y;
				model->vertices[face.v1].nz = normals[vn[0] - 1].z;

				model->vertices[face.v2].nx = normals[vn[1] - 1].x;
				model->vertices[face.v2].ny = normals[vn[1] - 1].y;
				model->vertices[face.v2].nz = normals[vn[1] - 1].z;

				model->vertices[face.v3].nx = normals[vn[2] - 1].x;
				model->vertices[face.v3].ny = normals[vn[2] - 1].y;
				model->vertices[face.v3].nz = normals[vn[2] - 1].z;

				if (matches == 12) {
					model->vertices[face2.v1].nx = normals[vn[0] - 1].x;
					model->vertices[face2.v1].ny = normals[vn[0] - 1].y;
					model->vertices[face2.v1].nz = normals[vn[0] - 1].z;

					model->vertices[face2.v2].nx = normals[vn[2] - 1].x;
					model->vertices[face2.v2].ny = normals[vn[2] - 1].y;
					model->vertices[face2.v2].nz = normals[vn[2] - 1].z;

					model->vertices[face2.v3].nx = normals[vn[3] - 1].x;
					model->vertices[face2.v3].ny = normals[vn[3] - 1].y;
					model->vertices[face2.v3].nz = normals[vn[3] - 1].z;
				}
			}
		}
	}
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

GLuint cubeVAO, cubeVBO_position, cubeVBO_color, cubeEBO;
Model cubeModel;

void InitBufferForCube() {
	// �ε��� ������
	std::vector<unsigned int> indices;

	for (const auto& face : cubeModel.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	std::vector<glm::vec3> vertexColors;
	for (size_t i = 0; i < cubeModel.vertices.size(); ++i) {
		// ������ ���� ���� (�ܼ��� �� �������� �ٸ� ������ �ֱ� ����)
		vertexColors.push_back(glm::vec3(
			static_cast<float>(i % 3 == 0),
			static_cast<float>((i + 1) % 3 == 0),
			static_cast<float>((i + 2) % 3 == 0)
		));
	}

	// VAO ����
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &cubeVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position);
	glBufferData(GL_ARRAY_BUFFER, cubeModel.vertices.size() * sizeof(Vertex), &cubeModel.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(pAttribute);

	// ���� VBO ����
	GLint nAttribute = glGetAttribLocation(shaderProgram, "vNormal");
	glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
	glEnableVertexAttribArray(nAttribute);

	// ���� VBO ����
	glGenBuffers(1, &cubeVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO ����
	glGenBuffers(1, &cubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
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

	std::vector<glm::vec3> vertexColors;
	for (size_t i = 0; i < cylinderModel.vertices.size(); ++i) {
		// ������ ���� ���� (�ܼ��� �� �������� �ٸ� ������ �ֱ� ����)
		vertexColors.push_back(glm::vec3(
			static_cast<float>(i % 3 == 0),
			static_cast<float>((i + 1) % 3 == 0),
			static_cast<float>((i + 2) % 3 == 0)
		));
	}

	// VAO ����
	glGenVertexArrays(1, &cylinderVAO);
	glBindVertexArray(cylinderVAO);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &cylinderVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO_position);
	glBufferData(GL_ARRAY_BUFFER, cylinderModel.vertices.size() * sizeof(Vertex), &cylinderModel.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(pAttribute);

	// ���� VBO ����
	GLint nAttribute = glGetAttribLocation(shaderProgram, "vNormal");
	glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
	glEnableVertexAttribArray(nAttribute);

	// ���� VBO ����
	glGenBuffers(1, &cylinderVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
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

	// ���̴� ������
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgram = make_shaderProgram();

	// OBJ ���� �ε�
	read_obj_file("cylinder.obj", &cylinderModel);
	read_obj_file("cube.obj", &cubeModel);

	// ���� �ʱ�ȭ
	InitBufferForCylinder();
	InitBufferForCube();
	InitBufferForAxes();  // �� ���� �ʱ�ȭ �߰�
}

GLvoid drawScene() {
	// �÷� ���ۿ� ���� ���۸� ����ϴ�
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	//---------------------------------------------------------------
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

	//-----------------
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, lightX, 0.2, lightZ);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
	glUniform3f(lightColorLocation, lightR, lightG, lightB);
	unsigned int objColorLocation = glGetUniformLocation(shaderProgram, "objectColor"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��
	glUniform3f(objColorLocation, 1.0, 0.5, 0.7);
	if (LightOn) {
		unsigned int objColorLocation = glGetUniformLocation(shaderProgram, "ambientLight"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��
		glUniform3f(objColorLocation, 1.0, 0.5, 0.7);
	}
	else {
		unsigned int objColorLocation = glGetUniformLocation(shaderProgram, "ambientLight"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��
		glUniform3f(objColorLocation, 0.0,0.0,0.0);
	}
	//------------------

	glBindVertexArray(cubeVAO);

	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Cube1));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glUniform3f(objColorLocation, 1.0, 0.2, 0.4);

	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Cube2));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	glBindVertexArray(0);

	glBindVertexArray(cylinderVAO);

	glUniform3f(objColorLocation, 1.0, 0.4, 0.6);

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


	if (lightMove) {
		lightangle += 3;
		lightX = cos(lightangle * Pi / 180) * 1.0f;
		lightZ = sin(lightangle * Pi / 180) * 1.0f;
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
		lightMove = true;
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
		//m = !m;
		LightOn = !LightOn;
		break;
	case'M':
		//M = !M;
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
		lightR = rand() % 11 / 10.0f;
		lightG = rand() % 11 / 10.0f;
		lightB = rand() % 11 / 10.0f;
		break;
	case'C':
		Tx = 0.0f;  ry = 0.0f;  ry34 = 0;  dy34 = 5;
		tx34 = 1.0f;  rz12 = 0;  dz12 = 5;
		break;
	case 's':
		lightMove = false;
		break;
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