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
GLfloat t1x = 2.0f;//-0.3f;
int runX = 0; int rundx = 15;
int rotate = 0; int rotateNum = -1;
GLfloat tx = 0.0f; GLfloat ty = 0.0f; GLfloat tz = 0.0f;
bool jump = false; int jumpNum = 0;
int speed = 2;
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
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'v' && line[1] == ' ') {
			// 버텍스 좌표 읽기
			Vertex vertex;
			sscanf_s(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			model->vertices.push_back(vertex);
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			// 면 데이터 읽기 (v//vn 형식)
			unsigned int v1, v2, v3, vn1, vn2, vn3;
			int matches = sscanf_s(line + 2, "%u//%u %u//%u %u//%u", &v1, &vn1, &v2, &vn2, &v3, &vn3);

			if (matches == 6) {
				// v1, v2, v3는 1-based 인덱스이므로 0-based 인덱스로 변경
				Face face;
				face.v1 = v1 - 1;
				face.v2 = v2 - 1;
				face.v3 = v3 - 1;
				model->faces.push_back(face);
				std::cout << "Face added: (" << face.v1 << ", " << face.v2 << ", " << face.v3 << ")\n";
			}
		}
	}
	std::cout << "complete file " << filename << '\n';
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

GLuint shaderProgram;

GLuint cubeVAO1, cubeVBO_position1, cubeVBO_color1, cubeEBO1;
Model cubeModel1;

void InitBufferForCube1() {
	// 인덱스 데이터
	std::vector<unsigned int> indices;

	// 버텍스와 색상을 면 단위로 재정의
	std::vector<Vertex> faceVertices;
	std::vector<glm::vec3> faceColors = {
		glm::vec3(0.9f, 0.9f, 0.9f),
		glm::vec3(0.6f, 0.6f, 0.6f),
		glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.6f, 0.6f, 0.6f),
		glm::vec3(0.3f, 0.3f, 0.3f),
		glm::vec3(0.3f, 0.3f, 0.3f),
	};

	std::vector<glm::vec3> vertexColors; // 최종 색상 데이터

	for (size_t i = 0; i < cubeModel1.faces.size(); ++i) {
		const auto& face = cubeModel1.faces[i];
		glm::vec3 color = faceColors[i / 2]; // 각 면의 색상 (면 번호 기반)

		// 각 면의 버텍스를 중복 추가
		faceVertices.push_back(cubeModel1.vertices[face.v1]);
		faceVertices.push_back(cubeModel1.vertices[face.v2]);
		faceVertices.push_back(cubeModel1.vertices[face.v3]);

		// 색상도 동일하게 추가
		vertexColors.push_back(color);
		vertexColors.push_back(color);
		vertexColors.push_back(color);

		// 인덱스는 단순히 0부터 추가
		indices.push_back(i * 3);
		indices.push_back(i * 3 + 1);
		indices.push_back(i * 3 + 2);
	}

	// VAO 설정
	glGenVertexArrays(1, &cubeVAO1);
	glBindVertexArray(cubeVAO1);

	// VBO 설정 (버텍스 데이터)
	glGenBuffers(1, &cubeVBO_position1);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position1);
	glBufferData(GL_ARRAY_BUFFER, faceVertices.size() * sizeof(Vertex), &faceVertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);

	// 색상 VBO 설정
	glGenBuffers(1, &cubeVBO_color1);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_color1);
	glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO 설정
	glGenBuffers(1, &cubeEBO1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

GLuint cubeVAO2, cubeVBO_position2, cubeVBO_color2, cubeEBO2;
Model cubeModel2;

void InitBufferForCube2() {
	// 인덱스 데이터
	std::vector<unsigned int> indices;

	// 버텍스와 색상을 면 단위로 재정의
	std::vector<Vertex> faceVertices;
	std::vector<glm::vec3> faceColors = {
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.8f, 0.8f),
		glm::vec3(1.0f, 0.8f, 0.8f),
	};

	std::vector<glm::vec3> vertexColors; // 최종 색상 데이터

	for (size_t i = 0; i < cubeModel2.faces.size(); ++i) {
		const auto& face = cubeModel2.faces[i];
		glm::vec3 color = faceColors[i / 2]; // 각 면의 색상 (면 번호 기반)

		// 각 면의 버텍스를 중복 추가
		faceVertices.push_back(cubeModel2.vertices[face.v1]);
		faceVertices.push_back(cubeModel2.vertices[face.v2]);
		faceVertices.push_back(cubeModel2.vertices[face.v3]);

		// 색상도 동일하게 추가
		vertexColors.push_back(color);
		vertexColors.push_back(color);
		vertexColors.push_back(color);

		// 인덱스는 단순히 0부터 추가
		indices.push_back(i * 3);
		indices.push_back(i * 3 + 1);
		indices.push_back(i * 3 + 2);
	}

	// VAO 설정
	glGenVertexArrays(1, &cubeVAO2);
	glBindVertexArray(cubeVAO2);

	// VBO 설정 (버텍스 데이터)
	glGenBuffers(1, &cubeVBO_position2);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position2);
	glBufferData(GL_ARRAY_BUFFER, faceVertices.size() * sizeof(Vertex), &faceVertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);

	// 색상 VBO 설정
	glGenBuffers(1, &cubeVBO_color2);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_color2);
	glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO 설정
	glGenBuffers(1, &cubeEBO2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

GLuint cubeVAO3, cubeVBO_position3, cubeVBO_color3, cubeEBO3;
Model cubeModel3;

void InitBufferForCube3() {
	// 인덱스 데이터
	std::vector<unsigned int> indices;

	// 버텍스와 색상을 면 단위로 재정의
	std::vector<Vertex> faceVertices;
	std::vector<glm::vec3> faceColors = {
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.6f, 0.6f),
	};

	std::vector<glm::vec3> vertexColors; // 최종 색상 데이터

	for (size_t i = 0; i < cubeModel3.faces.size(); ++i) {
		const auto& face = cubeModel3.faces[i];
		glm::vec3 color = faceColors[i / 2]; // 각 면의 색상 (면 번호 기반)

		// 각 면의 버텍스를 중복 추가
		faceVertices.push_back(cubeModel3.vertices[face.v1]);
		faceVertices.push_back(cubeModel3.vertices[face.v2]);
		faceVertices.push_back(cubeModel3.vertices[face.v3]);

		// 색상도 동일하게 추가
		vertexColors.push_back(color);
		vertexColors.push_back(color);
		vertexColors.push_back(color);

		// 인덱스는 단순히 0부터 추가
		indices.push_back(i * 3);
		indices.push_back(i * 3 + 1);
		indices.push_back(i * 3 + 2);
	}

	// VAO 설정
	glGenVertexArrays(1, &cubeVAO3);
	glBindVertexArray(cubeVAO3);

	// VBO 설정 (버텍스 데이터)
	glGenBuffers(1, &cubeVBO_position3);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position3);
	glBufferData(GL_ARRAY_BUFFER, faceVertices.size() * sizeof(Vertex), &faceVertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);

	// 색상 VBO 설정
	glGenBuffers(1, &cubeVBO_color3);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_color3);
	glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO 설정
	glGenBuffers(1, &cubeEBO3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

GLuint cubeVAO4, cubeVBO_position4, cubeVBO_color4, cubeEBO4;
Model cubeModel4;

void InitBufferForCube4() {
	// 인덱스 데이터
	std::vector<unsigned int> indices;

	// 버텍스와 색상을 면 단위로 재정의
	std::vector<Vertex> faceVertices;
	std::vector<glm::vec3> faceColors = {
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.4f, 0.4f),
		glm::vec3(1.0f, 0.4f, 0.4f),
	};

	std::vector<glm::vec3> vertexColors; // 최종 색상 데이터

	for (size_t i = 0; i < cubeModel4.faces.size(); ++i) {
		const auto& face = cubeModel4.faces[i];
		glm::vec3 color = faceColors[i / 2]; // 각 면의 색상 (면 번호 기반)

		// 각 면의 버텍스를 중복 추가
		faceVertices.push_back(cubeModel4.vertices[face.v1]);
		faceVertices.push_back(cubeModel4.vertices[face.v2]);
		faceVertices.push_back(cubeModel4.vertices[face.v3]);

		// 색상도 동일하게 추가
		vertexColors.push_back(color);
		vertexColors.push_back(color);
		vertexColors.push_back(color);

		// 인덱스는 단순히 0부터 추가
		indices.push_back(i * 3);
		indices.push_back(i * 3 + 1);
		indices.push_back(i * 3 + 2);
	}

	// VAO 설정
	glGenVertexArrays(1, &cubeVAO4);
	glBindVertexArray(cubeVAO4);

	// VBO 설정 (버텍스 데이터)
	glGenBuffers(1, &cubeVBO_position4);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_position4);
	glBufferData(GL_ARRAY_BUFFER, faceVertices.size() * sizeof(Vertex), &faceVertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(pAttribute);

	// 색상 VBO 설정
	glGenBuffers(1, &cubeVBO_color4);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_color4);
	glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(cAttribute);

	// EBO 설정
	glGenBuffers(1, &cubeEBO4);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void InitOpenGL() {
	// GLEW 초기화
	glewExperimental = GL_TRUE;
	glewInit();

	// 깊이 테스트 활성화
	glEnable(GL_DEPTH_TEST);

	// 배경색을 흰색으로 설정 (R, G, B, A: 각각 1.0f로 설정하면 흰색)
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 흰색 배경

	// 셰이더 컴파일
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgram = make_shaderProgram();

	// OBJ 파일 로드
	read_obj_file("cube.obj", &cubeModel1);
	read_obj_file("cube.obj", &cubeModel2);
	read_obj_file("cube.obj", &cubeModel3);
	read_obj_file("cube.obj", &cubeModel4);

	// 버퍼 초기화
	InitBufferForCube1();
	InitBufferForCube2();
	InitBufferForCube3();
	InitBufferForCube4();
}

GLvoid drawScene() {
	// 컬러 버퍼와 깊이 버퍼를 지웁니다
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	//---------------------------------------------------------------
	glUseProgram(shaderProgram);
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

	//무대
	glm::mat4 S_1 = glm::mat4(1.0f);
	glm::mat4 H1 = glm::mat4(1.0f);
	S_1 = glm::scale(S_1, glm::vec3(5.0, 5.0, 5.0));
	H1 = S_1;
	//무대 앞면
	glm::mat4 S_1_ = glm::mat4(1.0f);
	glm::mat4 T_1_1 = glm::mat4(1.0f);
	glm::mat4 T_1_2 = glm::mat4(1.0f);
	glm::mat4 H1_1 = glm::mat4(1.0f);
	glm::mat4 H1_2 = glm::mat4(1.0f);
	S_1_ = glm::scale(S_1_, glm::vec3(2.5, 5.0, 5.0));
	T_1_1 = glm::translate(T_1_1, glm::vec3(t1x, 0.0, 0.0));
	T_1_2 = glm::translate(T_1_2, glm::vec3(-t1x, 0.0, 0.0));
	H1_1 = S_1_ * T_1_1;
	H1_2 = S_1_ * T_1_2;

	//로봇 전체
	glm::mat4 R = glm::mat4(1.0f);
	glm::mat4 T = glm::mat4(1.0f);
	glm::mat4 Default = glm::mat4(1.0f);
	R = glm::rotate(R, glm::radians(GLfloat(rotate)), glm::vec3(0.0, 1.0, 0.0));
	T = glm::translate(T, glm::vec3(tx, ty - 1.0f, tz));
	Default = T * R;
	// 로봇 머리
	glm::mat4 S_2 = glm::mat4(1.0f);
	glm::mat4 T_2 = glm::mat4(1.0f);
	glm::mat4 H2 = glm::mat4(1.0f);
	S_2 = glm::scale(S_2, glm::vec3(0.3, 0.3, 0.2));
	T_2 = glm::translate(T_2, glm::vec3(0.0, -0.1, 0.0));
	H2 = Default * T_2 * S_2;
	//로봇 코
	glm::mat4 S_2_ = glm::mat4(1.0f);
	glm::mat4 T_2_ = glm::mat4(1.0f);
	glm::mat4 H2_ = glm::mat4(1.0f);
	S_2_ = glm::scale(S_2_, glm::vec3(0.1, 0.1, 0.05));
	T_2_ = glm::translate(T_2_, glm::vec3(0.0, -0.1, 0.1));
	H2_ = Default * T_2_ * S_2_;
	//로봇몸
	glm::mat4 S_3 = glm::mat4(1.0f);
	glm::mat4 T_3 = glm::mat4(1.0f);
	glm::mat4 H3 = glm::mat4(1.0f);
	S_3 = glm::scale(S_3, glm::vec3(0.45, 0.5, 0.25)); // 0.2 0.3 0.1
	T_3 = glm::translate(T_3, glm::vec3(0.0, -0.4, 0.0));
	H3 = Default * T_3 * S_3;
	//로봇 다리
	glm::mat4 S_4_1 = glm::mat4(1.0f);
	glm::mat4 T_4_1 = glm::mat4(1.0f);
	glm::mat4 t_4_1 = glm::mat4(1.0f);
	glm::mat4 t_4_2 = glm::mat4(1.0f);
	glm::mat4 R_4_1 = glm::mat4(1.0f);
	glm::mat4 H4_1 = glm::mat4(1.0f);
	S_4_1 = glm::scale(S_4_1, glm::vec3(0.2, 0.5, 0.2));
	T_4_1 = glm::translate(T_4_1, glm::vec3(0.1, -0.8, 0.0));
	t_4_1 = glm::translate(t_4_1, glm::vec3(0.0, 0.2, 0.0));
	t_4_2 = glm::translate(t_4_2, glm::vec3(0.0, -0.2, 0.0));
	R_4_1 = glm::rotate(R_4_1, glm::radians(GLfloat(runX)), glm::vec3(1.0, 0.0, 0.0));
	H4_1 = Default * T_4_1 * t_4_1 * R_4_1 * t_4_2 * S_4_1;
	glm::mat4 S_4_2 = glm::mat4(1.0f);
	glm::mat4 T_4_2 = glm::mat4(1.0f);
	glm::mat4 R_4_2 = glm::mat4(1.0f);
	glm::mat4 H4_2 = glm::mat4(1.0f);
	S_4_2 = glm::scale(S_4_2, glm::vec3(0.2, 0.55, 0.2));
	T_4_2 = glm::translate(T_4_2, glm::vec3(-0.1, -0.8, 0.0));
	R_4_2 = glm::rotate(R_4_2, glm::radians(GLfloat(-runX)), glm::vec3(1.0, 0.0, 0.0));
	H4_2 = Default * T_4_2 * t_4_1 * R_4_2 * t_4_2 * S_4_2;
	//로봇 팔
	glm::mat4 S_5_1 = glm::mat4(1.0f);
	glm::mat4 T_5_1 = glm::mat4(1.0f);
	glm::mat4 t_5_1 = glm::mat4(1.0f);
	glm::mat4 t_5_2 = glm::mat4(1.0f);
	glm::mat4 R_5_1 = glm::mat4(1.0f);
	glm::mat4 H5_1 = glm::mat4(1.0f);
	S_5_1 = glm::scale(S_5_1, glm::vec3(0.1, 0.5, 0.1));
	T_5_1 = glm::translate(T_5_1, glm::vec3(0.2, -0.4, 0.0));
	t_5_1 = glm::translate(t_5_1, glm::vec3(0.0, 0.2, 0.0));
	t_5_2 = glm::translate(t_5_2, glm::vec3(0.0, -0.2, 0.0));
	R_5_1 = glm::rotate(R_5_1, glm::radians(GLfloat(-runX)), glm::vec3(1.0, 0.0, 0.0));
	H5_1 = Default * T_5_1 * t_5_1 * R_5_1 * t_5_2 * S_5_1;
	glm::mat4 S_5_2 = glm::mat4(1.0f);
	glm::mat4 T_5_2 = glm::mat4(1.0f);
	glm::mat4 R_5_2 = glm::mat4(1.0f);
	glm::mat4 H5_2 = glm::mat4(1.0f);
	S_5_2 = glm::scale(S_5_2, glm::vec3(0.1, 0.5, 0.1));
	T_5_2 = glm::translate(T_5_2, glm::vec3(-0.2, -0.4, 0.0));
	R_5_2 = glm::rotate(R_5_2, glm::radians(GLfloat(runX)), glm::vec3(1.0, 0.0, 0.0));
	H5_2 = Default * T_5_2 * t_5_1 * R_5_2 * t_5_2 * S_5_2;

	//장애물
	glm::mat4 T_6 = glm::mat4(1.0f);
	T_6 = glm::translate(T_6, glm::vec3(-1.6, -1.6, 0.0));

	glm::mat4 T_7 = glm::mat4(1.0f);
	T_7 = glm::translate(T_7, glm::vec3(0.6, -1.6, -1.6));

	glm::mat4 T_8 = glm::mat4(1.0f);
	T_8 = glm::translate(T_8, glm::vec3(+1.6, -1.6, 0.0));

	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H1));
	glBindVertexArray(cubeVAO1);

	//무대
	for (int i = 0; i < 6; ++i) {
		if (i != 2)
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * i));
	}

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H1_1));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * 2));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H1_2));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * 2));

	glBindVertexArray(0);
	glBindVertexArray(cubeVAO2);

	//로봇
	//얼굴
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H2));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	//코
	glBindVertexArray(0);
	glBindVertexArray(cubeVAO3);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H2_));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	//몸
	glBindVertexArray(0);
	glBindVertexArray(cubeVAO2);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H3));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	//다리
	glBindVertexArray(0);
	glBindVertexArray(cubeVAO3);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H4_1));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindVertexArray(cubeVAO4);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H4_2));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	//팔
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H5_1));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindVertexArray(cubeVAO3);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H5_2));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	; glBindVertexArray(0);

	//장애물
	glBindVertexArray(cubeVAO1);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(T_6));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(T_7));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(T_8));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

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
	if (rundx > 0)
		runX += rundx + speed * 3;
	else
		runX += rundx - speed * 3;

	if (runX >= 45 + speed * 10 || runX <= -45 - speed * 10)
		rundx *= -1;

	if (rotateNum == -1) {
		switch (rotate) {
		case 180:
			tz -= 0.03f + 0.015f * speed;
			if (tz < -1.9f)
				rotate = 0;

			// 장애물과 충돌처리
			if (!jump && ty < 0.8 && (((tx <= -1.1f || tx >= 1.1f) && (tz >= -0.9f && tz <= 0.9f))
				|| (tx >= 0.5f && tx <= 1.5f && tz <= -1.1f)))
				rotate = 0;
			break;
		case -90:
			tx -= 0.03f + 0.015f * speed;
			if (tx < -1.9f)
				rotate = 90;

			if (!jump && ty < 0.8 && (((tx <= -1.1f || tx >= 1.1f) && (tz >= -0.9f && tz <= 0.9f))
				|| (tx >= 0.5f && tx <= 1.5f && tz <= -1.1f)))
				rotate = 90;
			break;
		case 0:
			tz += 0.03f + 0.015f * speed;
			if (tz > 1.9f)
				rotate = 180;

			if (!jump && ty < 0.8 && (((tx <= -1.1f || tx >= 1.1f) && (tz >= -0.9f && tz <= 0.9f))
				|| (tx >= 0.5f && tx <= 1.5f && tz <= -1.1f)))
				rotate = 180;
			break;
		case 90:
			tx += 0.03f + 0.015f * speed;
			if (tx > 1.9f)
				rotate = -90;

			if (!jump && ty < 0.8 && (((tx <= -1.1f || tx >= 1.1f) && (tz >= -0.9f && tz <= 0.9f))
				|| (tx >= 0.5f && tx <= 1.5f && tz <= -1.1f)))
				rotate = -90;
			break;
		}
	}
	else {
		if (rotateNum > rotate)
			rotate += 15;
		else if (rotateNum < rotate)
			rotate -= 15;

		if (rotate == rotateNum)
			rotateNum = -1;
	}

	if (jump) {
		ty += 0.08f;  // 위로 점프
		++jumpNum;
		if (jumpNum == 15) {
			jump = false;
			jumpNum = 0;
		}
	}
	else {
		ty -= 0.08f;

		if (ty <= 0.0f)
			ty = 0.0f;
		
		else if(!jump && ty < 0.8 && (((tx <= -1.1f || tx >= 1.1f) && (tz >= -0.9f && tz <= 0.9f))
			|| (tx >= 0.5f && tx <= 1.5f && tz <= -1.1f)))
			ty = 0.8f;
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
		cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPos, 1.0f);
		break;
	case 'Y':
		cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPos, 1.0f);
		break;
	case 'O':
		t1x += 0.05f;
		break;
	case 'o':
		t1x -= 0.05f;
		break;
	case'w':
		rotateNum = 180;
		break;
	case'a':
		rotateNum = -90;
		break;
	case's':
		rotateNum = 0;
		break;
	case'd':
		rotateNum = 90;
		break;
	case'j':
		jump = true;
		break;
	case'i':
		cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
		t1x = 2.0f;
		runX = 0; rundx = 15;
		rotate = 0; rotateNum = -1;
		tx = 0.0f; ty = -1.15f; tz = 0.0f;
		jump = false;
		speed = 5;
		break;
	case'+':
		if (speed <= 13)
			++speed;
		break;
	case'-':
		if (speed >= -3)
			--speed;
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

	glEnable(GL_DEPTH_TEST);

	InitOpenGL();

	glutDisplayFunc(drawScene); //--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); //--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutTimerFunc(60, TimerFunction, 1); // 타이머 함수 설정
	glutMainLoop(); //--- 이벤트 처리 시작
}