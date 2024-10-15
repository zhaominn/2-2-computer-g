#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <gl/glew.h>
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
GLvoid Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);


struct D {
	GLfloat dx;
	GLfloat dy;
	int moveNum;
};

float Sign(float x1, float y1, float x2, float y2, float x3, float y3);
int mode[15] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }; //움직임
D move[15] = {
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
	{0.01,0.01},
};
int shape[15] = { 1,1,1,2,2,2, 3, 3, 3, 4, 4, 4, 5, 5, 5 }; //꼭짓점 개수
int select;
#include <vector>
#include <random>
bool isDrag = true;//면으로 그리기
//----------------------------------
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);

GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체
//-----------------------------------
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

void make_vertexShaders()
{
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

GLuint make_shaderProgram()
{
	GLchar errorLog[512];
	GLint result;

	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기

	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기

	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능

	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}

	glUseProgram(shaderID); //--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
	return shaderID;
}

std::vector<glm::vec3> vertexColor = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f)
};

std::vector<glm::vec3> vertexPosition = {
	glm::vec3(1.0f,0.0f,0.0f),
	glm::vec3(1.0f,0.0f,0.0f)
};

GLuint VAO, VBO_position, VBO_color;

void InitBuffer()
{
	//--- Vertex Array Object 생성
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//--- 위치 속성
	glGenBuffers(1, &VBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glBufferData(GL_ARRAY_BUFFER, vertexPosition.size() * sizeof(glm::vec3), glm::value_ptr(vertexPosition[0]), GL_STATIC_DRAW);

	//--- 색상 속성
	glGenBuffers(1, &VBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertexColor.size() * sizeof(glm::vec3), glm::value_ptr(vertexColor[0]), GL_STATIC_DRAW);

	//--- vPos 속성 변수에 값을 저장
	GLint pAttribute = glGetAttribLocation(shaderProgramID, "vPos");
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(pAttribute);

	//--- vColor 속성 변수에 값을 저장
	GLint cAttribute = glGetAttribLocation(shaderProgramID, "vColor");
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID); // 셰이더 프로그램 활성화
	glBindVertexArray(VAO);  // VAO 바인딩

	glDrawArrays(GL_LINES, 0, 2);

	for (int i = 0; i < 15; ++i) {
		switch (shape[i]) {
		case 1:
			glDrawArrays(GL_TRIANGLES, 2 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 3 + 6 * i, 3);
			break;
		case 2:
			glDrawArrays(GL_LINES, 2 + 6 * i, 2);
			break;
		case 3:
			glDrawArrays(GL_TRIANGLES, 2 + 6 * i, 3);
			break;
		case 4:
			glDrawArrays(GL_TRIANGLES, 2 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 3 + 6 * i, 3);
			break;
		case 5:
			glDrawArrays(GL_TRIANGLES, 2 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 3 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 4 + 6 * i, 3);
			break;
		case 6:
			glDrawArrays(GL_TRIANGLES, 2 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 3 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 4 + 6 * i, 3);
			glDrawArrays(GL_TRIANGLES, 5 + 6 * i, 3);
			break;
		}
	}
	glutSwapBuffers(); // 화면에 출력하기
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}
// 스크린 좌표를 OpenGL 좌표로 변환하는 함수
void ScreenToOpenGL(int x, int y, GLfloat& X, GLfloat& Y) {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int windowWidth = viewport[2];
	int windowHeight = viewport[3];

	X = (static_cast<float>(x) / static_cast<float>(windowWidth)) * 2.0f - 1.0f;
	Y = 1.0f - (static_cast<float>(y) / static_cast<float>(windowHeight)) * 2.0f;
}

void makeShapes() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-9.0, 9.0);

	for (int i = 0; i < 3; ++i) { // 점
		GLfloat x = round(dis(gen) * 10) / 100.0f;
		GLfloat y = round(dis(gen) * 10) / 100.0f;
		GLfloat r = (rand() % 10) / 10.0f;
		GLfloat g = (rand() % 10) / 10.0f;
		GLfloat b = (rand() % 10) / 10.0f;

		glm::vec3 temp = { x - 0.01f,y + 0.01f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.01f,y + 0.01f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.01f,y - 0.01f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.01f,y - 0.01f,0.0f };
		vertexPosition.push_back(temp);

		temp = { x - 0.01f,y - 0.01f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.01f,y - 0.01f,0.0f };
		vertexPosition.push_back(temp);

		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
	}

	for (int i = 0; i < 3; ++i) { // 선
		GLfloat x = round(dis(gen) * 10) / 100.0f;
		GLfloat y = round(dis(gen) * 10) / 100.0f;
		GLfloat r = (rand() % 10) / 10.0f;
		GLfloat g = (rand() % 10) / 10.0f;
		GLfloat b = (rand() % 10) / 10.0f;

		glm::vec3 temp = { x,y,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.2f,y,0.0f };
		vertexPosition.push_back(temp);

		temp = { x + 0.2f,y,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.2f,y,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.2f,y,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.2f,y,0.0f };
		vertexPosition.push_back(temp);

		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
		temp = { r,g,b };
		vertexColor.push_back(temp);
	}

	for (int i = 0; i < 3; ++i) { // 삼각형
		GLfloat x = round(dis(gen) * 10) / 100.0f;
		GLfloat y = round(dis(gen) * 10) / 100.0f;
		GLfloat r = (rand() % 10) / 10.0f;
		GLfloat g = (rand() % 10) / 10.0f;
		GLfloat b = (rand() % 10) / 10.0f;

		glm::vec3 temp = { x,y + 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);

		temp = { x - 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);

		temp = { r,g,b };
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
	}

	for (int i = 0; i < 3; ++i) { // 사각형
		GLfloat x = round(dis(gen) * 10) / 100.0f;
		GLfloat y = round(dis(gen) * 10) / 100.0f;
		GLfloat r = (rand() % 10) / 10.0f;
		GLfloat g = (rand() % 10) / 10.0f;
		GLfloat b = (rand() % 10) / 10.0f;

		glm::vec3 temp = { x - 0.1f,y + 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.1f,y + 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);

		temp = { x + 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.1f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);

		temp = { r,g,b };
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
	}

	for (int i = 0; i < 3; ++i) { // 오각형
		GLfloat x = round(dis(gen) * 10) / 100.0f;
		GLfloat y = round(dis(gen) * 10) / 100.0f;
		GLfloat r = (rand() % 10) / 10.0f;
		GLfloat g = (rand() % 10) / 10.0f;
		GLfloat b = (rand() % 10) / 10.0f;

		glm::vec3 temp = { x - 0.1f, y + 0.01f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x - 0.06f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x, y + 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.06f,y - 0.1f,0.0f };
		vertexPosition.push_back(temp);
		temp = { x + 0.1f, y + 0.01f,0.0f };
		vertexPosition.push_back(temp);

		temp = { x + 0.1f, y + 0.01f,0.0f };
		vertexPosition.push_back(temp);

		temp = { r,g,b };
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
		vertexColor.push_back(temp);
	}

	InitBuffer(); //버퍼 초기화
	glutPostRedisplay(); //화면 갱신
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); //--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); //--- 윈도우의 위치 지정
	glutInitWindowSize(800, 800); //--- 윈도우의 크기 지정
	glutCreateWindow("짱이되자^^"); //--- 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();

	InitBuffer(); // 버퍼 초기화

	makeShapes();

	glutDisplayFunc(drawScene); //--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); //--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutTimerFunc(60, TimerFunction, 1); // 타이머 함수 설정
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMainLoop(); //--- 이벤트 처리 시작

}

GLvoid Keyboard(unsigned char key, int x, int y)
{

	InitBuffer(); //버퍼 초기화
	glutPostRedisplay(); //화면 갱신
}

float Sign(float x1, float y1, float x2, float y2, float x3, float y3) {
	return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
}

bool PointInTriangle(float x, float y, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
	// (x, y)가 삼각형 안에 있는지 판단
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = Sign(x, y, p0.x, p0.y, p1.x, p1.y);
	d2 = Sign(x, y, p1.x, p1.y, p2.x, p2.y);
	d3 = Sign(x, y, p2.x, p2.y, p0.x, p0.y);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

bool PointInRectangle(float x, float y, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	// 점이 사각형의 네 점 안에 있는지 확인
	return PointInTriangle(x, y, p0, p1, p2) || PointInTriangle(x, y, p2, p3, p0);
}

// 오각형 충돌 체크 함수
bool PointInPentagon(float x, float y, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
	return PointInTriangle(x, y, p0, p1, p2) || PointInTriangle(x, y, p1, p2, p3) || PointInTriangle(x, y, p2, p3, p4);
}

// 육각형 충돌 체크 함수
bool PointInHexagon(float x, float y, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 p5) {
	return PointInTriangle(x, y, p0, p1, p2) || PointInTriangle(x, y, p1, p2, p3) || PointInTriangle(x, y, p2, p3, p4) || PointInTriangle(x, y, p3, p4, p5);
}

void Mouse(int button, int state, int x, int y) {
	GLfloat X, Y;
	ScreenToOpenGL(x, y, X, Y); // 좌표 변환

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			isDrag = true;
			for (int i = 0; i < 15; ++i) {
				switch (shape[i]) {
					// 선택된 도형 검출
				case 1:
					if ((X < vertexPosition[5 + 6 * i].x) && (vertexPosition[4 + 6 * i].x < X)
						&& (Y < vertexPosition[2 + 6 * i].y) && (vertexPosition[4 + 6 * i].y < Y)) {
						select = i;
					}
					break;
				case 2:
					if (vertexPosition[2 + 6 * i].y == Y
						&& vertexPosition[2 + 6 * i].x < X && vertexPosition[3 + 6 * i].x > X) {
						select = i;
					}
					break;
				case 3:
					if (PointInTriangle(X, Y, vertexPosition[2 + 6 * i], vertexPosition[3 + 6 * i], vertexPosition[4 + 6 * i])) {
						select = i;
					}
					break;
				case 4:
					if ((X < vertexPosition[5 + 6 * i].x) && (vertexPosition[4 + 6 * i].x < X)
						&& (Y < vertexPosition[2 + 6 * i].y) && (vertexPosition[4 + 6 * i].y < Y)) {
						select = i;
					}
					break;
				case 5:
					if (PointInPentagon(X, Y, vertexPosition[2 + 6 * i], vertexPosition[3 + 6 * i], vertexPosition[4 + 6 * i], vertexPosition[5 + 6 * i], vertexPosition[6 + 6 * i])) {
						select = i;
					}
					break;
				case 6:
					if (PointInHexagon(X, Y, vertexPosition[2 + 6 * i], vertexPosition[3 + 6 * i], vertexPosition[4 + 6 * i], vertexPosition[5 + 6 * i], vertexPosition[6 + 6 * i], vertexPosition[7 + 6 * i])) {
						select = i;
					}
					break;
				}
			}
		}
		else if (state == GLUT_UP && isDrag) {
			isDrag = false;
			int selected = -1;

			for (int i = 0; i < 15; ++i) {
				if (i != select) {
					switch (shape[i]) {
						// 선택된 도형 검출
					case 1:
						if ((X < vertexPosition[5 + 6 * i].x) && (vertexPosition[4 + 6 * i].x < X)
							&& (Y < vertexPosition[2 + 6 * i].y) && (vertexPosition[4 + 6 * i].y < Y)) {
							selected = i;
						}
						break;
					case 2:
						switch (shape[select]) {
						case 1:
							if (PointInRectangle(vertexPosition[2 + 6 * i].x, vertexPosition[2 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * i])
								|| PointInRectangle(vertexPosition[3 + 6 * i].x, vertexPosition[3 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * i])) {
								selected = i;
							}
							break;
						case 3:
							if (PointInTriangle(vertexPosition[2 + 6 * i].x, vertexPosition[2 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select])
								|| PointInTriangle(vertexPosition[3 + 6 * i].x, vertexPosition[3 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select])) {
								selected = i;
							}
							break;
						case 4:
							if (PointInRectangle(vertexPosition[2 + 6 * i].x, vertexPosition[2 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * select])
								|| PointInRectangle(vertexPosition[3 + 6 * i].x, vertexPosition[3 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * select])) {
								selected = i;
							}
							break;
						case 5:
							if (PointInPentagon(vertexPosition[2 + 6 * i].x, vertexPosition[2 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * select], vertexPosition[6 + 6 * select])
								|| PointInPentagon(vertexPosition[3 + 6 * i].x, vertexPosition[3 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * select], vertexPosition[6 + 6 * select])) {
								selected = i;
							}
							break;
						case 6:
							if (PointInHexagon(vertexPosition[2 + 6 * i].x, vertexPosition[2 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * select], vertexPosition[6 + 6 * select], vertexPosition[7 + 6 * select])
								|| PointInHexagon(vertexPosition[3 + 6 * i].x, vertexPosition[3 + 6 * i].y, vertexPosition[2 + 6 * select], vertexPosition[3 + 6 * select], vertexPosition[4 + 6 * select], vertexPosition[5 + 6 * select], vertexPosition[6 + 6 * select], vertexPosition[7 + 6 * select])) {
								selected = i;
							}
							break;
						}
						break;
					case 3:
						if (PointInTriangle(X, Y, vertexPosition[2 + 6 * i], vertexPosition[3 + 6 * i], vertexPosition[4 + 6 * i])) {
							selected = i;
						}
						break;
					case 4:
						if ((X < vertexPosition[5 + 6 * i].x) && (vertexPosition[4 + 6 * i].x < X)
							&& (Y < vertexPosition[2 + 6 * i].y) && (vertexPosition[4 + 6 * i].y < Y)) {
							selected = i;
						}
						break;
					case 5:
						if (PointInPentagon(X, Y, vertexPosition[2 + 6 * i], vertexPosition[3 + 6 * i], vertexPosition[4 + 6 * i], vertexPosition[5 + 6 * i], vertexPosition[6 + 6 * i])) {
							selected = i;
						}
						break;
					case 6:
						if (PointInHexagon(X, Y, vertexPosition[2 + 6 * i], vertexPosition[3 + 6 * i], vertexPosition[4 + 6 * i], vertexPosition[5 + 6 * i], vertexPosition[6 + 6 * i], vertexPosition[7 + 6 * i])) {
							selected = i;
						}
						break;
					}
				}
			}

			if (selected != -1) {
				shape[selected] = (shape[select] + shape[selected]) % 7;
				if (shape[selected] == 0)
					shape[selected] = 1;
				shape[select] = -1;

				GLfloat r = (rand() % 10) / 10.0f;
				GLfloat g = (rand() % 10) / 10.0f;
				GLfloat b = (rand() % 10) / 10.0f;
				vertexColor[2 + 6 * selected] = { r,g,b };
				vertexColor[3 + 6 * selected] = { r,g,b };
				vertexColor[4 + 6 * selected] = { r,g,b };
				vertexColor[5 + 6 * selected] = { r,g,b };
				vertexColor[6 + 6 * selected] = { r,g,b };
				vertexColor[7 + 6 * selected] = { r,g,b };

				switch (shape[selected]) {
				case 1:
					vertexPosition[2 + 6 * selected] = { X - 0.01f,Y + 0.01f,0.0f };
					vertexPosition[3 + 6 * selected] = { X + 0.01f,Y + 0.01f,0.0f };
					vertexPosition[4 + 6 * selected] = { X - 0.01f,Y - 0.01f,0.0f };
					vertexPosition[5 + 6 * selected] = { X + 0.01f,Y - 0.01f,0.0f };
					break;
				case 2:
					vertexPosition[2 + 6 * selected] = { X - 0.1f,Y,0.0f };
					vertexPosition[3 + 6 * selected] = { X + 0.1f,Y,0.0f };
					break;
				case 3:
					vertexPosition[2 + 6 * selected] = { X,Y + 0.1f,0.0f };
					vertexPosition[3 + 6 * selected] = { X + 0.1f,Y - 0.1f,0.0f };
					vertexPosition[4 + 6 * selected] = { X - 0.1f,Y - 0.1f,0.0f };
					break;
				case 4:
					vertexPosition[2 + 6 * selected] = { X - 0.1f,Y + 0.1f,0.0f };
					vertexPosition[3 + 6 * selected] = { X + 0.1f,Y + 0.1f,0.0f };
					vertexPosition[4 + 6 * selected] = { X - 0.1f,Y - 0.1f,0.0f };
					vertexPosition[5 + 6 * selected] = { X + 0.1f,Y - 0.1f,0.0f };
					break;
				case 5:
					vertexPosition[2 + 6 * selected] = { X - 0.1f, Y + 0.01f,0.0f };
					vertexPosition[3 + 6 * selected] = { X - 0.06f,Y - 0.1f,0.0f };
					vertexPosition[4 + 6 * selected] = { X, Y + 0.1f,0.0f };
					vertexPosition[5 + 6 * selected] = { X + 0.06f,Y - 0.1f,0.0f };
					vertexPosition[6 + 6 * selected] = { X + 0.1f, Y + 0.01f,0.0f };
					break;
				case 6:
					vertexPosition[2 + 6 * selected] = { X - 0.05f,Y + 0.1f,0.0f };
					vertexPosition[3 + 6 * selected] = { X + 0.05f,Y + 0.1f,0.0f };
					vertexPosition[4 + 6 * selected] = { X - 0.1f,Y,0.0f };
					vertexPosition[5 + 6 * selected] = { X + 0.1f,Y,0.0f };
					vertexPosition[6 + 6 * selected] = { X - 0.05f,Y - 0.1f,0.0f };
					vertexPosition[7 + 6 * selected] = { X + 0.05f,Y - 0.1f,0.0f };
					break;
				}
				mode[selected] = rand() % 2 + 1;
			}
			select = -1;
		}


	}
	InitBuffer(); // 버퍼 초기화
	glutPostRedisplay(); // 화면 갱신
}

void Motion(int x, int y) {
	//마우스가움직이는중일때
	if (isDrag) {
		GLfloat X, Y;
		ScreenToOpenGL(x, y, X, Y); // 좌표 변환
		switch (shape[select]) {
		case 1:
			vertexPosition[2 + 6 * select] = { X - 0.01f,Y + 0.01f,0.0f };
			vertexPosition[3 + 6 * select] = { X + 0.01f,Y + 0.01f,0.0f };
			vertexPosition[4 + 6 * select] = { X - 0.01f,Y - 0.01f,0.0f };
			vertexPosition[5 + 6 * select] = { X + 0.01f,Y - 0.01f,0.0f };
			break;
		case 2:
			vertexPosition[2 + 6 * select] = { X - 0.1f,Y,0.0f };
			vertexPosition[3 + 6 * select] = { X + 0.1f,Y,0.0f };
			break;
		case 3:
			vertexPosition[2 + 6 * select] = { X ,Y + 0.1f,0.0f };
			vertexPosition[3 + 6 * select] = { X + 0.1f,Y - 0.1f,0.0f };
			vertexPosition[4 + 6 * select] = { X - 0.1f,Y - 0.1f,0.0f };
			break;
		case 4:
			vertexPosition[2 + 6 * select] = { X - 0.1f,Y + 0.1f,0.0f };
			vertexPosition[3 + 6 * select] = { X + 0.1f,Y + 0.1f,0.0f };
			vertexPosition[4 + 6 * select] = { X - 0.1f,Y - 0.1f,0.0f };
			vertexPosition[5 + 6 * select] = { X + 0.1f,Y - 0.1f,0.0f };
			break;
		case 5:
			vertexPosition[2 + 6 * select] = { X - 0.1f, Y + 0.01f,0.0f };
			vertexPosition[3 + 6 * select] = { X - 0.06f,Y - 0.1f,0.0f };
			vertexPosition[4 + 6 * select] = { X , Y + 0.1f,0.0f };
			vertexPosition[5 + 6 * select] = { X + 0.06f,Y - 0.1f,0.0f };
			vertexPosition[6 + 6 * select] = { X + 0.1f, Y + 0.01f,0.0f };
			break;
		case 6:
			vertexPosition[2 + 6 * select] = { X - 0.05f, Y + 0.1f,0.0f };
			vertexPosition[3 + 6 * select] = { X + 0.05f, Y + 0.1f,0.0f };
			vertexPosition[4 + 6 * select] = { X - 0.1f, Y ,0.0f };
			vertexPosition[5 + 6 * select] = { X + 0.1f, Y ,0.0f };
			vertexPosition[6 + 6 * select] = { X - 0.05f, Y - 0.1f ,0.0f };
			vertexPosition[7 + 6 * select] = { X + 0.05f, Y - 0.1f,0.0f };
			break;
		}
	}
	InitBuffer(); //버퍼 초기화
	glutPostRedisplay(); //화면 갱신
}


void TimerFunction(int value)
{
	for (int i = 0; i < 15; ++i) {
		if (mode[i] == 1) { // 지그재그 양옆
			if (move[i].moveNum == 0) {
				vertexPosition[2 + 6 * i].x += move[i].dx;
				vertexPosition[3 + 6 * i].x += move[i].dx;
				vertexPosition[4 + 6 * i].x += move[i].dx;
				vertexPosition[5 + 6 * i].x += move[i].dx;
				vertexPosition[6 + 6 * i].x += move[i].dx;
				vertexPosition[7 + 6 * i].x += move[i].dx;

				GLfloat M = vertexPosition[2 + 6 * i].x; // 초기값 설정
				GLfloat m = vertexPosition[2 + 6 * i].x; // 초기값 설정

				// 좌표들 중에서 최댓값과 최솟값을 찾기
				for (int j = 2; j <= 7; ++j) {
					GLfloat x = vertexPosition[j + 6 * i].x;
					if (x > M) {
						M = x; // 최댓값 갱신
					}
					if (x < m) {
						m = x; // 최솟값 갱신
					}
				}
				if (M >= 1.0 || m <= -1) {
					++move[i].moveNum;
					move[i].dx *= -1;
				}
			}
			else {
				vertexPosition[2 + 6 * i].y += move[i].dy;
				vertexPosition[3 + 6 * i].y += move[i].dy;
				vertexPosition[4 + 6 * i].y += move[i].dy;
				vertexPosition[5 + 6 * i].y += move[i].dy;
				vertexPosition[6 + 6 * i].y += move[i].dy;
				vertexPosition[7 + 6 * i].y += move[i].dy;

				GLfloat M = vertexPosition[2 + 6 * i].y; // 초기값 설정
				GLfloat m = vertexPosition[2 + 6 * i].y; // 초기값 설정

				// 좌표들 중에서 최댓값과 최솟값을 찾기
				for (int j = 2; j <= 7; ++j) {
					GLfloat y = vertexPosition[j + 6 * i].y;
					if (y > M) {
						M = y; // 최댓값 갱신
					}
					if (y < m) {
						m = y; // 최솟값 갱신
					}
				}
				if (M >= 1.0 || m <= -1) {
					++move[i].moveNum;
					move[i].dy *= -1;
				}

				++move[i].moveNum;
				if (move[i].moveNum == 15)
					move[i].moveNum = 0;
			}
		}
		else if (mode[i] == 2) {//
			vertexPosition[2 + 6 * i].x += move[i].dx;
			vertexPosition[3 + 6 * i].x += move[i].dx;
			vertexPosition[4 + 6 * i].x += move[i].dx;
			vertexPosition[5 + 6 * i].x += move[i].dx;
			vertexPosition[6 + 6 * i].x += move[i].dx;
			vertexPosition[7 + 6 * i].x += move[i].dx;
			vertexPosition[2 + 6 * i].y += move[i].dy;
			vertexPosition[3 + 6 * i].y += move[i].dy;
			vertexPosition[4 + 6 * i].y += move[i].dy;
			vertexPosition[5 + 6 * i].y += move[i].dy;
			vertexPosition[6 + 6 * i].y += move[i].dy;
			vertexPosition[7 + 6 * i].y += move[i].dy;

			GLfloat X = vertexPosition[2 + 6 * i].x; // 초기값 설정
			GLfloat x = vertexPosition[2 + 6 * i].x; // 초기값 설정
			GLfloat Y = vertexPosition[2 + 6 * i].y; // 초기값 설정
			GLfloat y = vertexPosition[2 + 6 * i].y; // 초기값 설정

			// 좌표들 중에서 최댓값과 최솟값을 찾기
			for (int j = 2; j <= 7; ++j) {
				GLfloat xx = vertexPosition[j + 6 * i].x;
				GLfloat yy = vertexPosition[j + 6 * i].y;
				if (xx > X) {
					X = xx; // 최댓값 갱신
				}
				if (xx < x) {
					x = xx; // 최솟값 갱신
				}
				if (yy > Y) {
					Y = yy; // 최댓값 갱신
				}
				if (yy < y) {
					y = yy; // 최솟값 갱신
				}
			}

			if (X >= 1.0 || x <= -1) {
				move[i].dx *= -1;
			}
			if (Y >= 1.0 || y <= -1) {
				move[i].dy *= -1;
			}
		}

	}
	InitBuffer(); // 버퍼 초기화
	glutPostRedisplay(); // 화면 갱신

	glutTimerFunc(60, TimerFunction, 1); // 타이머 함수 호출
}