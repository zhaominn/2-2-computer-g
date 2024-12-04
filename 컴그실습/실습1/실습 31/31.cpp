#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <gl/glew.h>
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "gl/stb_image.h"

int widthImage, heightImage, numberOfChannel;
unsigned int texture1; unsigned int texture2; unsigned int texture3;

//----------------------------------
bool c = true; bool moveX = false; bool moveY = false;
GLfloat transX = 0.0f; GLfloat transY = 0.0f;
GLfloat Xangle = 30.0f; GLfloat Yangle = 30.0f;
int dx = 1; int dy = 1; int Lightangle = 0;
bool m = true; GLfloat z = 10;
//----------------------------------
#define MAX_LINE_LENGTH 1024

typedef struct {
	float x, y, z;   // ��ġ
	float nx, ny, nz; // ���� ���� �߰�
	float tx, ty;     // �ؽ�ó ��ǥ
} Vertex;

typedef struct {
	unsigned int v1, v2, v3;    // ���ؽ� �ε���
	unsigned int t1, t2, t3;    // �ؽ�ó ��ǥ �ε���
} Face;

typedef struct {
	std::vector<Vertex> vertices;
	std::vector<Face> faces;
	std::vector<glm::vec2> texCoords; // �ؽ�ó ��ǥ
} Model;

Model objModel; // �� �����͸� ������ ���� ����

float vertexData1[] = {
		-0.5f, -0.5f, -0.5f,	1.0, 0.0, 0.0,			1.0, 0.0,		// ����
		 0.5f,  0.5f, -0.5f,	1.0, 0.0, 0.0,			0.0, 1.0,
		 0.5f, -0.5f, -0.5f,	1.0, 0.0, 0.0,			0.0, 0.0,

		 0.5f,  0.5f, -0.5f,	1.0, 0.0, 0.0,			0.0, 1.0,
		-0.5f, -0.5f, -0.5f,	1.0, 0.0, 0.0,			1.0, 1.0,
		-0.5f,  0.5f, -0.5f,	1.0, 0.0, 0.0,			0.0, 1.0,

		-0.5f,  0.5f,  0.5f,	0.0, 0.0, 1.0,			1.0, 1.0,		// ����
		-0.5f,  0.5f, -0.5f,	0.0, 0.0, 1.0,			0.0, 1.0,
		-0.5f, -0.5f, -0.5f,	0.0, 0.0, 1.0,			0.0, 0.0,

		-0.5f, -0.5f, -0.5f,	0.0, 0.0, 1.0,			0.0, 0.0,
		-0.5f, -0.5f,  0.5f,	0.0, 0.0, 1.0,			1.0, 0.0,
		-0.5f,  0.5f,  0.5f,	0.0, 0.0, 1.0,			0.0, 1.0,

		 0.5f,  0.5f,  0.5f,	1.0, 1.0, 0.0,			0.0, 1.0, // ������
		 0.5f, -0.5f, -0.5f,	1.0, 1.0, 0.0,			1.0, 0.0,
		 0.5f,  0.5f, -0.5f,	1.0, 1.0, 0.0,			0.0, 1.0,

		 0.5f, -0.5f, -0.5f,	1.0, 1.0, 0.0,			1.0, 0.0,
		 0.5f,  0.5f,  0.5f,	1.0, 1.0, 0.0,			0.0, 1.0,
		 0.5f, -0.5f,  0.5f,	1.0, 1.0, 0.0,			0.0, 0.0,


		-0.5f, -0.5f, -0.5f,	1.0, 0.0, 1.0,			0.0, 0.0,	// �Ʒ���
		 0.5f, -0.5f, -0.5f,	1.0, 0.0, 1.0,			1.0, 0.0,
		 0.5f, -0.5f,  0.5f,	1.0, 0.0, 1.0,			0.0, 1.0,

		 0.5f, -0.5f,  0.5f,	1.0, 0.0, 1.0,			0.0, 1.0,
		-0.5f, -0.5f,  0.5f,	1.0, 0.0, 1.0,			0.0, 1.0,
		-0.5f, -0.5f, -0.5f,	1.0, 0.0, 1.0,			0.0, 0.0,


		-0.5f,  0.5f, -0.5f,	0.0, 1.0, 1.0,			0.0, 1.0,	// ����
		 0.5f,  0.5f,  0.5f,	0.0, 1.0, 1.0,			1.0, 0.0,
		 0.5f,  0.5f, -0.5f,	0.0, 1.0, 1.0,			1.0, 1.0,

		 0.5f,  0.5f,  0.5f,	0.0, 1.0, 1.0,			1.0, 0.0,
		-0.5f,  0.5f, -0.5f,	0.0, 1.0, 1.0,			0.0, 1.0,
		-0.5f,  0.5f,  0.5f,	0.0, 1.0, 1.0,			0.0, 0.0,

		-0.5f, -0.5f,  0.5f,	0.0, 1.0, 0.0,			0.0, 0.0,	// ����
		 0.5f, -0.5f,  0.5f,	0.0, 1.0, 0.0,			1.0, 0.0,
		 0.5f,  0.5f,  0.5f,	0.0, 1.0, 0.0,			1.0, 1.0,

		 0.5f,  0.5f,  0.5f,	0.0, 1.0, 0.0,			1.0, 1.0,
		-0.5f,  0.5f,  0.5f,	0.0, 1.0, 0.0,			0.0, 1.0,
		-0.5f, -0.5f,  0.5f,	0.0, 1.0, 0.0,			0.0, 0.0
};

float vertexData2[] = {
		-0.5f, -0.5f, -0.5f,	1.0, 0.0, 0.0,			1.0, 0.0,		// ����
		 0.0f,  0.5f, -0.0f,	1.0, 0.0, 0.0,			0.0, 1.0,
		 0.5f, -0.5f, -0.5f,	1.0, 0.0, 0.0,			0.0, 0.0,

		-0.5f, -0.5f, -0.5f,	0.0, 0.0, 1.0,			0.0, 0.0,		// ����
		-0.5f, -0.5f,  0.5f,	0.0, 0.0, 1.0,			1.0, 0.0,
		-0.0f,  0.5f,  0.0f,	0.0, 0.0, 1.0,			0.0, 1.0,

		 0.5f, -0.5f, -0.5f,	1.0, 1.0, 0.0,			1.0, 0.0, // ������
		 0.0f,  0.0f,  0.0f,	1.0, 1.0, 0.0,			0.0, 1.0,
		 0.5f, -0.5f,  0.5f,	1.0, 1.0, 0.0,			0.0, 0.0,


		-0.5f, -0.5f, -0.5f,	1.0, 0.0, 1.0,			0.0, 0.0,	// �Ʒ���
		 0.5f, -0.5f, -0.5f,	1.0, 0.0, 1.0,			1.0, 0.0,
		 0.5f, -0.5f,  0.5f,	1.0, 0.0, 1.0,			0.0, 1.0,

		 0.5f, -0.5f,  0.5f,	1.0, 0.0, 1.0,			0.0, 1.0,
		-0.5f, -0.5f,  0.5f,	1.0, 0.0, 1.0,			0.0, 1.0,
		-0.5f, -0.5f, -0.5f,	1.0, 0.0, 1.0,			0.0, 0.0,


		-0.5f, -0.5f,  0.5f,	0.0, 1.0, 0.0,			0.0, 0.0,	// ����
		 0.5f, -0.5f,  0.5f,	0.0, 1.0, 0.0,			1.0, 0.0,
		 0.0f,  0.5f,  0.0f,	0.0, 1.0, 0.0,			1.0, 1.0,
};

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
			glm::vec2 texCoord;
			int result = sscanf_s(line + 3, "%f %f", &texCoord.x, &texCoord.y);
			texCoord.y = 1 - texCoord.y;

			if (result == 2) {
				std::cout << "Read TexCoord: (" << texCoord.x << ", " << texCoord.y << ")" << std::endl;
			}
			else {
				std::cerr << "Error parsing texture coordinates!" << std::endl;
			}
			// ���ؽ��� �ؽ�ó ��ǥ �߰�
			model->texCoords.push_back(texCoord); // �ؽ�ó ��ǥ�� ������ ���ͷ� �����ϴ� ���� �����ϴ�
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			// �� ������ �б� (v/vt/vn ���� ó��)
			unsigned int v[4], vt[4], vn[4];
			int matches = sscanf_s(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u",
				&v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2], &vn[2], &v[3], &vt[3], &vn[3]);

			if (matches == 9 || matches == 12) {
				// �ﰢ�� Ȥ�� �簢�� ó��
				Face face;
				face.v1 = v[0] - 1; // vertex �ε���
				face.v2 = v[1] - 1;
				face.v3 = v[2] - 1;

				// �ؽ�ó ��ǥ �ε����� ���� (1���� �����ϴ� �ε������� 0-based�� ��ȯ)
				face.t1 = vt[0] - 1;
				face.t2 = vt[1] - 1;
				face.t3 = vt[2] - 1;

				model->faces.push_back(face);

				// �簢���� ��� �� ��° �ﰢ�� ����
				if (matches == 12) {
					Face face2;
					face2.v1 = v[0] - 1;
					face2.v2 = v[2] - 1;
					face2.v3 = v[3] - 1;
					// �ؽ�ó ��ǥ �ε����� ����
					face2.t1 = vt[0] - 1;
					face2.t2 = vt[2] - 1;
					face2.t3 = vt[3] - 1;

					model->faces.push_back(face2);
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

GLuint cubeVAO, cubeVBO_position, cubeEBO;
GLuint pyramidVAO, pyramidVBO_position, pyramidEBO;
GLuint shaderProgram;

Model cubeModel, pyramidModel, planeModel;

void InitBufferForCube() {
	// �ε��� ������
	std::vector<unsigned int> indices;

	for (const auto& face : cubeModel.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	// VAO ����
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &cubeVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position);


	//--- ���� ���� vertexData �� ����� ���� ���ۿ� �������� ��

	glGenBuffers(1, &cubeVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData1), vertexData1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);			//--- ��ġ �Ӽ�
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	//--- �븻�� �Ӽ�
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	//--- �ؽ�ó ��ǥ �Ӽ�
	glEnableVertexAttribArray(2);

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

	// VAO ����
	glGenVertexArrays(1, &pyramidVAO);
	glBindVertexArray(pyramidVAO);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &pyramidVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO_position);
	glBufferData(GL_ARRAY_BUFFER, pyramidModel.vertices.size() * sizeof(Vertex), &pyramidModel.vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &pyramidVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData2), vertexData2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);			//--- ��ġ �Ӽ�
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	//--- �븻�� �Ӽ�
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	//--- �ؽ�ó ��ǥ �Ӽ�
	glEnableVertexAttribArray(2);

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

void InitTexture()
{
	stbi_set_flip_vertically_on_load(true); //--- �̹����� �Ųٷ� �����ٸ� �߰�
	glGenTextures(1, &texture1); //--- �ؽ�ó ����
	glBindTexture(GL_TEXTURE_2D, texture1); //--- �ؽ�ó ���ε�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data = stbi_load("cube.png", &widthImage, &heightImage, &numberOfChannel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, widthImage, heightImage, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //---�ؽ�ó �̹��� ����

	stbi_image_free(data);

	stbi_set_flip_vertically_on_load(true); //--- �̹����� �Ųٷ� �����ٸ� �߰�
	glGenTextures(1, &texture2); //--- �ؽ�ó ����
	glBindTexture(GL_TEXTURE_2D, texture2); //--- �ؽ�ó ���ε�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data2 = stbi_load("pyramid.png", &widthImage, &heightImage, &numberOfChannel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, widthImage, heightImage, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2); //---�ؽ�ó �̹��� ����

	stbi_image_free(data2);

	stbi_set_flip_vertically_on_load(true); //--- �̹����� �Ųٷ� �����ٸ� �߰�
	glGenTextures(1, &texture3); //--- �ؽ�ó ����
	glBindTexture(GL_TEXTURE_2D, texture3); //--- �ؽ�ó ���ε�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data3 = stbi_load("plane.png", &widthImage, &heightImage, &numberOfChannel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, widthImage, heightImage, 0, GL_RGBA, GL_UNSIGNED_BYTE, data3); //---�ؽ�ó �̹��� ����

	stbi_image_free(data3);
}

void InitOpenGL() {
	// GLEW �ʱ�ȭ
	glewExperimental = GL_TRUE;
	glewInit();

	// ���� �׽�Ʈ Ȱ��ȭ
	glEnable(GL_DEPTH_TEST);

	// ���� ���� ���� ����
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// ���̴� ������
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgram = make_shaderProgram();

	InitTexture();

	// OBJ ���� �ε�
	read_obj_file("cube.obj", &cubeModel);
	read_obj_file("pyramid.obj", &pyramidModel);

	// ���� �ʱ�ȭ
	InitBufferForCube();
	InitBufferForPyramid();
	InitBufferForAxes();  // �� ���� �ʱ�ȭ �߰�

}

void drawAxes() {
	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 6);  // 6���� ������ �׸��� (�� ��� 2���� ����)
	glBindVertexArray(0);
}

GLvoid drawScene() {
	// �÷� ���ۿ� ���� ���۸� ����ϴ�
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	//---------------------------------------------------------------
	glUseProgram(shaderProgram);
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
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

	glUseProgram(shaderProgram);

	unsigned int lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, 0.0, 10.0, 10.0);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgram, "viewPos"); //--- viewPos �� ����: ī�޶� ��ġ
	glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 modelMatrix = glm::mat4(1.0f);

	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Y�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X�� ȸ��

	// �� ��ȯ ����� ���̴��� ����
	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// �� �׸���
	drawAxes();

	// �̵� ��ȯ ���� (���� �̵�)

	modelMatrix = glm::translate(modelMatrix, glm::vec3(transX, transY, 0.0f)); // X��� Y�� �̵�

	// ȸ�� ��ȯ ���� (X�� -> Y�� ����, ������ ���� ��ǥ���� ���� ��������)
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Xangle - 30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Yangle - 30.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Y�� ȸ��

	// ��ȯ ����� ���̴��� ����
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	//------------------------------------------------------------------------------------------------
	glm::mat4 plane = glm::mat4(1.0f);
	plane= glm::scale(plane, glm::vec3(7.0, 7.0, 0.1));
	plane = glm::translate(plane, glm::vec3(0.0,0.0, -5.0));


	glUseProgram(shaderProgram);
	
	if (c) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(cubeVAO);

		glBindTexture(GL_TEXTURE_2D, texture1);
		//glDrawElements(GL_TRIANGLES, cubeModel.faces.size() * 3, GL_UNSIGNED_INT, 0);

		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
		glBindVertexArray(0);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(pyramidVAO);
		glBindTexture(GL_TEXTURE_2D, texture2);
		//glDrawElements(GL_TRIANGLES, pyramidModel.faces.size() * 3, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 10 * 3);
	}
	glBindVertexArray(0);

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(plane));
glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
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
	if (moveX)
		Xangle += dx;
	if (moveY)
		Yangle += dy;

	glutPostRedisplay(); // ȭ�� ����
	glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸� �Լ� ȣ��
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c': c = true; break;
	case 'p': c = false; break;
	case 'x': dx = 1; moveX = true; break;
	case 'X': dx = -1; moveX = true; break;
	case 'y': dy = 1; moveY = true; break;
	case 'Y': dy = -1; moveY = true; break;
	case 's': moveX = false; moveY = false; Xangle = 30.0f; Yangle = 30.0f; transX = 0.0f; transY = 0.0f; break;
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
	glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	glutMainLoop();               // �̺�Ʈ ó�� ����
}
