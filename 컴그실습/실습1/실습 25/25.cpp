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
//--------------------
bool p = false; bool m = false;

GLfloat angle = 0.0f;
GLfloat tx = 0.01f; GLfloat ty = 0.0f; GLfloat tz = 0.0;
GLfloat dy = 0.0f; GLfloat dz = 0.0f;

GLfloat lightX = 2.0f; GLfloat lightZ = 0.0f; int Lightangle = 0;
GLfloat lightR = 1.0f; GLfloat lightG = 1.0f; GLfloat lightB = 1.0f;

GLfloat ry1;
//----------------------------------
#define MAX_LINE_LENGTH 1024
#define M_PI 3.14159265358979323846

typedef struct {
	float x, y, z;   // 위치
	float nx, ny, nz; // 법선 벡터 추가
} Vertex;

typedef struct {
	unsigned int v1, v2, v3;
} Face;

typedef struct {
	std::vector<Vertex> vertices;
	std::vector<Face> faces;
} Model;

Model objModel; // 모델 데이터를 저장할 전역 변수

// OBJ 파일에서 버텍스와 면을 읽어오는 함수
void read_obj_file(const char* filename, Model* model) {
	FILE* file;
	fopen_s(&file, filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	char line[MAX_LINE_LENGTH];
	std::vector<glm::vec3> normals; // 법선 벡터 임시 저장
	std::vector<glm::vec2> texCoords; // 텍스처 좌표 임시 저장

	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'v' && line[1] == ' ') {
			// 버텍스 좌표 읽기
			Vertex vertex;
			sscanf_s(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			model->vertices.push_back(vertex);
		}
		else if (line[0] == 'v' && line[1] == 'n') {
			// 법선 벡터 읽기
			glm::vec3 normal;
			sscanf_s(line + 3, "%f %f %f", &normal.x, &normal.y, &normal.z);
			normals.push_back(normal);
		}
		else if (line[0] == 'v' && line[1] == 't') {
			// 텍스처 좌표 읽기
			glm::vec2 texCoord;
			sscanf_s(line + 3, "%f %f", &texCoord.x, &texCoord.y);
			texCoords.push_back(texCoord);
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			// 면 데이터 읽기 (v/vt/vn 형식 처리)
			unsigned int v[4], vt[4], vn[4];
			int matches = sscanf_s(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u",
				&v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2], &vn[2], &v[3], &vt[3], &vn[3]);

			if (matches == 9 || matches == 12) {
				// 삼각형 혹은 사각형 처리
				Face face;
				face.v1 = v[0] - 1;
				face.v2 = v[1] - 1;
				face.v3 = v[2] - 1;
				model->faces.push_back(face);

				Face face2;
				if (matches == 12) {
					// 사각형인 경우, 두 번째 삼각형 생성
					face2.v1 = v[0] - 1;
					face2.v2 = v[2] - 1;
					face2.v3 = v[3] - 1;
					model->faces.push_back(face2);
				}

				// 법선 벡터 저장
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

GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

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
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
	}
	else {
		std::cout << "Vertex shader 컴파일 성공!" << std::endl;
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
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
	}
	else {
		std::cout << "Fragment shader 컴파일 성공!" << std::endl;
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
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	else {
		std::cout << "Shader program 연결 성공!" << std::endl;
	}

	glUseProgram(shaderID);
	return shaderID;
}

GLuint axisVAO, axisVBO_position, axisVBO_color;
GLuint shaderProgram;

void InitBufferForAxes() {
	// 축의 정점 데이터
	std::vector<glm::vec3> axisVertices = {
		// X축
		glm::vec3(-10.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.0f, 0.0f),
		// Y축
		glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(0.0f, 10.0f, 0.0f),
		// Z축
		glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 10.0f)
	};

	// 축의 색상 데이터
	std::vector<glm::vec3> axisColors = {
		// X축은 빨간색
		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
		// Y축은 초록색
		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		// Z축은 파란색
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)
	};

	// VAO 설정
	glGenVertexArrays(1, &axisVAO);
	glBindVertexArray(axisVAO);

	// VBO 설정 (버텍스 데이터)
	glGenBuffers(1, &axisVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, axisVBO_position);
	glBufferData(GL_ARRAY_BUFFER, axisVertices.size() * sizeof(glm::vec3), &axisVertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(pAttribute);

	// VBO 설정 (색상 데이터)
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
	glDrawArrays(GL_LINES, 0, 6);  // 6개의 정점을 그리기 (각 축당 2개의 정점)
	glBindVertexArray(0);
}

GLuint circleVAO, circleVBO_position, circleVBO_color;

void InitBufferForCircle() {
	// 축의 정점 데이터
	std::vector<glm::vec3> circleVertices;
	for (int i = 0; i < 360; i++) {
		double angle = i * 3.141592 / 180;
		GLfloat x = 0.6 * cos(angle);
		GLfloat z = 0.6 * sin(angle);
		glm::vec3 temp = { x,0.0f,z };
		circleVertices.push_back(temp);
	}

	// 축의 색상 데이터
	std::vector<glm::vec3> circleColors;

	for (int i = 0; i < 360; i++) {
		glm::vec3 temp = { 0.0f,0.0f,0.0f };
		circleColors.push_back(temp);
	}

	// VAO 설정
	glGenVertexArrays(1, &circleVAO);
	glBindVertexArray(circleVAO);

	// VBO 설정 (버텍스 데이터)
	glGenBuffers(1, &circleVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, circleVBO_position);
	glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(glm::vec3), &circleVertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(pAttribute);

	// VBO 설정 (색상 데이터)
	glGenBuffers(1, &circleVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, circleVBO_color);
	glBufferData(GL_ARRAY_BUFFER, circleColors.size() * sizeof(glm::vec3), &circleColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);

	glBindVertexArray(0);
}

void drawCircle() {
	glBindVertexArray(circleVAO);
	glDrawArrays(GL_LINE_STRIP, 0, 360);
	glBindVertexArray(0);
}

GLuint sphereVAO, sphereVBO_position, sphereVBO_color, sphereEBO;

Model sphereModel;

std::vector<glm::vec3> sphereVertices;
std::vector<unsigned int> sphereIndices;

void InitBufferForSphere() {
	// 인덱스 데이터
	std::vector<unsigned int> indices;

	for (const auto& face : sphereModel.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	std::vector<glm::vec3> vertexColors;
	for (size_t i = 0; i < sphereModel.vertices.size(); ++i) {
		// 임의의 색상 지정 (단순히 각 정점마다 다른 색상을 주기 위해)
		vertexColors.push_back(glm::vec3(
			static_cast<float>(i % 3 == 0),
			static_cast<float>((i + 1) % 3 == 0),
			static_cast<float>((i + 2) % 3 == 0)
		));
	}

	// VAO 설정
	glGenVertexArrays(1, &sphereVAO);
	glBindVertexArray(sphereVAO);

	// VBO 설정 (버텍스 데이터)
	glGenBuffers(1, &sphereVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO_position);
	glBufferData(GL_ARRAY_BUFFER, sphereModel.vertices.size() * sizeof(Vertex), &sphereModel.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(pAttribute);

	// 법선 VBO 설정
	GLint nAttribute = glGetAttribLocation(shaderProgram, "vNormal");
	glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
	glEnableVertexAttribArray(nAttribute);

	// 색상 VBO 설정
	glGenBuffers(1, &sphereVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO 설정
	glGenBuffers(1, &sphereEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void InitOpenGL() {
	// GLEW 초기화
	glewExperimental = GL_TRUE;
	glewInit();

	// 깊이 테스트 활성화
	glEnable(GL_DEPTH_TEST);

	// 셰이더 컴파일
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgram = make_shaderProgram();

	// OBJ 파일 로드
	read_obj_file("sphere.obj", &sphereModel);

	// 버퍼 초기화
	InitBufferForSphere();
	InitBufferForAxes();  // 축 버퍼 초기화 추가
	InitBufferForCircle();
}

GLvoid drawScene() {
	// 컬러 버퍼와 깊이 버퍼를 지웁니다
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	//---------------------------------------------------------------
	glUseProgram(shaderProgram);
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform"); //--- 뷰잉 변환 설정
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	//---------------------------------------------------------------

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- 투영 공간 설정: fovy, aspect, near, far
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -1.0)); //--- 공간을 z축 이동
	unsigned int projectionLocation = glGetUniformLocation(shaderProgram, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	//---------------------------------------------------------------
	glm::mat4 Rx = glm::mat4(1.0f);
	glm::mat4 Ry = glm::mat4(1.0f);
	glm::mat4 Rz = glm::mat4(1.0f);
	glm::mat4 T1 = glm::mat4(1.0f);
	glm::mat4 RR = glm::mat4(1.0f);
	glm::mat4 S = glm::mat4(1.0f);

	S = glm::scale(S, glm::vec3(0.5, 0.5, 0.5));
	T1 = glm::translate(T1, glm::vec3(tx, ty, tz));
	Rx = glm::rotate(Rx, glm::radians(-10.0f), glm::vec3(1.0, 0.0, 0.0));
	Ry = glm::rotate(Ry, glm::radians(dy), glm::vec3(0.0, 1.0, 0.0));
	Rz = glm::rotate(Rz, glm::radians(dz), glm::vec3(0.0, 0.0, 1.0));
	RR = T1 * Rx * Rz * Ry;
	//---------------------------------------------------------------
	//행성
	glm::mat4 T2 = glm::mat4(1.0f);
	T2 = glm::translate(T2, glm::vec3(0.24 * cos(angle), 0.0f, 0.24 * sin(angle)));

	glm::mat4 S1 = glm::mat4(1.0f);
	glm::mat4 Tx1 = glm::mat4(1.0f);
	glm::mat4 Ry1 = glm::mat4(1.0f);
	glm::mat4 H11 = glm::mat4(1.0f);
	glm::mat4 H12 = glm::mat4(1.0f);
	S1 = glm::scale(S1, glm::vec3(0.4f, 0.4f, 0.4f));
	Tx1 = glm::translate(Tx1, glm::vec3(0.6f, 0.0f, 0.0f));
	Ry1 = glm::rotate(Ry, glm::radians(ry1), glm::vec3(0.0, 1.0, 0.0));
	H11 = RR * Ry1 * Tx1 * S1;
	H12 = RR * T2 * Ry1 * Tx1 * S1 * S1;

	glm::mat4 Rz21 = glm::mat4(1.0f);
	glm::mat4 H20 = glm::mat4(1.0f);
	glm::mat4 H21 = glm::mat4(1.0f);
	glm::mat4 H22 = glm::mat4(1.0f);
	glm::mat4 H23 = glm::mat4(1.0f);
	Rz21 = glm::rotate(Rz21, glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
	H20 = RR * Rz21;
	H21 = H20 * Ry1 * Tx1 * S1;
	H22 = RR * T2 * Rz21 * Ry1 * Tx1 * S1 * S1;
	H23 = Rz21 * Ry1 * Tx1 * S1;

	glm::mat4 Rz31 = glm::mat4(1.0f);
	glm::mat4 H30 = glm::mat4(1.0f);
	glm::mat4 H31 = glm::mat4(1.0f);
	glm::mat4 H32 = glm::mat4(1.0f);
	glm::mat4 H33 = glm::mat4(1.0f);
	Rz31 = glm::rotate(Rz31, glm::radians(-45.0f), glm::vec3(0.0, 0.0, 1.0));
	H30 = RR * Rz31;
	H31 = H30 * Ry1 * Tx1 * S1;
	H32 = RR * T2 * Rz31 * Ry1 * Tx1 * S1 * S1;
	H33 = Rz31 * Ry1 * Tx1 * S1;

	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(RR)); //--- modelTransform 변수에 변환 값 적용하기
	drawAxes();
	drawCircle();

	if (m)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//-------------------------------------------------------
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, lightX, 0.0, lightZ);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, lightR, lightG, lightB);
	unsigned int objColorLocation = glGetUniformLocation(shaderProgram, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
	glUniform3f(objColorLocation, 1.0, 0.5, 0.7);

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(RR * S)); //--- modelTransform 변수에 변환 값 적용하기
	//중심
	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLES, sphereModel.faces.size() * 3, GL_UNSIGNED_INT, 0); // 구를 삼각형으로 렌더링

	glUniform3f(objColorLocation, 0.5, 0.5, 1.0);
	//행성1
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H11 * S));
	glDrawElements(GL_TRIANGLES, sphereModel.faces.size() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	drawCircle();
	//행성2
	glBindVertexArray(sphereVAO);
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H20 * S));
	drawCircle();
	glBindVertexArray(sphereVAO);
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H21 * S));
	glDrawElements(GL_TRIANGLES, sphereModel.faces.size() * 3, GL_UNSIGNED_INT, 0);
	//행성3
	glBindVertexArray(sphereVAO);
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H30 * S));
	drawCircle();
	glBindVertexArray(sphereVAO);
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H31 * S));
	glDrawElements(GL_TRIANGLES, sphereModel.faces.size() * 3, GL_UNSIGNED_INT, 0);

	glUniform3f(objColorLocation, 0.7, 1.0, 0.7);
	//달1
	glBindVertexArray(sphereVAO);
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H12 * S));
	glDrawElements(GL_TRIANGLES, sphereModel.faces.size() * 3, GL_UNSIGNED_INT, 0);

	//달2
	glm::vec3 position = glm::vec3(H23[3]); // 위치는 유지하고
	glm::mat4 flatTransform = glm::translate(glm::mat4(1.0f), position); // 위치만 반영
	flatTransform = glm::scale(flatTransform, glm::vec3(0.36, 1.0f, 0.36)); // 반지름에 따른 스케일 적용
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(RR * flatTransform));
	drawCircle();

	glBindVertexArray(sphereVAO);
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H22 * S));
	glDrawElements(GL_TRIANGLES, sphereModel.faces.size() * 3, GL_UNSIGNED_INT, 0);


	//달3
	position = glm::vec3(H33[3]); // 위치는 유지하고
	flatTransform = glm::translate(glm::mat4(1.0f), position); // 위치만 반영
	flatTransform = glm::scale(flatTransform, glm::vec3(0.36, 1.0f, 0.36)); // 반지름에 따른 스케일 적용
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(RR * flatTransform));
	drawCircle();
	glBindVertexArray(sphereVAO);
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H32 * S));
	glDrawElements(GL_TRIANGLES, sphereModel.faces.size() * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);



	// 버퍼를 화면에 표시
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void TimerFunction(int value)
{
	glutPostRedisplay(); // 화면 갱신
	/*
	ry1 += 1.0f;
	angle += 0.1f;
	if (angle >= 360)
		angle = 0;
*/
	glutTimerFunc(60, TimerFunction, 1); // 타이머 함수 호출
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'p':
		p = !p; // p 키로 투영 모드 전환
		break;
	case'm':
		m = !m;
		break;
	case'+':
		tz -= 0.01;
		break;
	case'-':
		tz += 0.01;
		break;
	case 'y':
		dy -= 1.0f;
		break;
	case 'Y':
		dy += 1.0f;
		break;
	case 'z':
		dz -= 1.0f;
		break;
	case 'Z':
		dz += 1.0f;
		break;
	case 'w':
		ty += 0.01;
		break;
	case 's':
		ty -= 0.01;
		break;
	case 'a':
		tx -= 0.01;
		break;
	case 'd':
		tx += 0.01;
		break;
	case'r':
		Lightangle = (Lightangle + 1) % 360;
		lightX = cos(Lightangle) * 2.0f;
		lightZ = sin(Lightangle) * 2.0f;
		break;
	case'R':
		Lightangle = (Lightangle - 1) % 360;
		lightX = cos(Lightangle) * 2.0f;
		lightZ = sin(Lightangle) * 2.0f;
		break;
	case'c':
		lightR = rand() % 11 / 10.0f;
		lightG = rand() % 11 / 10.0f;
		lightB = rand() % 11 / 10.0f;
		break;
	default:
		break;
	}
	glutPostRedisplay(); //화면 갱신
}

GLvoid SpecialKey(int key, int x, int y) {
	switch (key) {
	}
	glutPostRedisplay();  // 화면을 갱신합니다.
}

void main(int argc, char** argv) {
	// 윈도우 생성
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("ㅍㅇㅌ");

	glEnable(GL_DEPTH_TEST);

	InitOpenGL();

	glutDisplayFunc(drawScene); //--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); //--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutSpecialFunc(SpecialKey);  // 특수 키 입력 처리 (방향키 등)
	glutTimerFunc(60, TimerFunction, 1); // 타이머 함수 설정
	glutMainLoop(); //--- 이벤트 처리 시작
}