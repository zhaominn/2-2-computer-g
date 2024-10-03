#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <gl/glew.h>
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <vector>
#include <random>

GLvoid Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);

//----------------------------------
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);

GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

int rememberShape[10]; //0점 1선 2삼각형 3사각형
int now = 0;
int shapeIndex = -1;

int selectindex = -1;
int moveindex = -1;
int movenum = 0;
int moveShapenum;

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

GLuint VAO, VBO;

std::vector<float> vertexData;

void makeVertexData(int numVertices, GLfloat x, GLfloat y) {
	float r, g, b;
	switch (numVertices) {
	case 0:
		vertexData.push_back(x);
		vertexData.push_back(y);
		vertexData.push_back(0);
		r = (rand() % 10) / 10.0f;
		g = (rand() % 10) / 10.0f;
		b = (rand() % 10) / 10.0f;
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);
		break;
	case 1:
		//정점
		vertexData.push_back(x - 0.1);
		vertexData.push_back(y);
		vertexData.push_back(0);
		r = (rand() % 10) / 10.0f;
		g = (rand() % 10) / 10.0f;
		b = (rand() % 10) / 10.0f;
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);

		vertexData.push_back(x + 0.1);
		vertexData.push_back(y);
		vertexData.push_back(0);
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);
		break;
	case 2:
		//정점
		vertexData.push_back(x);
		vertexData.push_back(y);
		vertexData.push_back(0);
		r = (rand() % 10) / 10.0f;
		g = (rand() % 10) / 10.0f;
		b = (rand() % 10) / 10.0f;
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);

		vertexData.push_back(x - 0.1);
		vertexData.push_back(y - 0.1);
		vertexData.push_back(0);
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);

		vertexData.push_back(x + 0.1);
		vertexData.push_back(y - 0.1);
		vertexData.push_back(0);
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);
		break;
	case 3:
		vertexData.push_back(x - 0.1);
		vertexData.push_back(y);
		vertexData.push_back(0);
		r = (rand() % 10) / 10.0f;
		g = (rand() % 10) / 10.0f;
		b = (rand() % 10) / 10.0f;
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);

		vertexData.push_back(x - 0.1);
		vertexData.push_back(y - 0.2);
		vertexData.push_back(0);
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);

		vertexData.push_back(x + 0.1);
		vertexData.push_back(y);
		vertexData.push_back(0);
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);

		vertexData.push_back(x + 0.1);
		vertexData.push_back(y - 0.2);
		vertexData.push_back(0);
		vertexData.push_back(r);
		vertexData.push_back(g);
		vertexData.push_back(b);
		break;
	}
}

void InitBuffer()
{
	//--- VAO 객체 생성 및 바인딩
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//--- vertex data 저장을 위한 VBO 생성 및 바인딩.
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//--- vertex data 데이터 입력.
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

	//--- 위치 속성: 속성 위치 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//--- 색상 속성: 속성 위치 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 배경색을 흰색으로 설정
	glClear(GL_COLOR_BUFFER_BIT); // 화면을 지움

	glUseProgram(shaderProgramID); // 셰이더 프로그램 활성화
	glBindVertexArray(VAO);  // VAO 바인딩

	int vertexOffset = 0;
	for (int i = 0; i < now; ++i) {
		switch (rememberShape[i]) {
		case 0: // 점
			glPointSize(5.0); // 점 크기 설정
			glDrawArrays(GL_POINTS, vertexOffset, 1);
			vertexOffset += 1;
			break;
		case 1: // 선
			glDrawArrays(GL_LINES, vertexOffset, 2);
			vertexOffset += 2;
			break;
		case 2: // 삼각형
			glDrawArrays(GL_TRIANGLES, vertexOffset, 3);
			vertexOffset += 3;
			break;
		case 3: // 사각형 (두 개의 삼각형으로 그리기)
			glDrawArrays(GL_TRIANGLE_STRIP, vertexOffset, 4);
			vertexOffset += 4;
			break;
		default:
			break;
		}
	}
	glutSwapBuffers(); // 더블 버퍼링을 사용해 화면을 갱신
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

	// X 좌표 변환: 0 ~ windowWidth -> -1.0 ~ 1.0
	X = (static_cast<float>(x) / static_cast<float>(windowWidth)) * 2.0f - 1.0f;

	// Y 좌표 변환: 0 ~ windowHeight -> 1.0 ~ -1.0 (OpenGL의 Y축은 위쪽이 +1)
	Y = 1.0f - (static_cast<float>(y) / static_cast<float>(windowHeight)) * 2.0f;
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); //--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); //--- 윈도우의 위치 지정
	glutInitWindowSize(800, 600); //--- 윈도우의 크기 지정
	glutCreateWindow("짱오브더짱짱짱짱짱이되고싶다"); //--- 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();


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
	case 'p': // 점
		shapeIndex = 0;
		break;
	case 'l':
		shapeIndex = 1;
		break;
	case 't':
		shapeIndex = 2;
		break;
	case 'r':
		shapeIndex = 3;
		break;
	case 'w':
		selectindex = rand() % now;
		moveindex = 0;
		movenum = 0;
		moveShapenum = 0;
		for (int i = 0; i < selectindex; ++i) {
			moveShapenum += 6 * (rememberShape[i] + 1);
		}
		break;
	case 'a':
		selectindex = rand() % now;
		moveindex = 1;
		movenum = 0;
		moveShapenum = 0;
		for (int i = 0; i < selectindex; ++i) {
			moveShapenum += 6 * (rememberShape[i] + 1);
		}
		break;
	case 's':
		selectindex = rand() % now;
		moveindex = 2;
		movenum = 0;
		moveShapenum = 0;
		for (int i = 0; i < selectindex; ++i) {
			moveShapenum += 6 * (rememberShape[i] + 1);
		}
		break;
	case 'd':
		selectindex = rand() % now;
		moveindex = 3;
		movenum = 0;
		moveShapenum = 0;
		for (int i = 0; i < selectindex; ++i) {
			moveShapenum += 6 * (rememberShape[i] + 1);
		}
		break;
	case 'c':
		vertexData.clear();
		now = 0;
		break;
	}
	InitBuffer(); //버퍼 초기화
	glutPostRedisplay(); //화면 refresh
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			GLfloat X, Y;
			ScreenToOpenGL(x, y, X, Y); // 좌표 변환	

			makeVertexData(shapeIndex, X, Y);
			if (now != 10) {
				rememberShape[now] = shapeIndex; // 점
				++now;
			}
		}
	}
	InitBuffer(); //버퍼 초기화
	glutPostRedisplay(); //화면 갱신
}

void TimerFunction(int value)
{
	switch (moveindex) {
	case -1: break;
	case 0: // 위로 이동
		for (int i = 0; i < (rememberShape[selectindex] + 1); ++i) {
			vertexData[moveShapenum + 1 + 6 * i] += 0.01f; // Y축 좌표를 증가시킴
		}
		++movenum;
		if (movenum == 10)
			moveindex = -1;
		break;
	case 1: // 왼쪽으로 이동
		for (int i = 0; i < (rememberShape[selectindex] + 1); ++i) {
			vertexData[moveShapenum + 0 + 6 * i] -= 0.01f; // X축 좌표를 감소시킴
		}
		++movenum;
		if (movenum == 10)
			moveindex = -1;
		break;
	case 2: // 아래로 이동
		for (int i = 0; i < (rememberShape[selectindex] + 1); ++i) {
			vertexData[moveShapenum + 1 + 6 * i] -= 0.01f; // Y축 좌표를 감소시킴
		}
		++movenum;
		if (movenum == 10)
			moveindex = -1;
		break;
	case 3: // 오른쪽으로 이동
		for (int i = 0; i < (rememberShape[selectindex] + 1); ++i) {
			vertexData[moveShapenum + 0 + 6 * i] += 0.01f; // X축 좌표를 증가시킴
		}
		++movenum;
		if (movenum == 10)
			moveindex = -1;
		break;
	}

	InitBuffer(); // 버퍼 초기화
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}