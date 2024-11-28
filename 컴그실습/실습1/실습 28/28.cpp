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
//glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 3.0f);
glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 3.0f);

bool s = false; bool lightRotate = false;
GLfloat lightX = 0.0f; GLfloat lightZ = 3.0f; GLfloat lightD = 3.0f;
GLfloat lightR = 1.0f; GLfloat lightG = 1.0f; GLfloat lightB = 1.0f;
int lightAngle = 90; float light = 1.0f;
int level = 0; //시어핀스키 삼각형 레벨
int startIndex = 0;
bool m = true; int p = 0;

struct Snow {
	GLfloat tx;
	GLfloat ty;
	GLfloat tz;
	GLfloat speed;
};

Snow snow[50];
/*
for (int i = 0; i < 50; ++i) {
	snow[i].tx = rand() % 40 / 20.0f - 1.0f;
	snow[i].ty = 2.0f;
	snow[i].tz = rand() % 40 / 20.0f - 1.0f;
	snow[i].speed = rand() % 20 / 30.0f;
}*/
//----------------------------------
#define MAX_LINE_LENGTH 1024
#define Pi 3.141592f

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

GLuint shaderProgram;

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

GLuint cubeVAO, cubeVBO_position, cubeVBO_color, cubeEBO;
Model cubeModel;

void InitBufferForCube() {
	// 인덱스 데이터
	std::vector<unsigned int> indices;

	for (const auto& face : cubeModel.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	std::vector<glm::vec3> vertexColors;
	for (size_t i = 0; i < cubeModel.vertices.size(); ++i) {
		// 임의의 색상 지정 (단순히 각 정점마다 다른 색상을 주기 위해)
		vertexColors.push_back(glm::vec3(
			static_cast<float>(i % 3 == 0),
			static_cast<float>((i + 1) % 3 == 0),
			static_cast<float>((i + 2) % 3 == 0)
		));
	}

	// VAO 설정
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	// VBO 설정 (버텍스 데이터)
	glGenBuffers(1, &cubeVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position);
	glBufferData(GL_ARRAY_BUFFER, cubeModel.vertices.size() * sizeof(Vertex), &cubeModel.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(pAttribute);

	// 법선 VBO 설정
	GLint nAttribute = glGetAttribLocation(shaderProgram, "vNormal");
	glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
	glEnableVertexAttribArray(nAttribute);

	// 색상 VBO 설정
	glGenBuffers(1, &cubeVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO 설정
	glGenBuffers(1, &cubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

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

GLuint pyramidVAO, pyramidVBO_position, pyramidVBO_color, pyramidEBO;

Model pyramidModel;

void InitBufferForPyramid() {
	// 인덱스 데이터
	std::vector<unsigned int> indices;

	for (const auto& face : pyramidModel.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	std::vector<glm::vec3> vertexColors;
	for (size_t i = 0; i < pyramidModel.vertices.size(); ++i) {
		// 임의의 색상 지정 (단순히 각 정점마다 다른 색상을 주기 위해)
		vertexColors.push_back(glm::vec3(
			static_cast<float>(i % 3 == 0),
			static_cast<float>((i + 1) % 3 == 0),
			static_cast<float>((i + 2) % 3 == 0)
		));
	}

	// VAO 설정
	glGenVertexArrays(1, &pyramidVAO);
	glBindVertexArray(pyramidVAO);

	// VBO 설정 (버텍스 데이터)
	glGenBuffers(1, &pyramidVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO_position);
	glBufferData(GL_ARRAY_BUFFER, pyramidModel.vertices.size() * sizeof(Vertex), &pyramidModel.vertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(pAttribute);

	// 법선 VBO 설정
	GLint nAttribute = glGetAttribLocation(shaderProgram, "vNormal");
	glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
	glEnableVertexAttribArray(nAttribute);

	// 색상 VBO 설정
	glGenBuffers(1, &pyramidVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO 설정
	glGenBuffers(1, &pyramidEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO);
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
	read_obj_file("pyramid.obj", &pyramidModel);
	read_obj_file("sphere.obj", &sphereModel);
	read_obj_file("cube.obj", &cubeModel);

	// 버퍼 초기화
	InitBufferForPyramid();
	InitBufferForSphere();
	InitBufferForCube();
}

std::vector<glm::mat4> triangles;

std::vector<glm::vec3> triangles_color;

void make_triangle(glm::mat4 triangle, int Level) {
	if (Level == level)
		return;

	glm::mat4 temp1 = triangle;
	temp1 = glm::scale(temp1, glm::vec3(0.5, 0.5, 0.5));
	temp1 = glm::translate(temp1, glm::vec3(0.0, 0.401, 0.0));
	triangles.push_back(temp1);

	glm::mat4 temp2 = triangle;
	temp2 = glm::scale(temp2, glm::vec3(0.5, 0.5, 0.5));
	temp2 = glm::translate(temp2, glm::vec3(-0.401, -0.401, 0.401));
	triangles.push_back(temp2);

	glm::mat4 temp3 = triangle;
	temp3 = glm::scale(temp3, glm::vec3(0.5, 0.5, 0.5));
	temp3 = glm::translate(temp3, glm::vec3(-0.401, -0.401, -0.401));
	triangles.push_back(temp3);

	glm::mat4 temp4 = triangle;
	temp4 = glm::scale(temp4, glm::vec3(0.5, 0.5, 0.5));
	temp4 = glm::translate(temp4, glm::vec3(0.401, -0.401, -0.401));
	triangles.push_back(temp4);

	glm::mat4 temp5 = triangle;
	temp5 = glm::scale(temp5, glm::vec3(0.5, 0.5, 0.5));
	temp5 = glm::translate(temp5, glm::vec3(0.401, -0.401, 0.401));
	triangles.push_back(temp5);

	glm::vec3 temp_color;
	temp_color = glm::vec3((Level * 0.1f), (Level * 0.1f), (Level * 0.1f));
	triangles_color.push_back(temp_color);
	triangles_color.push_back(temp_color);
	triangles_color.push_back(temp_color);
	triangles_color.push_back(temp_color);
	triangles_color.push_back(temp_color);

	make_triangle(temp1, Level + 1);
	make_triangle(temp2, Level + 1);
	make_triangle(temp3, Level + 1);
	make_triangle(temp4, Level + 1);
	make_triangle(temp5, Level + 1);
}

GLvoid drawScene() {
	// 컬러 버퍼와 깊이 버퍼를 지웁니다
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	//---------------------------------------------------------------
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform"); //--- 뷰잉 변환 설정
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	//---------------------------------------------------------------

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- 투영 공간 설정: fovy, aspect, near, far
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0)); //--- 공간을 z축 이동
	unsigned int projectionLocation = glGetUniformLocation(shaderProgram, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	//---------------------------------------------------------------
	//바닥
	glm::mat4 floor = glm::mat4(1.0f);
	floor = glm::scale(floor, glm::vec3(5.0, 0.05, 5.0));
	floor = glm::translate(floor, glm::vec3(0.0, -0.39, 0.0));
	//조명
	glm::mat4 lights = glm::mat4(1.0f);
	lights = glm::translate(lights, glm::vec3(lightX, 1.0, lightZ));
	lights = glm::scale(lights, glm::vec3(0.5, 0.5, 0.5));
	//눈
	glm::mat4 snow_model[50];
	for (int i = 0; i < 50; ++i) {
		snow_model[i] = glm::mat4(1.0f);
		snow_model[i] = glm::translate(snow_model[i], glm::vec3(snow[i].tx, snow[i].ty, snow[i].tz));
		snow_model[i] = glm::scale(snow_model[i], glm::vec3(0.05, 0.05, 0.05));
	}

	if (triangles.size() != 0)
		triangles.clear();

	if (triangles_color.size() != 0)
		triangles_color.clear();

	//기본 피라미드
	glm::mat4 temp = glm::mat4(1.0f);
	temp = glm::translate(temp, glm::vec3(0.0, 0.8, 0.0));
	temp = glm::scale(temp, glm::vec3(2.0, 2.0, 2.0));
	triangles.push_back(temp);
	triangles_color.push_back(glm::vec3(1.0, 0.7, 0.8));

	make_triangle(triangles[0], 0);

	//-----------------
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, lightX, 1.0, lightZ);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, lightR, lightG, lightB);
	unsigned int objColorLocation = glGetUniformLocation(shaderProgram, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
	glUniform3f(objColorLocation, 1.0, 0.5, 0.6);
	unsigned int ablColorLocation = glGetUniformLocation(shaderProgram, "ambientLight"); //--- ambient Color값 전달: (1.0, 0.5, 0.3)의 색
	if (m)
		glUniform3f(ablColorLocation, light, light, light);
	else
		glUniform3f(ablColorLocation, 0.0, 0.0, 0.0);
	//------------------
	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");


	glBindVertexArray(cubeVAO);
	//바닥
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(floor));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	//조명
	glUniform3f(objColorLocation, 1.0, 1.0, 1.0);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(lights));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindVertexArray(sphereVAO);
	//눈
	for (int i = 0; i < 50; ++i) {
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(snow_model[i]));
		glDrawElements(GL_TRIANGLES, sphereModel.faces.size() * 3, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);

	glBindVertexArray(pyramidVAO);


	for (int i = 0; i < triangles.size(); ++i) {
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(triangles[i]));
		glUniform3f(objColorLocation, triangles_color[i].x, triangles_color[i].y, triangles_color[i].z);
		glDrawElements(GL_TRIANGLES, pyramidModel.faces.size() * 3, GL_UNSIGNED_INT, 0);
	}
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
	if (s) {
		for (int i = 0; i < 50; ++i) {
			snow[i].ty -= snow[i].speed;

			if (snow[i].ty <= 0.0f)
				snow[i].ty = 6.0f;
		}
	}

	if (lightRotate) {
		lightAngle += 2;
		lightX = lightD * cos(lightAngle * Pi / 180);
		lightZ = lightD * sin(lightAngle * Pi / 180);
	}

	glutPostRedisplay();
	glutTimerFunc(60, TimerFunction, 1);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case'0':
		level = 0;
		startIndex = 0;
		break;
	case'1':
		level = 1;
		startIndex = 1;
		break;
	case'2':
		level = 2;
		startIndex = 6;
		break;
	case'3':
		level = 3;
		startIndex = 6; //31;
		break;
	case'4':
		level = 4;
		startIndex = 6; //156;
		break;
	case'5':
		level = 5;
		startIndex = 6; //681;
		break;
	case's':
		s = !s;
		break;
	case'r':
		lightRotate = !lightRotate;
		break;
	case'n':
		lightD -= 0.1f;
		lightX = lightD * cos(lightAngle * Pi / 180);
		lightZ = lightD * sin(lightAngle * Pi / 180);
		break;
	case'f':
		lightD += 0.1f;
		lightX = lightD * cos(lightAngle * Pi / 180);
		lightZ = lightD * sin(lightAngle * Pi / 180);
		break;
	case'+':
		light += 0.1f;
		break;
	case'-':
		light -= 0.1f;
		break;
	case'm':
		m = !m;
		break;
	case'c':
		lightR = rand() % 20 / 20.0f;
		lightG = rand() % 20 / 20.0f;
		lightB = rand() % 20 / 20.0f;
		break;
	case'p':
		p=(p+1)%4;
		switch (p) {
		case 0:
			lightX = 1.0f;
			lightZ = 0.0f;
			break;
		case 1:
			lightX = -1.0f;
			lightZ = 0.0f;
			break;
		case 2:
			lightX = 0.0f;
			lightZ = 1.0f;
			break;
		case 3:
			lightX = 0.0f;
			lightZ = -1.0f;
			break;
		}
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}

	glutPostRedisplay(); //화면 갱신
}

void main(int argc, char** argv) {
	// 윈도우 생성
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 0);
	glutInitWindowSize(800, 800);
	glutCreateWindow("ㅍㅇㅌ");

	InitOpenGL();

	for (int i = 0; i < 50; ++i) {
		snow[i].tx = rand() % 80 / 20.0f - 2.0f;
		snow[i].ty = 6.0f;
		snow[i].tz = rand() % 80 / 20.0f - 2.0f;
		snow[i].speed = rand() % 20 / 50.0f + 0.01f;
	}

	glutDisplayFunc(drawScene); //--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); //--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutTimerFunc(60, TimerFunction, 1); // 타이머 함수 설정
	glutMainLoop(); //--- 이벤트 처리 시작
}