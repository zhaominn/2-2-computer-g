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

#include <vector>
bool a = true;//면으로 그리기
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
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	//1
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	//2
	glm::vec3(1.0f, 0.3f, 0.3f),
	glm::vec3(1.0f, 0.3f, 0.3f),
	glm::vec3(1.0f, 0.3f, 0.3f),
	//3
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),
	//4
	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),
};

std::vector<glm::vec3> vertexPosition = {
	glm::vec3(1.0f,0.0f,0.0f),
	glm::vec3(-1.0f,0.0f,0.0f),
	glm::vec3(0.0f,1.0f,0.0f),
	glm::vec3(0.0f,-1.0f,0.0f),
	//1
	glm::vec3(-0.5f,0.6f,0.0f),
	glm::vec3(-0.4f,0.4f,0.0f),
	glm::vec3(-0.6f,0.4f,0.0f),
	//2
	glm::vec3(0.5f,0.6f,0.0f),
	glm::vec3(0.4f,0.4f,0.0f),
	glm::vec3(0.6f,0.4f,0.0f),
	//3
	glm::vec3(-0.5f,-0.4f,0.0f),
	glm::vec3(-0.4f,-0.6f,0.0f),
	glm::vec3(-0.6f,-0.6f,0.0f),
	//4
	glm::vec3(0.5f,-0.4f,0.0f),
	glm::vec3(0.4f,-0.6f,0.0f),
	glm::vec3(0.6f,-0.6f,0.0f),
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

bool valid[4][3];
int rightNum = 0;

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID); // 셰이더 프로그램 활성화
	glBindVertexArray(VAO);  // VAO 바인딩

	glUseProgram(shaderProgramID);

	if(a){
		glDrawArrays(GL_LINES, 0, 2);
		glDrawArrays(GL_LINES, 2, 2);
		glDrawArrays(GL_TRIANGLES, 4, 3);
		glDrawArrays(GL_TRIANGLES, 7, 3);
		glDrawArrays(GL_TRIANGLES, 10, 3);
		glDrawArrays(GL_TRIANGLES, 13, 3);

		for (int i = 0; i < rightNum; ++i) {
			glDrawArrays(GL_TRIANGLES, 16 + 3 * i, 3);
		}
	}
	else {
		glDrawArrays(GL_LINES, 0, 2);
		glDrawArrays(GL_LINES, 2, 2);
		glDrawArrays(GL_LINE_LOOP, 4, 3);
		glDrawArrays(GL_LINE_LOOP, 7, 3);
		glDrawArrays(GL_LINE_LOOP, 10, 3);
		glDrawArrays(GL_LINE_LOOP, 13, 3);

		for (int i = 0; i < rightNum; ++i) {
			glDrawArrays(GL_LINE_LOOP, 16 + 3 * i, 3);
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


void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); //--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); //--- 윈도우의 위치 지정
	glutInitWindowSize(800, 600); //--- 윈도우의 크기 지정
	glutCreateWindow("짱이되자^^"); //--- 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();

	InitBuffer(); // 버퍼 초기화

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 3; ++j)
			valid[i][j] = false;
	}

	glutDisplayFunc(drawScene); //--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); //--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutTimerFunc(100, TimerFunction, 1); // 타이머 함수 설정
	glutMouseFunc(Mouse);
	glutMainLoop(); //--- 이벤트 처리 시작

}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		a = true;
		break;
	case 'b':
		a = false;
		break;
	}
	InitBuffer(); //버퍼 초기화
	glutPostRedisplay(); //화면 갱신
}

void Mouse(int button, int state, int x, int y) {
	GLfloat X, Y;
	ScreenToOpenGL(x, y, X, Y); // 좌표 변환
	GLfloat r;
	GLfloat g;
	GLfloat b;
	glm::vec3 color;
	GLfloat size = (rand() % 20+1) / 100.0f;
	if (button == GLUT_LEFT_BUTTON) {	
		if (state == GLUT_DOWN) {
			if (X < 0 && Y>0) {
				vertexPosition[4] = { X,Y + size,0.0f };
				vertexPosition[5] = { X - size,Y - size,0.0f };
				vertexPosition[6] = { X + size,Y - size,0.0f };
				r = (rand() % 10) / 10.0f;
				g = (rand() % 10) / 10.0f;
				b = (rand() % 10) / 10.0f;
				vertexColor[4] = {r,g,b};
				vertexColor[5] = { r,g,b };
				vertexColor[6] = { r,g,b };
			}
			else if (X > 0 && Y > 0) {
				vertexPosition[7] = { X,Y + size,0.0f };
				vertexPosition[8] = { X - size,Y - size,0.0f };
				vertexPosition[9] = { X + size,Y - size,0.0f };
				r = (rand() % 10) / 10.0f;
				g = (rand() % 10) / 10.0f;
				b = (rand() % 10) / 10.0f;
				vertexColor[7] = { r,g,b };
				vertexColor[8] = { r,g,b };
				vertexColor[9] = { r,g,b };
			}
			else if (X < 0 && Y < 0) {
				vertexPosition[10] = { X,Y + size,0.0f };
				vertexPosition[11] = { X - size,Y - size,0.0f };
				vertexPosition[12] = { X + size,Y - size,0.0f };
				r = (rand() % 10) / 10.0f;
				g = (rand() % 10) / 10.0f;
				b = (rand() % 10) / 10.0f;
				vertexColor[10] = { r,g,b };
				vertexColor[11] = { r,g,b };
				vertexColor[12] = { r,g,b };
			}
			else if (X > 0 && Y < 0) {
				vertexPosition[13] = { X,Y + size,0.0f };
				vertexPosition[14] = { X - size,Y - size,0.0f };
				vertexPosition[15] = { X + size,Y - size,0.0f };
				r = (rand() % 10) / 10.0f;
				g = (rand() % 10) / 10.0f;
				b = (rand() % 10) / 10.0f;
				vertexColor[13] = { r,g,b };
				vertexColor[14] = { r,g,b };
				vertexColor[15] = { r,g,b };
			}
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		glm::vec3 vertex1;
		glm::vec3 vertex2;
		glm::vec3 vertex3;
		if (state == GLUT_DOWN) {
			if (X < 0 && Y>0) {
				for (int i = 0; i < 3; ++i) {
					if (!valid[0][i]) {
						vertex1 = { X,Y + size,0.0f };
						vertexPosition.push_back(vertex1);
						vertex2 = { X - size,Y - size,0.0f };
						vertexPosition.push_back(vertex2);
						vertex3 = { X + size,Y - size,0.0f };
						vertexPosition.push_back(vertex3);
						r = (rand() % 10) / 10.0f;
						g = (rand() % 10) / 10.0f;
						b = (rand() % 10) / 10.0f;
						color = {r,g,b};
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						valid[0][i] = true;
						++rightNum;
						break;
					}
				}
			}
			else if (X > 0 && Y > 0) {
				for (int i = 0; i < 3; ++i) {
					if (!valid[1][i]) {
						vertex1 = { X,Y + size,0.0f };
						vertexPosition.push_back(vertex1);
						vertex2 = { X - size,Y - size,0.0f };
						vertexPosition.push_back(vertex2);
						vertex3 = { X + size,Y - size,0.0f };
						vertexPosition.push_back(vertex3);
						r = (rand() % 10) / 10.0f;
						g = (rand() % 10) / 10.0f;
						b = (rand() % 10) / 10.0f;
						color = { r,g,b };
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						valid[1][i] = true;
						++rightNum;
						break;
					}
				}
			}
			else if (X < 0 && Y < 0) {
				for (int i = 0; i < 3; ++i) {
					if (!valid[2][i]) {
						vertex1 = { X,Y + size,0.0f };
						vertexPosition.push_back(vertex1);
						vertex2 = { X - size,Y - size,0.0f };
						vertexPosition.push_back(vertex2);
						vertex3 = { X + size,Y - size,0.0f };
						vertexPosition.push_back(vertex3);
						r = (rand() % 10) / 10.0f;
						g = (rand() % 10) / 10.0f;
						b = (rand() % 10) / 10.0f;
						color = { r,g,b };
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						valid[2][i] = true;
						++rightNum;
						break;
					}
				}
			}
			else if (X > 0 && Y < 0) {
				for (int i = 0; i < 3; ++i) {
					if (!valid[3][i]) {
						vertex1 = { X,Y + size,0.0f };
						vertexPosition.push_back(vertex1);
						vertex2 = { X - size,Y - size,0.0f };
						vertexPosition.push_back(vertex2);
						vertex3 = { X + size,Y - size,0.0f };
						vertexPosition.push_back(vertex3);
						r = (rand() % 10) / 10.0f;
						g = (rand() % 10) / 10.0f;
						b = (rand() % 10) / 10.0f;
						color = { r,g,b };
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						vertexColor.push_back(color);
						valid[3][i] = true;
						++rightNum;
						break;
					}
				}
			}
		}
	}
	InitBuffer(); //버퍼 초기화
	glutPostRedisplay(); //화면 갱신
}

void TimerFunction(int value)
{
	//glutTimerFunc(100, TimerFunction, 1); // 타이머 함수 호출
}