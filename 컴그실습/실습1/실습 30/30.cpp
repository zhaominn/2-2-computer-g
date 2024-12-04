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

#define STB_IMAGE_IMPLEMENTATION
#include "gl/stb_image.h"
int widthImage, heightImage, numberOfChannel;
unsigned int texture;
//---------------------
//glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 3.0f);
GLfloat cameraX = 0.0f; GLfloat cameraZ = 3.0f;

bool s = false; bool lightRotate = false;
GLfloat lightX = 0.0f; GLfloat lightZ = 3.0f; GLfloat lightD = 3.0f;
int lightAngle = 90; float light = 1.0f; int cameraangle = 90;
int level = 0; //시어핀스키 삼각형 레벨
int startIndex = 0;
int lightSet = 1;

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
	float tx, ty;     // 텍스처 좌표
} Vertex;

typedef struct {
	unsigned int v1, v2, v3;    // 버텍스 인덱스
	unsigned int t1, t2, t3;    // 텍스처 좌표 인덱스
} Face;

typedef struct {
	std::vector<Vertex> vertices;
	std::vector<Face> faces;
	std::vector<glm::vec2> texCoords; // 텍스처 좌표
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
			glm::vec2 texCoord;
			int result = sscanf_s(line + 3, "%f %f", &texCoord.x, &texCoord.y);

			if (result == 2) {
				std::cout << "Read TexCoord: (" << texCoord.x << ", " << texCoord.y << ")" << std::endl;
			}
			else {
				std::cerr << "Error parsing texture coordinates!" << std::endl;
			}
			// 버텍스에 텍스처 좌표 추가
			model->texCoords.push_back(texCoord); // 텍스처 좌표는 별도의 벡터로 관리하는 것이 좋습니다
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			// 면 데이터 읽기 (v/vt/vn 형식 처리)
			unsigned int v[4], vt[4], vn[4];
			int matches = sscanf_s(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u",
				&v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2], &vn[2], &v[3], &vt[3], &vn[3]);

			if (matches == 9 || matches == 12) {
				// 삼각형 혹은 사각형 처리
				Face face;
				face.v1 = v[0] - 1; // vertex 인덱스
				face.v2 = v[1] - 1;
				face.v3 = v[2] - 1;

				// 텍스처 좌표 인덱스를 저장 (1부터 시작하는 인덱스에서 0-based로 변환)
				face.t1 = vt[0] - 1;
				face.t2 = vt[1] - 1;
				face.t3 = vt[2] - 1;

				model->faces.push_back(face);

				// 사각형인 경우 두 번째 삼각형 생성
				if (matches == 12) {
					Face face2;
					face2.v1 = v[0] - 1;
					face2.v2 = v[2] - 1;
					face2.v3 = v[3] - 1;
					// 텍스처 좌표 인덱스를 저장
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

	// 텍스처 설정
	GLint tAttribute = glGetAttribLocation(shaderProgram, "vTexCoord");
	glVertexAttribPointer(tAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float))); //--- 텍스처 좌표 속성
	glEnableVertexAttribArray(tAttribute);

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

	// 텍스처 설정
	GLint tAttribute = glGetAttribLocation(shaderProgram, "vTexCoord");
	glVertexAttribPointer(tAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float))); //--- 텍스처 좌표 속성
	glEnableVertexAttribArray(tAttribute);

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

	// 텍스처 설정
	GLint tAttribute = glGetAttribLocation(shaderProgram, "vTexCoord");
	glVertexAttribPointer(tAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float))); //--- 텍스처 좌표 속성
	glEnableVertexAttribArray(tAttribute);


	// EBO 설정
	glGenBuffers(1, &pyramidEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void InitTexture()
{
	stbi_set_flip_vertically_on_load(true); //--- 이미지가 거꾸로 읽힌다면 추가
	glGenTextures(1, &texture); //--- 텍스처 생성
	glBindTexture(GL_TEXTURE_2D, texture); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data = stbi_load("pyramid.bmp", &widthImage, &heightImage, &numberOfChannel, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImage, heightImage, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //---텍스처 이미지 정의
	stbi_image_free(data);
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

	// 텍스쳐 로드
	InitTexture();

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

	glm::vec3 cameraPos = glm::vec3(cameraX, 2.0f, cameraZ);
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
	glUniform3f(lightColorLocation, lightSet, lightSet, lightSet);
	unsigned int ablColorLocation = glGetUniformLocation(shaderProgram, "ambientLight"); //--- ambient Color값 전달: (1.0, 0.5, 0.3)의 색
	glUniform3f(ablColorLocation, light, light, light);
	//------------------
	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");


	glBindVertexArray(cubeVAO);
	//바닥
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(floor));
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	//조명
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(lights));
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindVertexArray(sphereVAO);
	//눈
	for (int i = 0; i < 50; ++i) {
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(snow_model[i]));
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawElements(GL_TRIANGLES, sphereModel.faces.size() * 3, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);

	glBindVertexArray(pyramidVAO);

	for (int i = 0; i < triangles.size(); ++i) {
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(triangles[i]));
		glBindTexture(GL_TEXTURE_2D, texture);
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
	case'm':
		if (lightSet == 1)
			lightSet = 0;
		else
			lightSet = 1;
	case'f':
		lightD += 0.1f;
		lightX = lightD * cos(lightAngle * Pi / 180);
		lightZ = lightD * sin(lightAngle * Pi / 180);
		break;
	case 'y':
		cameraangle += 1;
		cameraX = 3.0f * cos(cameraangle * Pi / 180);
		cameraZ = 3.0f * sin(cameraangle * Pi / 180);
		break;
	case'+':
		light += 0.1f;
		break;
	case'-':
		light -= 0.1f;
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