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
bool c = true; bool h = false; bool w = false; bool moveX = false; bool moveY = false;
GLfloat transX = 0.0f; GLfloat transY = 0.0f;
GLfloat Xangle = 30.0f; GLfloat Yangle = 30.0f;
int dx = 1; int dy = 1;
GLfloat lightX = 0.0f; GLfloat lightZ = 10.0f; int Lightangle = 0;
bool m = true; GLfloat z = 10;
//----------------------------------
#define MAX_LINE_LENGTH 1024

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


void InitBufferForPyramid() {
	// �ε��� ������
	std::vector<unsigned int> indices;

	for (const auto& face : pyramidModel.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	std::vector<glm::vec3> vertexColors;
	for (size_t i = 0; i < pyramidModel.vertices.size(); ++i) {
		// ������ ���� ���� (�ܼ��� �� �������� �ٸ� ������ �ֱ� ����)
		vertexColors.push_back(glm::vec3(
			static_cast<float>(i % 3 == 0),
			static_cast<float>((i + 1) % 3 == 0),
			static_cast<float>((i + 2) % 3 == 0)
		));
	}

	// VAO ����
	glGenVertexArrays(1, &pyramidVAO);
	glBindVertexArray(pyramidVAO);

	// VBO ���� (���ؽ� ������)
	glGenBuffers(1, &pyramidVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO_position);
	glBufferData(GL_ARRAY_BUFFER, pyramidModel.vertices.size() * sizeof(Vertex), &pyramidModel.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(pAttribute);

	// ���� VBO ����
	GLint nAttribute = glGetAttribLocation(shaderProgram, "vNormal");
	glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
	glEnableVertexAttribArray(nAttribute);

	// ���� VBO ����
	glGenBuffers(1, &pyramidVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], GL_STATIC_DRAW);

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
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

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
	//---------------------------------------------------------------
	glUseProgram(shaderProgram);
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
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

	glUseProgram(shaderProgram);
	if (m) {
		unsigned int lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
		glUniform3f(lightPosLocation, lightX, 2.0, lightZ);
		unsigned int lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
		glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
		//unsigned int objColorLocation = glGetUniformLocation(shaderProgram, "objectColor"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��
		//glUniform3f(objColorLocation, 1.0, 0.5, 0.3);
		unsigned int viewPosLocation = glGetUniformLocation(shaderProgram, "viewPos"); //--- viewPos �� ����: ī�޶� ��ġ
		glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);
	}
	else {}
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

		glBindVertexArray(cubeVAO);
		glDrawElements(GL_TRIANGLES, cubeModel.faces.size() * 3, GL_UNSIGNED_INT, 0);

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
		glDrawElements(GL_TRIANGLES, pyramidModel.faces.size() * 3, GL_UNSIGNED_INT, 0);
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
	case 'n': c = !c;
	case 'm': m = !m;
	case'r':
		Lightangle = (Lightangle + 1) % 360;
		lightX = cos(Lightangle) * z;
		lightZ = sin(Lightangle) * z;
		break;
	case'z':
		++z;
		lightX = cos(Lightangle) * z;
		lightZ = sin(Lightangle) * z;
		break;
	case'Z':
		--z;
		lightX = cos(Lightangle) * z;
		lightZ = sin(Lightangle) * z;
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
	glutTimerFunc(60, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	glutMainLoop();               // �̺�Ʈ ó�� ����
}
