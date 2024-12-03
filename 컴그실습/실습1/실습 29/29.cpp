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
unsigned int texture;

//----------------------------------
bool c = true; bool moveX = false; bool moveY = false;
GLfloat transX = 0.0f; GLfloat transY = 0.0f;
GLfloat Xangle = 30.0f; GLfloat Yangle = 30.0f;
int dx = 1; int dy = 1; int Lightangle = 0;
bool m = true; GLfloat z = 10;
//----------------------------------
#define MAX_LINE_LENGTH 1024

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
				std::cout << "Read TexCoord: (" << texCoord.x<< ", " << texCoord.y << ")" << std::endl;
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

Model cubeModel, pyramidModel;

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

	// EBO 설정
	glGenBuffers(1, &cubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

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

GLuint axisVAO, axisVBO_position, axisVBO_color;

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

void InitTexture()
{
	stbi_set_flip_vertically_on_load(true); //--- 이미지가 거꾸로 읽힌다면 추가
	glGenTextures(1, &texture); //--- 텍스처 생성
	glBindTexture(GL_TEXTURE_2D, texture); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	unsigned char* data = stbi_load("pyramid.png", &widthImage, &heightImage, &numberOfChannel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImage, heightImage, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //---텍스처 이미지 정의
	stbi_image_free(data);
}

void InitOpenGL() {
	// GLEW 초기화
	glewExperimental = GL_TRUE;
	glewInit();

	// 깊이 테스트 활성화
	glEnable(GL_DEPTH_TEST);

	// 은면 제거 기준 설정
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// 셰이더 컴파일
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgram = make_shaderProgram();

	InitTexture();

	// OBJ 파일 로드
	read_obj_file("cube.obj", &cubeModel);
	read_obj_file("pyramid.obj", &pyramidModel);

	// 버퍼 초기화
	InitBufferForCube();
	InitBufferForPyramid();
	InitBufferForAxes();  // 축 버퍼 초기화 추가

}

void drawAxes() {
	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 6);  // 6개의 정점을 그리기 (각 축당 2개의 정점)
	glBindVertexArray(0);
}

GLvoid drawScene() {
	// 컬러 버퍼와 깊이 버퍼를 지웁니다
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	//---------------------------------------------------------------
	glUseProgram(shaderProgram);
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
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

	glUseProgram(shaderProgram);

	unsigned int lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, 0.0, 10.0, 10.0);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgram, "viewPos"); //--- viewPos 값 전달: 카메라 위치
	glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 modelMatrix = glm::mat4(1.0f);

	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Y축 회전
	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X축 회전

	// 모델 변환 행렬을 셰이더로 전달
	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// 축 그리기
	drawAxes();

	// 이동 변환 적용 (먼저 이동)
	modelMatrix = glm::translate(modelMatrix, glm::vec3(transX, transY, 0.0f)); // X축과 Y축 이동

	// 회전 변환 적용 (X축 -> Y축 순서, 고정된 월드 좌표계의 축을 기준으로)
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Xangle - 30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X축 회전
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Yangle - 30.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Y축 회전

	// 변환 행렬을 셰이더로 전달
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	//------------------------------------------------------------------------------------------------

	glUseProgram(shaderProgram);

	for (const auto& face : cubeModel.faces) {
		std::cout << "Face:" << std::endl;

		// 텍스처 좌표를 이미지 픽셀로 변환하여 출력
		float texWidth = (float)widthImage;
		float texHeight = (float)heightImage;

		glm::vec2 texCoord1 = cubeModel.texCoords[face.t1];
		glm::vec2 texCoord2 = cubeModel.texCoords[face.t2];
		glm::vec2 texCoord3 = cubeModel.texCoords[face.t3];

		// 텍스처 좌표를 픽셀 위치로 변환 (0.0-1.0 -> 0-200 범위로)
		int pixelX1 = (int)(texCoord1.x * texWidth);
		int pixelY1 = (int)((1.0f - texCoord1.y) * texHeight);  // OpenGL의 텍스처 좌표 시스템은 y가 반전됨

		int pixelX2 = (int)(texCoord2.x * texWidth);
		int pixelY2 = (int)((1.0f - texCoord2.y) * texHeight);

		int pixelX3 = (int)(texCoord3.x * texWidth);
		int pixelY3 = (int)((1.0f - texCoord3.y) * texHeight);

		// 출력
		std::cout << "Vertex 1 TexCoord: (" << texCoord1.x << ", " << texCoord1.y << ") => Pixel: (" << pixelX1 << ", " << pixelY1 << ")" << std::endl;
		std::cout << "Vertex 2 TexCoord: (" << texCoord2.x << ", " << texCoord2.y << ") => Pixel: (" << pixelX2 << ", " << pixelY2 << ")" << std::endl;
		std::cout << "Vertex 3 TexCoord: (" << texCoord3.x << ", " << texCoord3.y << ") => Pixel: (" << pixelX3 << ", " << pixelY3 << ")" << std::endl;
	}

	if (c) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawElements(GL_TRIANGLES, cubeModel.faces.size() * 3, GL_UNSIGNED_INT, 0);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(pyramidVAO);
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
	if (moveX)
		Xangle += dx;
	if (moveY)
		Yangle += dy;

	glutPostRedisplay(); // 화면 갱신
	glutTimerFunc(60, TimerFunction, 1); // 타이머 함수 호출
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

	glutDisplayFunc(drawScene);   // 출력 콜백함수의 지정
	glutReshapeFunc(Reshape);     // 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard);   // 일반 키 입력 처리
	glutTimerFunc(60, TimerFunction, 1); // 타이머 함수 설정
	glutMainLoop();               // 이벤트 처리 시작
}
