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
int mode = -1;
struct move {
	GLfloat dx;
	GLfloat dy;
	int moveNum;
	int mode;
	int num;
	int n;
};
move triangle[4];
const float pi = 3.14159265359f; // pi 값 정의
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
	glm::vec3(-0.6f,0.4f,0.0f),
	glm::vec3(-0.4f,0.4f,0.0f),
	//2
	glm::vec3(0.5f,0.6f,0.0f),
	glm::vec3(0.4f,0.4f,0.0f),
	glm::vec3(0.6f,0.4f,0.0f),
	//3
	glm::vec3(-0.5f,-0.4f,0.0f),
	glm::vec3(-0.6f,-0.6f,0.0f),
	glm::vec3(-0.4f,-0.6f,0.0f),
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

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID); // 셰이더 프로그램 활성화
	glBindVertexArray(VAO);  // VAO 바인딩

	glUseProgram(shaderProgramID);


	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_LINES, 2, 2);
	glDrawArrays(GL_TRIANGLES, 4, 3);
	glDrawArrays(GL_TRIANGLES, 7, 3);
	glDrawArrays(GL_TRIANGLES, 10, 3);
	glDrawArrays(GL_TRIANGLES, 13, 3);

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
	glutInitWindowSize(800, 800); //--- 윈도우의 크기 지정
	glutCreateWindow("짱이되자^^"); //--- 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();

	InitBuffer(); // 버퍼 초기화

	for (int i = 0; i < 4; ++i) {
		triangle[i].dx = -0.01f;
		triangle[i].dy = 0.01f;
		triangle[i].moveNum = 0;
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
	case '1':
		mode = 1;
		for (int i = 0; i < 4; ++i) {
			triangle[i] = { 0.01f,0.01f,0,0 };
		}
		{
			vertexPosition[4] = { -0.5f, 0.6f, 0.0f };
			vertexPosition[5] = { -0.6f, 0.4f, 0.0f };
			vertexPosition[6] = { -0.4f, 0.4f, 0.0f };
			vertexPosition[7] = { 0.5f, 0.6f, 0.0f };
			vertexPosition[8] = { 0.4f, 0.4f, 0.0f };
			vertexPosition[9] = { 0.6f, 0.4f, 0.0f };
			vertexPosition[10] = { -0.5f, -0.4f, 0.0f };
			vertexPosition[11] = { -0.6f, -0.6f, 0.0f };
			vertexPosition[12] = { -0.4f, -0.6f, 0.0f };
			vertexPosition[13] = { 0.5f, -0.4f, 0.0f };
			vertexPosition[14] = { 0.4f, -0.6f, 0.0f };
			vertexPosition[15] = { 0.6f, -0.6f, 0.0f };
		}
		break;
	case '2':
		mode = 2;
		for (int i = 0; i < 4; ++i) {
			triangle[i] = { 0.01f,0.01f,0,0 };
		}
		{
			vertexPosition[4] = { -0.5f, 0.6f, 0.0f };
			vertexPosition[5] = { -0.6f, 0.4f, 0.0f };
			vertexPosition[6] = { -0.4f, 0.4f, 0.0f };
			vertexPosition[7] = { 0.5f, 0.6f, 0.0f };
			vertexPosition[8] = { 0.4f, 0.4f, 0.0f };
			vertexPosition[9] = { 0.6f, 0.4f, 0.0f };
			vertexPosition[10] = { -0.5f, -0.4f, 0.0f };
			vertexPosition[11] = { -0.6f, -0.6f, 0.0f };
			vertexPosition[12] = { -0.4f, -0.6f, 0.0f };
			vertexPosition[13] = { 0.5f, -0.4f, 0.0f };
			vertexPosition[14] = { 0.4f, -0.6f, 0.0f };
			vertexPosition[15] = { 0.6f, -0.6f, 0.0f };
		}
		break;
	case '3':
		mode = 3;
		for (int i = 0; i < 4; ++i) {
			triangle[i] = { 0.01f,0.01f,0,0 - (i * 20) ,1,0 };
			vertexPosition[4 + 3 * i] = { 0.0f, 0.0f + 0.1f, 0.0f };
			vertexPosition[5 + 3 * i] = { 0.0f - 0.1f, 0.0f - 0.1f, 0.0f };
			vertexPosition[6 + 3 * i] = { 0.0f + 0.1f, 0.0f - 0.1f, 0.0f };
		}
		break;
	case '4':
		mode = 4;
		for (int i = 0; i < 4; ++i) {
			triangle[i] = { 0.01f,0.01f,0-i,0 };
			vertexPosition[4 + 3 * i] = { 0.0f, 0.0f + 0.1f, 0.0f };
			vertexPosition[5 + 3 * i] = { 0.0f - 0.1f, 0.0f - 0.1f, 0.0f };
			vertexPosition[6 + 3 * i] = { 0.0f + 0.1f, 0.0f - 0.1f, 0.0f };
		}
		break;
	}
	InitBuffer(); //버퍼 초기화
	glutPostRedisplay(); //화면 갱신
}

void Mouse(int button, int state, int x, int y) {
	GLfloat X, Y;
	ScreenToOpenGL(x, y, X, Y); // 좌표 변환


	InitBuffer(); //버퍼 초기화
	glutPostRedisplay(); //화면 갱신
}

void TimerFunction(int value)
{ //4~15
	switch (mode) {
	case 1:
		for (int i = 0; i < 4; ++i) {
			vertexPosition[4 + 3 * i].x += triangle[i].dx;
			vertexPosition[4 + 3 * i].y += triangle[i].dy;
			vertexPosition[5 + 3 * i].x += triangle[i].dx;
			vertexPosition[5 + 3 * i].y += triangle[i].dy;
			vertexPosition[6 + 3 * i].x += triangle[i].dx;
			vertexPosition[6 + 3 * i].y += triangle[i].dy;
			if ((vertexPosition[4 + 3 * i].y > 1) || vertexPosition[6 + 3 * i].y < -1) {
				triangle[i].dy *= -1;
				if (vertexPosition[5 + 3 * i].y == vertexPosition[6 + 3 * i].y) {
					vertexPosition[4 + 3 * i].x += 0.1f;
					vertexPosition[5 + 3 * i].y += 0.1f;
				}
				else {
					vertexPosition[4 + 3 * i].x -= 0.1f;
					vertexPosition[5 + 3 * i].y -= 0.1f;
				}
			}
			if ((vertexPosition[5 + 3 * i].x < -1) || (vertexPosition[6 + 3 * i].x > 1)) {
				triangle[i].dx *= -1;
				if (vertexPosition[5 + 3 * i].y == vertexPosition[6 + 3 * i].y) {
					vertexPosition[4 + 3 * i].x += 0.1f;
					vertexPosition[5 + 3 * i].y += 0.1f;
				}
				else {
					vertexPosition[4 + 3 * i].x -= 0.1f;
					vertexPosition[5 + 3 * i].y -= 0.1f;
				}
			}
		}
		break;
	case 2:
		for (int i = 0; i < 4; ++i) {
			if ((vertexPosition[5 + 3 * i].x <= -1) || (vertexPosition[6 + 3 * i].x >= 1)) {
				if (triangle[i].moveNum < 10) {
					vertexPosition[4 + 3 * i].y += triangle[i].dy;
					vertexPosition[5 + 3 * i].y += triangle[i].dy;
					vertexPosition[6 + 3 * i].y += triangle[i].dy;
					++triangle[i].moveNum;
				}
				else {
					if (vertexPosition[5 + 3 * i].y == vertexPosition[6 + 3 * i].y) {
						vertexPosition[4 + 3 * i].x += 0.1f;
						vertexPosition[5 + 3 * i].y += 0.1f;
					}
					else {
						vertexPosition[4 + 3 * i].x -= 0.1f;
						vertexPosition[5 + 3 * i].y -= 0.1f;
					}

					triangle[i].moveNum = 0;
					triangle[i].dx *= -1;
					vertexPosition[4 + 3 * i].x += 2 * triangle[i].dx;
					vertexPosition[5 + 3 * i].x += 2 * triangle[i].dx;
					vertexPosition[6 + 3 * i].x += 2 * triangle[i].dx;
				}
			}
			else {
				vertexPosition[4 + 3 * i].x += triangle[i].dx;
				vertexPosition[5 + 3 * i].x += triangle[i].dx;
				vertexPosition[6 + 3 * i].x += triangle[i].dx;
			}

			if ((vertexPosition[4 + 3 * i].y >= 1) || (vertexPosition[6 + 3 * i].y <= -1)) {
				if ((vertexPosition[5 + 3 * i].x <= -1) || (vertexPosition[6 + 3 * i].x >= 1)) {
					triangle[i].dy *= -1;
					if (vertexPosition[5 + 3 * i].y == vertexPosition[6 + 3 * i].y) {
						vertexPosition[4 + 3 * i].x += 0.1f;
						vertexPosition[5 + 3 * i].y += 0.1f;
					}
					else {
						vertexPosition[4 + 3 * i].x -= 0.1f;
						vertexPosition[5 + 3 * i].y -= 0.1f;
					}
				}
			}
		}
		break;
	case 3:
		for (int i = 0; i < 4; ++i) {
			switch (triangle[i].mode) {
			case 0:
				vertexPosition[4 + 3 * i].x += triangle[i].dx;
				vertexPosition[5 + 3 * i].x += triangle[i].dx;
				vertexPosition[6 + 3 * i].x += triangle[i].dx;
				++triangle[i].moveNum;
				break;
			case 1:
				vertexPosition[4 + 3 * i].y -= triangle[i].dy;
				vertexPosition[5 + 3 * i].y -= triangle[i].dy;
				vertexPosition[6 + 3 * i].y -= triangle[i].dy;
				++triangle[i].moveNum;
				break;
			case 2:
				vertexPosition[4 + 3 * i].x -= triangle[i].dx;
				vertexPosition[5 + 3 * i].x -= triangle[i].dx;
				vertexPosition[6 + 3 * i].x -= triangle[i].dx;
				++triangle[i].moveNum;
				break;
			case 3:
				vertexPosition[4 + 3 * i].y += triangle[i].dy;
				vertexPosition[5 + 3 * i].y += triangle[i].dy;
				vertexPosition[6 + 3 * i].y += triangle[i].dy;
				++triangle[i].moveNum;
				break;
			}

			if (triangle[i].moveNum % (15 * triangle[i].num) == 0) {
				if (triangle[i].mode == 1 || triangle[i].mode == 3)
					triangle[i].num++;
				triangle[i].mode++;
				if (triangle[i].mode == 4)
					triangle[i].mode = 0;

				

				if (triangle[i].mode >= 0) {
					if (triangle[i].n % 4 == 0) {
						vertexPosition[4 + 3 * i].x += 0.1f;
						vertexPosition[5 + 3 * i].y += 0.1f;
					}
					else if (triangle[i].n % 4 == 1) {
						vertexPosition[5 + 3 * i].y += 0.1f;
						vertexPosition[6 + 3 * i].x -= 0.1f;
					}
					else if (triangle[i].n % 4 == 2) {
						vertexPosition[4 + 3 * i].x -= 0.2f;
						vertexPosition[5 + 3 * i].y -= 0.2f;
						vertexPosition[6 + 3 * i].x += 0.1f;
						vertexPosition[6 + 3 * i].y += 0.1f;
					}
					else if (triangle[i].n % 4 == 3) {
						vertexPosition[4 + 3 * i].x += 0.1f;
						vertexPosition[6 + 3 * i].y -= 0.1f;
					}
					++triangle[i].n;
				}
			}
		}
		break;
	case 4:
		for (int i = 0; i < 4; ++i) {
			if(triangle[i].moveNum>0) {
				// 삼각형의 중심점 좌표 (이동 궤적을 적용)
				GLfloat x = (triangle[i].moveNum / 500.0f) * cos(triangle[i].moveNum * pi / 180.0f);
				GLfloat y = (triangle[i].moveNum / 500.0f) * sin(triangle[i].moveNum * pi / 180.0f);
				triangle[i].moveNum += 5; // 삼각형이 이동할 때의 각도 증가

				glm::vec3 center = glm::vec3(x, y, 0.0f); // 중심점 업데이트

				// 회전 각도 계산 (움직이는 각도만큼 회전)
				float angle = triangle[i].moveNum * pi / 180.0f; // 각도 값을 라디안으로 변환

				// 회전 변환 행렬 생성 (Z축을 기준으로 회전)
				glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

				// 삼각형의 각 정점에 회전 변환 적용 (중심점을 기준으로)
				glm::vec4 p1 = glm::vec4(glm::vec3(0.0f, 0.1f, 0.0f) - center, 1.0f); // 중심점 기준으로 이동
				glm::vec4 p2 = glm::vec4(glm::vec3(-0.1f, -0.1f, 0.0f) - center, 1.0f);
				glm::vec4 p3 = glm::vec4(glm::vec3(0.1f, -0.1f, 0.0f) - center, 1.0f);

				// 회전 변환 후 다시 중심점 위치로 이동
				vertexPosition[4 + 3 * i] = glm::vec3(rotationMatrix * p1) + center;
				vertexPosition[5 + 3 * i] = glm::vec3(rotationMatrix * p2) + center;
				vertexPosition[6 + 3 * i] = glm::vec3(rotationMatrix * p3) + center;
			}
			++triangle[i].moveNum;
		}
		break;
	}
	InitBuffer(); //버퍼 초기화
	glutPostRedisplay(); //화면 갱신
	glutTimerFunc(100, TimerFunction, 1); // 타이머 함수 호출
}