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
bool c = true; bool h = false; bool w = true; bool moveX = false; bool moveY = false;
GLfloat transX = 0.0f; GLfloat transY = 0.0f;
GLfloat Xangle = 30.0f; GLfloat Yangle = 30.0f;
int dx = 1; int dy = 1;
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
GLuint shaderProgram;

Model cubeModel, pyramidModel;

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
		glm::vec3(1.0f, 0.0f, 0.0f),   // ����
		glm::vec3(1.0f, 0.5f, 0.0f),   // ��Ȳ
		glm::vec3(1.0f, 1.0f, 0.0f),   // ���
		glm::vec3(0.0f, 1.0f, 0.0f),   // �ʷ� 
		glm::vec3(0.0f, 1.0f, 1.0f),   // û��
		glm::vec3(0.0f, 0.0f, 1.0f),   // �Ķ�
		glm::vec3(0.5f, 0.0f, 1.0f),   // ����
		glm::vec3(1.0f, 0.0f, 1.0f)    // ��ȫ
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

	// ���� VBO ����
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
		glm::vec3(1.0f, 0.0f, 0.0f),   // ����
		glm::vec3(1.0f, 1.0f, 0.0f),   // ���
		glm::vec3(0.0f, 1.0f, 0.0f),   // �ʷ�
		glm::vec3(0.0f, 0.0f, 1.0f),   // �Ķ�
		glm::vec3(0.5f, 0.0f, 1.0f)    // ����
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

void InitOpenGL() {
	// GLEW �ʱ�ȭ
	glewExperimental = GL_TRUE;
	glewInit();

	// ���� �׽�Ʈ Ȱ��ȭ
	glEnable(GL_DEPTH_TEST);

	// ���� ���� ���� ����
	glEnable(GL_CULL_FACE);      // ���� ���� Ȱ��ȭ
	glCullFace(GL_BACK);         // �ĸ��� ����
	glFrontFace(GL_CCW);         // �ݽð� ������ �ո����� �ν�

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
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Xangle-30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Yangle-30.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Y�� ȸ��

	// ��ȯ ����� ���̴��� ����
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// VAO ���ε� �� �׸���

	if (c) {
		if (h)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		if (w)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // ���̾������� ���
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // �⺻ ��� (ä��)
		for (int i = 0; i < 6; i++) {
			glBindVertexArray(cubeVAO);
			glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 4 * i));
		}
	}
	else {
		if (h)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		if (w)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // ���̾������� ���
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // �⺻ ��� (ä��)
		glBindVertexArray(pyramidVAO);
		glDrawElements(GL_TRIANGLES, 13, GL_UNSIGNED_INT, 0);
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
	if (moveX)
		Xangle += dx;
	if (moveY)
		Yangle += dy;

	InitOpenGL();
	glutPostRedisplay(); // ȭ�� ����
	glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸� �Լ� ȣ��
}

GLvoid SpecialKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		transY += 0.01f;  // ���� ����Ű: Y�� �̵� ����
		break;
	case GLUT_KEY_DOWN:
		transY -= 0.01f;  // �Ʒ��� ����Ű: Y�� �̵� ����
		break;
	case GLUT_KEY_LEFT:
		transX -= 0.01f;  // ���� ����Ű: X�� �̵� ����
		break;
	case GLUT_KEY_RIGHT:
		transX += 0.01f;  // ������ ����Ű: X�� �̵� ����
		break;
	}

	glutPostRedisplay();  // ȭ���� �����մϴ�.
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c': c = true; break;
	case 'p': c = false; break;
	case 'h': h = !h; break;
	case 'w': w = true; break;
	case 'W': w = false; break;
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
	glutSpecialFunc(SpecialKey);  // Ư�� Ű �Է� ó�� (����Ű ��)
	glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	glutMainLoop();               // �̺�Ʈ ó�� ����
}
