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
int rz = 0;
int face_valid[6] = { 1,1,1,1,1,1 };


// ���� ���콺 X ��ǥ ���� ����
int prevMouseX = -1;

struct shapes {
	GLfloat tx;
	GLfloat ty;
	GLfloat tz;
	GLfloat dx;
	GLfloat dy;
	GLfloat dz;
};

shapes stage[6] = {
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f},
};

std::vector <shapes> balls = {
	{ (rand() % 35) / 10.0f - 1.7f, (rand() % 35) / 10.0f - 1.7f, (rand() % 35) / 10.0f - 1.7f,
				-0.05f + (rand() % 11) * 0.01f,-0.05f + (rand() % 11) * 0.01f,-0.05f + (rand() % 11) * 0.01f }
};

shapes cubes[3] = {
	{ 0.0f, -1.6f, -1.6f, 0.0f, 0.0f, 0.0f},
	{ 0.0f, -1.7f, -0.9f, 0.0f, 0.0f, 0.0f},
	{ 0.0f, -1.8f, -0.4f, 0.0f, 0.0f, 0.0f}
};

//----------------------------------
#define MAX_LINE_LENGTH 1024
#define M_PI 3.14159265358979323846

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

GLuint shaderProgram;

GLuint cubeVAO1, cubeVBO_position1, cubeVBO_color1, cubeEBO1;
Model cubeModel1;

void InitBufferForCube1() {
	// �ε��� ������
	std::vector<unsigned int> indices;

	// ���ؽ��� ������ �� ������ ������
	std::vector<Vertex> faceVertices;
	std::vector<glm::vec3> faceColors = {
		glm::vec3(0.9f, 0.9f, 0.9f),
		glm::vec3(0.6f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(0.6f, 0.6f, 0.6f),
		glm::vec3(0.3f, 0.3f, 0.3f),
		glm::vec3(0.3f, 0.3f, 0.3f),
	};

	std::vector<glm::vec3> vertexColors; // ���� ���� ������

	for (size_t i = 0; i < cubeModel1.faces.size(); ++i) {
		const auto& face = cubeModel1.faces[i];
		glm::vec3 color = faceColors[i / 2]; // �� ���� ���� (�� ��ȣ ���)

		// �� ���� ���ؽ��� �ߺ� �߰�
		faceVertices.push_back(cubeModel1.vertices[face.v1]);
		faceVertices.push_back(cubeModel1.vertices[face.v2]);
		faceVertices.push_back(cubeModel1.vertices[face.v3]);

		// ���� �����ϰ� �߰�
		vertexColors.push_back(color);
		vertexColors.push_back(color);
		vertexColors.push_back(color);

		// �ε����� �ܼ��� 0���� �߰�
		indices.push_back(i * 3);
		indices.push_back(i * 3 + 1);
		indices.push_back(i * 3 + 2);
	}

	// VAO ����
	glGenVertexArrays(1, &cubeVAO1);
	glBindVertexArray(cubeVAO1);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &cubeVBO_position1);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position1);
	glBufferData(GL_ARRAY_BUFFER, faceVertices.size() * sizeof(Vertex), &faceVertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);

	// ���� VBO ����
	glGenBuffers(1, &cubeVBO_color1);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_color1);
	glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
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

	// ���ؽ��� ������ �� ������ ������
	std::vector<Vertex> faceVertices;
	std::vector<glm::vec3> faceColors = {
		glm::vec3(1.0f, 0.7f, 0.7f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.3f, 0.3f),
	};

	std::vector<glm::vec3> vertexColors; // ���� ���� ������

	for (size_t i = 0; i < cubeModel2.faces.size(); ++i) {
		const auto& face = cubeModel2.faces[i];
		glm::vec3 color = faceColors[i / 2]; // �� ���� ���� (�� ��ȣ ���)

		// �� ���� ���ؽ��� �ߺ� �߰�
		faceVertices.push_back(cubeModel2.vertices[face.v1]);
		faceVertices.push_back(cubeModel2.vertices[face.v2]);
		faceVertices.push_back(cubeModel2.vertices[face.v3]);

		// ���� �����ϰ� �߰�
		vertexColors.push_back(color);
		vertexColors.push_back(color);
		vertexColors.push_back(color);

		// �ε����� �ܼ��� 0���� �߰�
		indices.push_back(i * 3);
		indices.push_back(i * 3 + 1);
		indices.push_back(i * 3 + 2);
	}

	// VAO ����
	glGenVertexArrays(1, &cubeVAO2);
	glBindVertexArray(cubeVAO2);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &cubeVBO_position2);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position2);
	glBufferData(GL_ARRAY_BUFFER, faceVertices.size() * sizeof(Vertex), &faceVertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);

	// ���� VBO ����
	glGenBuffers(1, &cubeVBO_color2);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_color2);
	glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO ����
	glGenBuffers(1, &cubeEBO2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

GLuint sphereVAO, sphereVBO_position, sphereVBO_color, sphereEBO;
Model sphereModel;

std::vector<glm::vec3> sphereVertices;
std::vector<unsigned int> sphereIndices;

void InitBufferForSphere(float radius = 0.15f, int sectors = 36, int stacks = 18) {
	// ���� ������ ����
	for (int i = 0; i <= stacks; ++i) {
		float stackAngle = M_PI / 2 - i * M_PI / stacks; // -pi/2 to pi/2
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);

		for (int j = 0; j <= sectors; ++j) {
			float sectorAngle = j * 2 * M_PI / sectors; // 0 to 2pi
			float x = xy * cosf(sectorAngle);
			float y = xy * sinf(sectorAngle);
			sphereVertices.push_back(glm::vec3(x, y, z));
		}
	}

	// �ε��� ������ ����
	for (int i = 0; i < stacks; ++i) {
		int k1 = i * (sectors + 1);
		int k2 = k1 + sectors + 1;
		for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
			if (i != 0) {
				sphereIndices.push_back(k1);
				sphereIndices.push_back(k2);
				sphereIndices.push_back(k1 + 1);
			}
			if (i != (stacks - 1)) {
				sphereIndices.push_back(k1 + 1);
				sphereIndices.push_back(k2);
				sphereIndices.push_back(k2 + 1);
			}
		}
	}

	// ���� ������ ����
	std::vector<glm::vec3> vertexColor(sphereVertices.size(), glm::vec3(1.0f, 0.2f, 0.4f));

	// VAO ����
	glGenVertexArrays(1, &sphereVAO);
	glBindVertexArray(sphereVAO);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &sphereVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO_position);
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(glm::vec3), &sphereVertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(pAttribute);

	// VBO ���� (���� ������)
	glGenBuffers(1, &sphereVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(glm::vec3), &vertexColor[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(cAttribute);

	// EBO ���� (�ε��� ������)
	glGenBuffers(1, &sphereEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void InitOpenGL() {
	// GLEW �ʱ�ȭ
	glewExperimental = GL_TRUE;
	glewInit();

	// ���� �׽�Ʈ Ȱ��ȭ
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	// ������ ������� ���� (R, G, B, A: ���� 1.0f�� �����ϸ� ���)
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // ��� ���

	// ���̴� ������
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgram = make_shaderProgram();

	// OBJ ���� �ε�
	read_obj_file("cube.obj", &cubeModel1);
	read_obj_file("cube.obj", &cubeModel2);
	read_obj_file("sphere.obj", &sphereModel);

	// ���� �ʱ�ȭ
	InitBufferForCube1();
	InitBufferForCube2();
	InitBufferForSphere();
}

GLvoid drawScene() {
	// �÷� ���ۿ� ���� ���۸� ����ϴ�
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgram);
	//---------------------------------------------------------------
	glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewTransform"), 1, GL_FALSE, &view[0][0]);
	//---------------------------------------------------------------
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0)); //--- ������ z�� �̵�
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionTransform"), 1, GL_FALSE, &projection[0][0]);
	//---------------------------------------------------------------

	glm::mat4 Default = glm::mat4(1.0f);
	Default = glm::rotate(Default, glm::radians(GLfloat(rz)), glm::vec3(0.0, 0.0, 1.0));

	//����
	glm::mat4 S_1 = glm::mat4(1.0f);
	glm::mat4 T_1[6] = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f),
		glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), };
	S_1 = glm::scale(S_1, glm::vec3(5.0, 5.0, 5.0));

	//����ü
	glm::mat4 T_2 = glm::mat4(1.0f);
	glm::mat4 S_2[3] = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) };
	glm::mat4 H2 = glm::mat4(1.0f);
	S_2[0] = glm::scale(S_2[0], glm::vec3(1.0, 1.0, 1.0));
	S_2[1] = glm::scale(S_2[1], glm::vec3(0.75, 0.75, 0.75));
	S_2[2] = glm::scale(S_2[2], glm::vec3(0.5, 0.5, 0.5));

	//��
	glm::mat4 T_3 = glm::mat4(1.0f);
	//-------------------------------------------------------------------
	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");

	//����
	glEnable(GL_CULL_FACE); // �������� Ȱ��ȭ
	glBindVertexArray(cubeVAO1);
	for (int i = 0; i < 6; ++i) {
		T_1[i] = glm::translate(T_1[i], glm::vec3(stage[i].tx, stage[i].ty, 0.0f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Default * S_1 * T_1[i]));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * i));
	}
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE); // �������� ��Ȱ��ȭ

	//����ü
	glBindVertexArray(cubeVAO2);
	for (int i = 0; i < 3; ++i) {
		T_2 = glm::mat4(1.0f);
		T_2 = glm::translate(T_2, glm::vec3(cubes[i].tx, cubes[i].ty, cubes[i].tz));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Default * T_2 * S_2[i]));
		glDrawElements(GL_TRIANGLES, cubeModel2.faces.size() * 3, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);

	//����
	glBindVertexArray(sphereVAO);
	for (int i = 0; i < balls.size(); ++i) {
		T_3 = glm::mat4(1.0f);
		T_3 = glm::translate(T_3, glm::vec3(balls[i].tx, balls[i].ty, balls[i].tz));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Default * T_3));
		glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
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
	//����ü ������Ʈ
	glm::vec3 gravity = glm::rotate(glm::mat4(1.0f), glm::radians(float(rz)), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);

	for (int i = 0; i < 3; ++i) {
		cubes[i].tx -= gravity.x * 0.07f;
		cubes[i].ty += gravity.y * 0.07f;

		if (cubes[i].tx <= -1.6f - 0.1f * i) {
			if (face_valid[1] == 1)
				cubes[i].tx += gravity.x * 0.07f;
		}
		if (cubes[i].tx >= 1.6f + 0.1f * i) {
			if (face_valid[3] == 1)
				cubes[i].tx += gravity.x * 0.07f;
		}
		if (cubes[i].ty >= 1.6f + 0.1f * i) {
			if (face_valid[4] == 1)
				cubes[i].ty -= gravity.y * 0.07f;
		}
		if (cubes[i].ty <= -1.6f - 0.1f * i) {
			if (face_valid[5] == 1)
				cubes[i].ty -= gravity.y * 0.07f;
		}
	}

	//�� ������Ʈ
	for (int i = 0; i < balls.size(); ++i) {
		//�̵�
		balls[i].tx += balls[i].dx;
		balls[i].ty += balls[i].dy;
		balls[i].tz += balls[i].dz;

		// ���� ���� ��� ��� ��
		
		if (balls[i].tz <= -1.7f && (balls[i].tx >= -1.7f && balls[i].tx <= 1.7f && balls[i].ty >= -1.7f && balls[i].ty <= 1.7f)) {
			if (face_valid[0] == 1)
				balls[i].dz *= -1;
		}
		if (balls[i].tx <= -1.7f && (balls[i].ty >= -1.7f && balls[i].ty <= 1.7f && balls[i].tz >= -1.7f && balls[i].tz <= 1.7f)) {
			if (face_valid[1] == 1)
				balls[i].dx *= -1;
		}
		if (balls[i].tz >= 1.7f && (balls[i].tx >= -1.7f && balls[i].tx <= 1.7f && balls[i].ty >= -1.7f && balls[i].ty <= 1.7f)) {
			if (face_valid[2] == 1)
				balls[i].dz *= -1;
		}
		if (balls[i].tx >= 1.7f && (balls[i].ty >= -1.7f && balls[i].ty <= 1.7f && balls[i].tz >= -1.7f && balls[i].tz <= 1.7f)) {
			if (face_valid[3] == 1)
				balls[i].dx *= -1;
		}
		if (balls[i].ty >= 1.7f && (balls[i].tx >= -1.7f && balls[i].tx <= 1.7f && balls[i].tz >= -1.7f && balls[i].tz <= 1.7f)) {
			if (face_valid[4] == 1)
				balls[i].dy *= -1;
		}
		if (balls[i].ty <= -1.7f && (balls[i].tx >= -1.7f && balls[i].tx <= 1.7f && balls[i].tz >= -1.7f && balls[i].tz <= 1.7f)) {
			if (face_valid[5] == 1)
				balls[i].dy *= -1;
		}
	}

	//���� ������Ʈ
	for (int i = 0; i < 6; ++i) {
		if (face_valid[i] == 0) {
			stage[i].tx += stage[i].dx;
			stage[i].ty += stage[i].dy;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(60, TimerFunction, 1);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case'1':
		if (face_valid[0] == 1) {
			face_valid[0] = 0;
			stage[0].dy = 0.1f;
		}
		else {
			face_valid[0] = 1;
			stage[0].ty = 0.0f;
		}
			break;
	case'2':
		if (face_valid[1] == 1) {
			face_valid[1] = 0;
			stage[1].dy = 0.1f;
		}
		else {
			face_valid[1] = 1;
			stage[1].ty = 0.0f;
		}
		break;
	case'3':
		if (face_valid[2] == 1) {
			face_valid[2] = 0;
			stage[2].dy = 0.1f;
		}
		else {
			face_valid[2] = 1;
			stage[2].ty = 0.0f;
		}
		break;
	case'4':
		if (face_valid[3] == 1) {
			face_valid[3] = 0;
			stage[3].dy = 0.1f;
		}
		else {
			face_valid[3] = 1;
			stage[3].ty = 0.0f;
		}
		break;
	case'5':
		if (face_valid[4] == 1) {
			face_valid[4] = 0;
			stage[4].dy = 0.1f;
		}
		else {
			face_valid[4] = 1;
			stage[4].ty = 0.0f;
		}
		break;
	case'6':
		if (face_valid[5] == 1) {
			face_valid[5] = 0;
			stage[5].dx = 0.1f;
		}
		else {
			face_valid[5] = 1;
			stage[5].tx = 0.0f;
		}
		break;
	case 'z':
		cameraPos.z += 0.1;
		break;
	case 'Z':
		cameraPos.z -= 0.1;
		break;
	case 'y':
		cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPos, 1.0f);
		break;
	case 'Y':
		cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPos, 1.0f);
		break;
	case 'b':
		if (balls.size() <= 5) {
			shapes temp = {
				(rand() % 35) / 10.0f - 1.7f, (rand() % 35) / 10.0f - 1.7f, (rand() % 35) / 10.0f - 1.7f,
				-0.05f + (rand() % 6) * 0.02f,-0.05f + (rand() % 6) * 0.02f,-0.05f + (rand() % 6) * 0.02f
			};
			balls.push_back(temp);
		}
		break;
	}
	glutPostRedisplay(); //ȭ�� ����
}

void Motion(int x, int y) {

	if (prevMouseX == -1) {
		prevMouseX = x;
		return;
	}

	// ���� �Ǵ� ���������� �̵� ����
	if (x > prevMouseX) {
		rz -= 1;
	}
	else if (x < prevMouseX) {
		rz += 1;
	}

	// ���� ��ġ�� ���� ��ġ�� ������Ʈ
	prevMouseX = x;

	glutPostRedisplay(); //ȭ�� ����
}

void main(int argc, char** argv) {
	// ������ ����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 0);
	glutInitWindowSize(800, 800);
	glutCreateWindow("������");

	glEnable(GL_DEPTH_TEST);

	InitOpenGL();

	glutDisplayFunc(drawScene); //--- ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape); //--- �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard); //--- Ű���� �Է� �ݹ��Լ� ����
	glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	glutMotionFunc(Motion);
	glutMainLoop(); //--- �̺�Ʈ ó�� ����
}