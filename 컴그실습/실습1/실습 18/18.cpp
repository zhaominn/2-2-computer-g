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

GLfloat ry1;
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

GLuint circleVAO, circleVBO_position, circleVBO_color;

void InitBufferForCircle() {
	// 축의 정점 데이터
	std::vector<glm::vec3> circleVertices;
	for (int i = 0; i < 360; i++) { 
	double angle = i * 3.141592 / 180;
	GLfloat x = 0.6 * cos(angle);
	GLfloat z = 0.6* sin(angle);
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

void InitBufferForSphere(float radius = 0.15f, int sectors = 36, int stacks = 18) {
	// 정점 데이터 생성
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

	// 인덱스 데이터 생성
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

	// 색상 데이터 설정
	std::vector<glm::vec3> vertexColor(sphereVertices.size(), glm::vec3(0.5f, 0.5f, 1.0f)); // 파란색 계열

	// VAO 설정
	glGenVertexArrays(1, &sphereVAO);
	glBindVertexArray(sphereVAO);

	// VBO 설정 (버텍스 데이터)
	glGenBuffers(1, &sphereVBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO_position);
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(glm::vec3), &sphereVertices[0], GL_STATIC_DRAW);

	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(pAttribute);

	// VBO 설정 (색상 데이터)
	glGenBuffers(1, &sphereVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(glm::vec3), &vertexColor[0], GL_STATIC_DRAW);

	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(cAttribute);

	// EBO 설정 (인덱스 데이터)
	glGenBuffers(1, &sphereEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);

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
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f)); //--- z축으로 -3만큼 이동 -> 카메라를 z축으로 3만큼 이동한 효과
	unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform"); //--- 버텍스 세이더에서 viewTransform 변수위치
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]); //--- viewTransform 변수에 변환값 적용하기
	//---------------------------------------------------------------
	glm::mat4 projection = glm::mat4(1.0f);
	if (p) {
		projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);  // 직각 투영 설정
	}
	else {
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- 투영 공간 설정: fovy, aspect, near, far
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0)); //--- 공간을 z축 이동
	}
	unsigned int projectionLocation = glGetUniformLocation(shaderProgram, "projectionTransform");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	//---------------------------------------------------------------
	glm::mat4 Rx = glm::mat4(1.0f);
	glm::mat4 Ry = glm::mat4(1.0f);
	glm::mat4 Rz= glm::mat4(1.0f);
	glm::mat4 T1 = glm::mat4(1.0f);
	glm::mat4 RR = glm::mat4(1.0f);

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
	H11 = RR *Ry1* Tx1*S1;
	H12 = RR * T2 * Ry1 * Tx1 * S1 * S1;

	glm::mat4 Rz21 = glm::mat4(1.0f);
	glm::mat4 H20 = glm::mat4(1.0f);
	glm::mat4 H21 = glm::mat4(1.0f);
	glm::mat4 H22 = glm::mat4(1.0f);
	Rz21 = glm::rotate(Rz21, glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
	H20 = RR * Rz21;
	H21 = H20 * Ry1 * Tx1 * S1;
	//H22= H21에서 반지름만큼 왼쪽으로 이동. 근데 Rz21전에 이동하는...ㅜ
	H22 = RR * T2 * Rz21 * Ry1 * Tx1 * S1*S1;

	glm::mat4 Rz31 = glm::mat4(1.0f);
	glm::mat4 H30 = glm::mat4(1.0f);
	glm::mat4 H31 = glm::mat4(1.0f);
	glm::mat4 H32 = glm::mat4(1.0f);
	Rz31 = glm::rotate(Rz31, glm::radians(-45.0f), glm::vec3(0.0, 0.0, 1.0));
	H30 = RR * Rz31;
	H31 = H30 * Ry1 * Tx1 * S1;
	H32 = RR * T2 * Rz31 * Ry1 * Tx1 * S1 * S1;

	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(RR)); //--- modelTransform 변수에 변환 값 적용하기
	drawAxes();
	drawCircle();

	if(m)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//중심
	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0); // 구를 삼각형으로 렌더링
	
	
	//행성1
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H11));
	glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	drawCircle();
	//달1
	glBindVertexArray(sphereVAO);
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H12));
	glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
	
	//행성2
	glBindVertexArray(sphereVAO);
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H20));
	drawCircle();
	glBindVertexArray(sphereVAO);
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform"); 
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H21));
	glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
	//달2
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H22));
	glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

	//행성3
	glBindVertexArray(sphereVAO);
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H30));
	drawCircle();
	glBindVertexArray(sphereVAO);
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H31));
	glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

	//달
	modelLocation = glGetUniformLocation(shaderProgram, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(H32));
	glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
	
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

	ry1 += 1.0f;
	angle+=0.1f;
	if (angle >= 360)
		angle = 0;

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
	default:
		break;
	}

	//InitOpenGL();
	glutPostRedisplay(); //화면 갱신
}

GLvoid SpecialKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		ty += 0.01;
		break;
	case GLUT_KEY_DOWN:
		ty -= 0.01;
		break;
	case GLUT_KEY_LEFT:
		tx -= 0.01;
		break;
	case GLUT_KEY_RIGHT:
		tx += 0.01;
		break;
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