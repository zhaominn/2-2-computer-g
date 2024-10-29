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
GLfloat Xangle = 30.0f; GLfloat Yangle = 30.0f;
int shape1_num = 0; int shape2_num = 3;

GLfloat rotation1_X = 0.0f; GLfloat rotation_dx = 2.0f;
GLfloat rotation1_y = 0.0f; GLfloat rotation_dy = 2.0f;
GLfloat rotation2_X = 0.0f;
GLfloat rotation2_y = 0.0f;
GLfloat revolution_y = 0.0f;
GLfloat revolution_x = 0.0f;
GLfloat revolution_x1 = 0.0f;
bool Rotation_X = false;
bool Rotation_Y = false;
int rotationNum = 3;
bool revolution = false; GLfloat revolutuion_dy = 1.0f;


GLfloat scale_aNum = 1.0f;
GLfloat scale_oNum = 1.0f;
GLfloat scale_d = 0.01f;
int moveMode = -1;
int transMode = -1;
GLfloat Tx = 0.5f;
GLfloat Tx1 = 0.0f;
GLfloat Tx2 = 0.0f;
GLfloat Tdx = 0.01f;
GLfloat Ty1 = 0.0f;
GLfloat Ty2 = 0.0f;
GLfloat Tz1 = 0.0f;
GLfloat Tz2 = 0.0f;
GLfloat dz = 0.01f;
GLfloat dy = 0.01f;
GLfloat dx = 0.01f;
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

	glUseProgram(shaderID);
	return shaderID;
}

GLuint cubeVAO, cubeVBO_position, cubeVBO_color, cubeEBO;
GLuint pyramidVAO, pyramidVBO_position, pyramidVBO_color, pyramidEBO;
GLuint cylinderVAO, cylinderVBO_position, cylinderVBO_color, cylinderEBO;
GLuint coneVAO, coneVBO_position, coneVBO_color, coneEBO;
GLuint shaderProgram;

Model cubeModel, pyramidModel, cylinderModel, coneModel;

// ������ü�� �Ƕ�̵��� �ε��� �迭 �߰�

// ������ü�� ���� �ʱ�ȭ �Լ�
void InitBufferForCube() {
	// �ε��� ������

	std::vector<unsigned int> indices = {
		0, 1, 2, 3,  // �ո�
		4, 5, 6, 7,  // �޸�
		0, 3, 7, 4,  // ���ʸ�
		1, 2, 6, 5,  // �����ʸ�
		3, 2, 6, 7,  // ���ʸ�
		0, 4, 5, 1   // �Ʒ��ʸ�
	};


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

// �Ƕ�̵��� ���� �ʱ�ȭ �Լ�
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

void InitBufferForCylinder() {
	std::vector<glm::vec3> vertexColor = {
		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),

		glm::vec3(1.0f, 0.9f, 0.9f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.2f, 0.2f),

		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),

		glm::vec3(1.0f, 0.9f, 0.9f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.2f, 0.2f),

		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),

		glm::vec3(1.0f, 0.9f, 0.9f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.2f, 0.2f),

		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),

		glm::vec3(1.0f, 0.9f, 0.9f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.2f, 0.2f),

		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),

		glm::vec3(1.0f, 0.9f, 0.9f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.2f, 0.2f),
	};

	std::vector<unsigned int> indices;

	for (const auto& face : pyramidModel.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}
	glGenVertexArrays(1, &cylinderVAO);
	glBindVertexArray(cylinderVAO);

	glGenBuffers(1, &cylinderVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO_position);
	glBufferData(GL_ARRAY_BUFFER, cylinderModel.vertices.size() * sizeof(Vertex), &cylinderModel.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);

	glGenBuffers(1, &cylinderVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(glm::vec3), &vertexColor[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);

	glGenBuffers(1, &cylinderEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void InitBufferForCone() {
	std::vector<glm::vec3> vertexColor = {
		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),

		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),

		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),

		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),

		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),

		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),

		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),

		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.3f, 0.3f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.9f, 0.9f),
	};
	std::vector<unsigned int> indices;

	for (const auto& face : pyramidModel.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	glGenVertexArrays(1, &coneVAO);
	glBindVertexArray(coneVAO);

	glGenBuffers(1, &coneVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, coneVBO_position);
	glBufferData(GL_ARRAY_BUFFER, coneModel.vertices.size() * sizeof(Vertex), &coneModel.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);

	glGenBuffers(1, &coneVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, coneVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(glm::vec3), &vertexColor[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);

	glGenBuffers(1, &coneEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coneEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

GLuint axisVAO, axisVBO_position, axisVBO_color;

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

GLUquadric* quadric;

void InitOpenGL() {
	// GLEW �ʱ�ȭ
	glewExperimental = GL_TRUE;
	glewInit();

	// ���� �׽�Ʈ Ȱ��ȭ
	glEnable(GL_DEPTH_TEST);

	// ����� �׸� �� �ֵ��� Cull Face ��Ȱ��ȭ
	glDisable(GL_CULL_FACE);

	// ������ ������� ���� (R, G, B, A: ���� 1.0f�� �����ϸ� ���)
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // ��� ���

	// ���̴� ������
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgram = make_shaderProgram();

	// OBJ ���� �ε�
	read_obj_file("cube.obj", &cubeModel);
	read_obj_file("pyramid.obj", &pyramidModel);
	read_obj_file("cylinder.obj", &cylinderModel);
	read_obj_file("cone.obj", &coneModel);

	// ���� �ʱ�ȭ
	InitBufferForCube();
	InitBufferForPyramid();
	InitBufferForCylinder();  // ����� �ʱ�ȭ �߰�
	InitBufferForCone();      // ���� �ʱ�ȭ �߰�
	InitBufferForAxes();  // �� ���� �ʱ�ȭ �߰�
}

void drawAxes() {
	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 6);  // 6���� ������ �׸��� (�� ��� 2���� ����)
	glBindVertexArray(0);
}

void drawConeWithGLU() {

	// GLU ���� �׸���
	quadric = gluNewQuadric();
	glPushMatrix();
	gluQuadricDrawStyle(quadric, GLU_LINE);
	gluCylinder(quadric, 0.1f, 0.0f, 0.2f, 20, 20); // �ظ� ������ 0.2, ���� ������ 0.0 (����), ���� 0.5
	glPopMatrix();
}

void drawCylinderWithGLU() {

	// GLU ����� �׸���
	quadric = gluNewQuadric();
	glPushMatrix();
	gluQuadricDrawStyle(quadric, GLU_LINE);
	gluCylinder(quadric, 0.1f, 0.2f, 0.2f, 20, 20); // �ظ� ������ 0.2, ���� ������ 0.2, ���� 0.5
	glPopMatrix();
}

GLvoid drawScene() {
	// �÷� ���ۿ� ���� ���۸� ����ϴ�
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Y�� ȸ��

	// �� ��ȯ ����� ���̴��� ����
	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// �� �׸���
	drawAxes();

	// �̵� ��
	{
		modelMatrix = glm::rotate(modelMatrix, glm::radians(revolution_x), glm::vec3(1.0f, 0.0f, 0.0f)); // X�� ȸ��
		modelMatrix = glm::rotate(modelMatrix, glm::radians(revolution_y), glm::vec3(0.0f, 1.0f, 0.0f)); // Y�� ȸ��

		modelMatrix = glm::rotate(modelMatrix, glm::radians(revolution_x1), glm::vec3(1.0f, 0.0f, 0.0f)); // X�� ȸ��

		modelMatrix = glm::scale(modelMatrix, glm::vec3(scale_oNum, scale_oNum, scale_oNum));
	}

	modelMatrix = glm::translate(modelMatrix, glm::vec3(Tx, Ty1, Tz1)); // X�� �̵�

	modelMatrix = glm::translate(modelMatrix, glm::vec3(Tx1, 0.0f, 0.0f)); // X�� �̵�
	//�̵� ��
	{
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation1_X), glm::vec3(1.0f, 0.0f, 0.0f)); // X�� ȸ��
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation1_y), glm::vec3(0.0f, 1.0f, 0.0f)); // Y�� ȸ��

		modelMatrix = glm::scale(modelMatrix, glm::vec3(scale_aNum, scale_aNum, scale_aNum));
	}

	// ��ȯ ����� ���̴��� ����
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	// �� �׸���
	drawAxes();
	// ���� 1
	switch (shape1_num) {
	case 0: // ������ü
		for (int i = 0; i < 6; i++) {
			glBindVertexArray(cubeVAO);
			glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 4 * i));
		}
		break;
	case 1: // �Ƕ�̵�
		glBindVertexArray(pyramidVAO);
		glDrawElements(GL_TRIANGLES, 13, GL_UNSIGNED_INT, 0);
		break;
	case 2: // GLU �����
		drawCylinderWithGLU();
		break;
	case 3: // GLU ����
		drawConeWithGLU();
		break;
	}

	modelMatrix = glm::mat4(1.0f);
	//�̵� ��
	{
		modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X�� ȸ��
		modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Y�� ȸ��

		modelMatrix = glm::rotate(modelMatrix, glm::radians(revolution_x), glm::vec3(1.0f, 0.0f, 0.0f)); // X�� ȸ��
		modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f + revolution_y), glm::vec3(0.0f, 1.0f, 0.0f)); // Y�� ȸ��

		modelMatrix = glm::rotate(modelMatrix, glm::radians(revolution_x1), glm::vec3(1.0f, 0.0f, 0.0f)); // X�� ȸ��

		modelMatrix = glm::scale(modelMatrix, glm::vec3(scale_oNum, scale_oNum, scale_oNum));
	}
	modelMatrix = glm::translate(modelMatrix, glm::vec3(Tx, Ty2, Tz2)); // X�� �̵�

	modelMatrix = glm::translate(modelMatrix, glm::vec3(Tx2, 0.0f, 0.0f)); // X�� �̵�
	//�̵� ��
	{
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation2_X), glm::vec3(1.0f, 0.0f, 0.0f)); // X�� ȸ��
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation2_y), glm::vec3(0.0f, 1.0f, 0.0f)); // Y�� ȸ��

		modelMatrix = glm::scale(modelMatrix, glm::vec3(scale_aNum, scale_aNum, scale_aNum));
	}
	// �� ��ȯ ����� ���̴��� ����
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	// �� �׸���
	drawAxes();
	switch (shape2_num) {
	case 0: // ������ü
		for (int i = 0; i < 6; i++) {
			glBindVertexArray(cubeVAO);
			glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 4 * i));
		}
		break;
	case 1: // �Ƕ�̵�
		glBindVertexArray(pyramidVAO);
		glDrawElements(GL_TRIANGLES, 13, GL_UNSIGNED_INT, 0);
		break;
	case 2: // GLU �����
		drawCylinderWithGLU();
		break;
	case 3: // GLU ����
		drawConeWithGLU();
		break;
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
	switch (moveMode) {
	case 1:
		revolution_y += revolutuion_dy;
		Tx += 0.005f;
		break;
	case 2:
		Tx += Tdx;
		if (Tx < -0.5f || Tx > 0.5f)
			Tdx *= -1;
		break;
	case 3:
		revolution_y += revolutuion_dy;
		break;
	case 4:
		//revolution_x += 2.0f;
		//Tz1 += dz;
		//Tz2 += dz;
		Ty1 += dy;
		Ty2 -= dy;
		Tx1 -= dx;
		Tx2 -= dx;
		if (Ty1 >= 0.5f)
			dy = -0.01f;
		if (Tx1 <= -1.0f)
			dx = -0.01f;
		if (Ty1 <= -0.5f)
			dy = 0.01f;
		if (Tx1 >= 0.0f)
			dx = 0.01f;
		break;
	case 5:
		revolution_y += revolutuion_dy;
		rotation1_y += rotation_dy;
		rotation2_y += rotation_dy;
		//rotation1_X += rotation_dx;
		//rotation2_X += rotation_dx;
		scale_aNum += scale_d;
		if (scale_aNum > 2.0f || scale_aNum < 0.1f)
			scale_d *= -1;
		break;
	}

	InitOpenGL();
	glutPostRedisplay(); // ȭ�� ����
	glutTimerFunc(30, TimerFunction, 1); // Ÿ�̸� �Լ� ȣ��
}

void reset() {
	shape1_num = 3; shape2_num = 0;
	rotation1_X = 0.0f;
	rotation1_y = 0.0f;
	rotation2_X = 0.0f;
	rotation2_y = 0.0f;
	revolution_y = 0.0f;
	revolution_x = 0.0f;
	Rotation_X = false;
	Rotation_Y = false;
	rotationNum = 3;
	revolution = false;
	scale_aNum = 1.0f;
	scale_oNum = 1.0f;
	scale_d = 0.01f;
	moveMode = -1;
	Tx = 0.5f;
	Tx1 = 0.0f;
	Tx2 = 0.0f;
	Ty1 = 0.0f;
	Ty2 = 0.0f;
	Tz1 = 0.0f;
	Tz2 = 0.0f;
	dz = 0.01f;
	dy = 0.01f;
	dx = 0.01f;
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		rotationNum = 1;
		break;
	case '2':
		rotationNum = 2;
		break;
	case '3':
		rotationNum = 3;
		break;
	case 'x':
		Rotation_X = true;
		rotation_dx = 2.0f;
		switch (rotationNum) {
		case 1:
			rotation1_X += rotation_dx;
			break;
		case 2:
			rotation2_X += rotation_dx;
			break;
		case 3:
			rotation1_X += rotation_dx;
			rotation2_X += rotation_dx;
			break;
		}
		break;
	case'X':
		Rotation_X = true;
		rotation_dx = -2.0f;
		switch (rotationNum) {
		case 1:
			rotation1_X += rotation_dx;
			break;
		case 2:
			rotation2_X += rotation_dx;
			break;
		case 3:
			rotation1_X += rotation_dx;
			rotation2_X += rotation_dx;
			break;
		}
		break;
	case'y':
		Rotation_Y = true;
		rotation_dy = 2.0f;
		switch (rotationNum) {
		case 1:
			rotation1_y += rotation_dy;
			break;
		case 2:
			rotation2_y += rotation_dy;
			break;
		case 3:
			rotation1_y += rotation_dy;
			rotation2_y += rotation_dy;
			break;
		}
		break;
	case'Y':
		Rotation_Y = true;
		rotation_dy = -2.0f;
		switch (rotationNum) {
		case 1:
			rotation1_y += rotation_dy;
			break;
		case 2:
			rotation2_y += rotation_dy;
			break;
		case 3:
			rotation1_y += rotation_dy;
			rotation2_y += rotation_dy;
			break;
		}
		break;
	case'r':
		revolution = true;
		revolutuion_dy = 1.0f;
		revolution_y += revolutuion_dy;
		break;
	case'R':
		revolution = true;
		revolutuion_dy = -1.0f;
		revolution_y += revolutuion_dy;
		break;
	case'c':
		shape1_num = rand() % 4;
		shape2_num = rand() % 4;
		break;
	case's':
		reset();
		break;
	case 'a':
		scale_aNum -= 0.05f;
		break;
	case 'A':
		scale_aNum += 0.05f;
		break;
	case 'o':
		scale_oNum -= 0.05f;
		break;
	case 'O':
		scale_oNum += 0.05f;
		break;
	case '4':
		//x�� �̵�
		transMode = 4;
		break;
	case '5':
		//y�� �̵�
		transMode = 5;
		break;
	case '6':
		//z�� �̵�
		transMode = 6;
		break;
	case '7':
		//xz��鿡�� �̵� or ���� �̵�
		transMode = 7;
		break;
	case'g':
		//��鿡 �����̷��� �׸���, �� �����̷� ��ġ�� ���� ��ü �̵� �ִϸ��̼�
		reset();
		Tx = 0.0f;
		revolutuion_dy = 1.0f;
		moveMode = 1;
		break;
	case 'h':
		//�� ������ ������ ����ϸ� ������ �ڸ��� �̵��ϴ� �ִϸ��̼�
		reset();
		revolution_y = 90.0f;
		moveMode = 2;
		break;
	case 'j':
		//�� ������ �����ϸ鼭 ������ �ڸ��� �̵��ϴ� �ִϸ��̼�
		reset();
		revolutuion_dy = 1.0f;
		moveMode = 3;
		break;
	case 'k':
		//�� ������ �� ���� ����, �ٸ� ������ �Ʒ��� �̵��ϸ鼭 ������ �ڸ��� �̵��ϴ� �ִϸ��̼�
		reset();
		revolution_y = 90.0f;
		moveMode = 4;
		break;
	case'l':
		//�� ������ �� ���� Ȯ��, �ٸ� �� ���� ��ҵǸ� ������ �����ϱ�
		rotation_dy = 4.0f;
		rotation_dx = 4.0f;
		moveMode = 5;
		break;
	}

	glutPostRedisplay();  // ȭ���� �����մϴ�.
}

GLvoid SpecialKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		switch (transMode) {
		case 4:
			revolution_x = 0.0f;
			revolution_y = 0.0f;
			Tx += 0.05f;
			break;
		case 5:
			revolution_x = 0.0f;
			revolution_x1 = 90.0f;
			revolution_y = 0.0f;
			Tz1 += 0.05f;
			Tz2 += 0.05f;
			break;
		case 6:
			revolution_x = 0.0f;
			revolution_y = 90.0f;
			Tx += 0.05f;
			break;
		case 7:
			revolution_x = 0.0f;
			revolution_y = 0.0f;
			Tx += 0.05f;
			break;
		}
		break;
	case GLUT_KEY_DOWN:
		switch (transMode) {
		case 4:
			revolution_x = 0.0f;
			revolution_y = 0.0f;
			Tx -= 0.05f;
			break;
		case 5:
			revolution_x = 0.0f;
			revolution_x1 = 90.0f;
			revolution_y = 0.0f;
			Tz1 -= 0.05f;
			Tz2 -= 0.05f;
			break;
		case 6:
			revolution_x = 0.0f;
			revolution_y = 90.0f;
			Tx -= 0.05f;
			break;
		case 7:
			revolution_x = 0.0f;
			revolution_y = 0.0f;
			Tx -= 0.05f;
			break;
		}
		break;
	case GLUT_KEY_LEFT:
		switch (transMode) {
		case 7:
			revolution_x = 0.0f;
			revolution_x1 = 90.0f;
			revolution_y = 0.0f;
			Ty1 += 0.05f;
			Ty2 += 0.05f;
			break;
		}
		break;
	case GLUT_KEY_RIGHT:
		switch (transMode) {
		case 7:
			revolution_x = 0.0f;
			revolution_x1 = 90.0f;
			revolution_y = 0.0f;
			Ty1 -= 0.05f;
			Ty2 -= 0.05f;
			break;
		}
		break;
	}

	glutPostRedisplay();  // ȭ���� �����մϴ�.
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

	glutDisplayFunc(drawScene);   // ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape);     // �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard);   // �Ϲ� Ű �Է� ó��
	glutSpecialFunc(SpecialKey);  // Ư�� Ű �Է� ó�� (����Ű ��)
	glutTimerFunc(30, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	glutMainLoop();               // �̺�Ʈ ó�� ����
}


