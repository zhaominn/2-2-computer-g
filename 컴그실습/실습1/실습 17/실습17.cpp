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
bool h = false;
bool Y_move = false; GLfloat dY = 0.0f;
bool c = true;
bool t = false; bool f = false; bool s = false; bool b = false;
bool o = false; bool r = false;
bool p = false;

GLfloat ry1 = 0.0f; GLfloat rx1 = 0.0f; GLfloat drx1 = 1.5f; GLfloat tz1 = 0.0f; GLfloat dtz1 = 0.01f;
GLfloat ty2 = 0.0f; GLfloat dty2 = 0.01f;
GLfloat s3 = 1.0f; GLfloat sd3 = 0.01f;
GLfloat ry5 = 0.0f; GLfloat rx5 = 0.0f; GLfloat ty5 = -0.3f;

GLfloat rrx1 = 0.0f; GLfloat rrdx1 = 3.0f; GLfloat ttz1 = 0.0f; GLfloat ttdz1 = 0.005f;
GLfloat rrz2= 0.0f; GLfloat rrdz2 = 3.0f; GLfloat ttx2 = 0.0f; GLfloat ttdx2 = 0.005f;
GLfloat rrx3 = 0.0f; GLfloat rrdx3 = 3.0f; GLfloat ttz3 = 0.0f; GLfloat ttdz3 = 0.005f;
GLfloat rrz4 = 0.0f; GLfloat rrdz4 = 3.0f; GLfloat ttx4 = 0.0f; GLfloat ttdx4 = 0.005f;
int moveNum = 0;
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

GLuint cubeVAO, cubeVBO_position, cubeVBO_color, cubeEBO;
GLuint pyramidVAO, pyramidVBO_position, pyramidVBO_color, pyramidEBO;

Model cubeModel, pyramidModel;

void InitBufferForCube() {
	// 인덱스 데이터
	std::vector<unsigned int> indices;

	for (const auto& face : cubeModel.faces) {
		indices.push_back(face.v1);
		indices.push_back(face.v2);
		indices.push_back(face.v3);
	}

	// 색상 데이터 설정
	std::vector<glm::vec3> vertexColor = {
		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.0f, 0.5f), glm::vec3(0.5f, 0.5f, 0.0f),
		glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.5f, 0.5f, 1.0f),
		glm::vec3(1.0f, 0.5f, 0.5f), glm::vec3(1.0f, 0.7f, 0.3f), glm::vec3(0.3f, 1.0f, 0.5f), glm::vec3(0.7f, 0.3f, 1.0f),
		glm::vec3(0.9f, 0.6f, 0.1f), glm::vec3(0.1f, 0.9f, 0.6f), glm::vec3(0.6f, 0.1f, 0.9f), glm::vec3(0.2f, 0.8f, 0.4f),
		glm::vec3(0.8f, 0.4f, 0.2f), glm::vec3(0.2f, 0.4f, 0.8f), glm::vec3(0.4f, 0.8f, 0.2f), glm::vec3(1.0f, 1.0f, 1.0f)
	};

	// VAO 설정
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	// VBO 설정 (버텍스 데이터)
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

	// 색상 데이터 설정
	std::vector<glm::vec3> vertexColor = {
		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.0f, 0.5f), 
		glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f),
		glm::vec3(0.9f, 0.6f, 0.1f), glm::vec3(0.1f, 0.9f, 0.6f), glm::vec3(0.6f, 0.1f, 0.9f),
	};

	// VAO 설정
	glGenVertexArrays(1, &pyramidVAO);
	glBindVertexArray(pyramidVAO);

	// VBO 설정 (버텍스 데이터)
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

	// 배경색을 흰색으로 설정 (R, G, B, A: 각각 1.0f로 설정하면 흰색)
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 흰색 배경

	// 셰이더 컴파일
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgram = make_shaderProgram();

	// OBJ 파일 로드
	read_obj_file("cube.obj", &cubeModel);
	read_obj_file("pyramid.obj", &pyramidModel);

	// 버퍼 초기화
	InitBufferForCube();
	InitBufferForPyramid();
	InitBufferForAxes();  // 축 버퍼 초기화 추가
}

GLvoid drawScene() {
	// 컬러 버퍼와 깊이 버퍼를 지웁니다
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	//---------------------------------------------------------------
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f)); //--- z축으로 -3만큼 이동 -> 카메라를 z축으로 3만큼 이동한 효과
	unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform"); //--- 버텍스 세이더에서 viewTransform 변수위치
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]); //--- viewTransform 변수에 변환값 적용하기
	//---------------------------------------------------------------
	if(p){
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);  // 직각 투영 설정
		unsigned int projectionLocation = glGetUniformLocation(shaderProgram, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
	else {
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- 투영 공간 설정: fovy, aspect, near, far
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0)); //--- 공간을 z축 이동
		unsigned int projectionLocation = glGetUniformLocation(shaderProgram, "projectionTransform"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
	//---------------------------------------------------------------

		//0
		glm::mat4 Ry = glm::mat4(1.0f);
		glm::mat4 Rx = glm::mat4(1.0f);
		glm::mat4 RR = glm::mat4(1.0f);
		Ry = glm::rotate(Ry, glm::radians(-20.0f + dY), glm::vec3(0.0, 1.0, 0.0));
		Rx = glm::rotate(Rx, glm::radians(-10.0f), glm::vec3(1.0, 0.0, 0.0));
		RR = Rx * Ry;
	//---------------------------------------------------------------
		//1
		glm::mat4 Rx1 = glm::mat4(1.0f);
		glm::mat4 Tz1 = glm::mat4(1.0f);
		glm::mat4 H1 = glm::mat4(1.0f);
		Rx1 = glm::rotate(Rx1, glm::radians(-rx1), glm::vec3(1.0, 0.0, 0.0));
		Tz1 = glm::translate(Tz1, glm::vec3(0.0, 0.0, -tz1));
		H1 = RR * Tz1 * Rx1;
		//2
		glm::mat4 Ty2 = glm::mat4(1.0f);
		glm::mat4 H2 = glm::mat4(1.0f);
		Ty2 = glm::translate(Ty2, glm::vec3(0.0, ty2, 0.0));
		H2 = RR * Ty2;
		//3
		glm::mat4 S3 = glm::mat4(1.0f);
		glm::mat4 H3 = glm::mat4(1.0f);
		S3 = glm::scale(S3, glm::vec3(s3, s3, s3));
		H3 = RR * S3;
		//5
		glm::mat4 Rx5 = glm::mat4(1.0f);
		glm::mat4 Ty51 = glm::mat4(1.0f);
		glm::mat4 Ty52 = glm::mat4(1.0f);
		glm::mat4 H5 = glm::mat4(1.0f);
		Ty51 = glm::translate(Ty51, glm::vec3(0.0, ty5, 0.0));
		Ty52 = glm::translate(Ty52, glm::vec3(0.0, -ty5, 0.0));
		Rx5 = glm::rotate(Rx5, glm::radians(rx5), glm::vec3(1.0, 0.0, 0.0));
		H5 = RR * Ty52 * Rx5 * Ty51;
	//---------------------------------------------------------------
		//1
		glm::mat4 RRx1 = glm::mat4(1.0f);
		glm::mat4 TTz11 = glm::mat4(1.0f);
		glm::mat4 TTz12 = glm::mat4(1.0f);
		RRx1 = glm::rotate(RRx1, glm::radians(-rrx1), glm::vec3(1.0, 0.0, 0.0));
		TTz11 = glm::translate(TTz11, glm::vec3(0.0, 0.0,0.3f));
		TTz12 = glm::translate(TTz12, glm::vec3(0.0, 0.0, -0.3f));
		glm::mat4 h1 = glm::mat4(1.0f);
		h1 = RR * TTz12 * RRx1 * TTz11;
		//2
		glm::mat4 RRz2 = glm::mat4(1.0f);
		glm::mat4 TTx21 = glm::mat4(1.0f);
		glm::mat4 TTx22 = glm::mat4(1.0f);
		RRz2 = glm::rotate(RRz2, glm::radians(rrz2), glm::vec3(0.0, 0.0, 1.0));
		TTx21 = glm::translate(TTx21, glm::vec3(0.3f, 0.0, 0.0));
		TTx22 = glm::translate(TTx22, glm::vec3(-0.3f,0.0, 0.0));
		glm::mat4 h2 = glm::mat4(1.0f);
		h2 = RR * TTx22 * RRz2 * TTx21;
		//3
		glm::mat4 RRx3 = glm::mat4(1.0f);
		glm::mat4 TTz31 = glm::mat4(1.0f);
		glm::mat4 TTz32 = glm::mat4(1.0f);
		RRx3 = glm::rotate(RRx3, glm::radians(rrx3), glm::vec3(1.0, 0.0, 0.0));
		TTz31 = glm::translate(TTz31, glm::vec3(0.0, 0.0, -0.3f));
		TTz32 = glm::translate(TTz32, glm::vec3(0.0, 0.0, 0.3f));
		glm::mat4 h3 = glm::mat4(1.0f);
		h3 = RR * TTz32 * RRx3 * TTz31;
		//4
		glm::mat4 RRz4 = glm::mat4(1.0f);
		glm::mat4 TTx41 = glm::mat4(1.0f);
		glm::mat4 TTx42 = glm::mat4(1.0f);
		RRz4 = glm::rotate(RRz4, glm::radians(-rrz4), glm::vec3(0.0, 0.0, 1.0));
		TTx41 = glm::translate(TTx41, glm::vec3(-0.3f, 0.0, 0.0));
		TTx42 = glm::translate(TTx42, glm::vec3(0.3f, 0.0, 0.0));
		glm::mat4 h4 = glm::mat4(1.0f);
		h4 = RR * TTx42 * RRz4 * TTx41;

	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(RR)); //--- modelTransform 변수에 변환 값 적용하기

	glFrontFace(GL_CCW);
	if (h)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	drawAxes();


	// VAO 바인딩 및 그리기
	glBindVertexArray(cubeVAO);
	
	if(c){
		//1
		modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H1));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//2
		modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H2));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * 1));
		//4
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * 3));
		//3
		modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H3));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * 2));
		//5
		modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H5));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * 4));
		//6
		modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(RR));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 6 * 5));
	}
	else {
		glBindVertexArray(pyramidVAO);
		modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(h1));
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		
		modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(h2));
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 3 * 1));

		modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(h3));
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 3 * 2));

		modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(h4));
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 3 * 3));

		modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(RR));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 3 * 4));
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
	glutPostRedisplay(); // 화면 갱신

	if (Y_move)
		dY += 5;
	if (t)
		rx5 += 5;
	if (f) {
		rx1 += drx1;
		tz1 += dtz1;
		if (rx1 > 90 || rx1 < 0) {
			drx1 *= -1;
			dtz1 *= -1;
			f = false;
		}
	}
	if (s) {
		ty2 += dty2;
		if (ty2 <= 0.0f || ty2>=0.6f) {
			dty2 *= -1;
			s = false;
		}
	}
	if (b) {
		s3 -= sd3;
		if (s3 >= 1.0f || s3 <= 0.0f) {
			sd3 *= -1;
			b = false;
		}
	}

	if (o) {
		rrx1 += rrdx1;
		ttz1 += ttdz1;
		
		rrx3 += rrdx3;
		ttz3 += ttdz3;

		rrz2 += rrdz2;
		ttx2 += ttdx2;

		rrz4 += rrdz4;
		ttx4 += ttdx4;

		if (rrz4 >= 231.2f || rrz4 <= 0.0f) {
			rrdx1 *= -1;
			ttdz1 *= -1;
			rrdz2 *= -1;
			ttdx2 *= -1;
			rrdx3 *= -1;
			ttdz3 *= -1;
			rrdz4 *= -1;
			ttdx4 *= -1;
			o = false;
		}
	}

	if (r) {
		switch (moveNum) {
		case 0:
			rrx1 += rrdx1;
			ttz1 += ttdz1;
			if (rrx1 >= 115.0f || rrx1 <= 0.0f) {
				rrdx1 *= -1;
				ttdz1 *= -1;
				++moveNum;
			}
			break;
		case 1:
			rrz2 += rrdz2;
			ttx2 += ttdx2;
			if (rrz2 >= 115.0f || rrz2 <= 0.0f) {
				rrdz2 *= -1;
				ttdx2 *= -1;
				++moveNum;
			}
			break;
		case 2:
			rrx3 += rrdx3;
			ttz3 += ttdz3;
			if (rrx3 >= 115.0f || rrx3 <= 0.0f) {
				rrdx3 *= -1;
				ttdz3 *= -1;
				++moveNum;
			}
			break;
		case 3:
			rrz4 += rrdz4;
			ttx4 += ttdx4;
			if (rrz4 >= 115.0f || rrz4 <= 0.0f) {
				rrdz4 *= -1;
				ttdx4 *= -1;
				r = false;
			}
			break;
		}
	}

	glutTimerFunc(60, TimerFunction, 1); // 타이머 함수 호출
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'h':
		h = !h;
		break;
	case 'y':
		Y_move = !Y_move;
		break;
	case 't':
		c = true;
		t = !t;
		break;
	case 'f':
		c = true;
		f = !f;
		break;
	case 's':
		c = true;
		s = !s;
		break;
	case 'b':
		c = true;
		b = !b;
		break;
	case 'o':
		c = false;
		o = !o;
		break;
	case 'r':
		c = false;
		r = !r;
		moveNum = 0;
		break;
	case 'p':
		p = !p;
		break;
	}

	//InitOpenGL();
	glutPostRedisplay(); //화면 갱신
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
	glutTimerFunc(60, TimerFunction, 1); // 타이머 함수 설정
	glutMainLoop(); //--- 이벤트 처리 시작
}