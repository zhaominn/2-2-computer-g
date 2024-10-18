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


#include <vector>
#include <string>
bool isDrag = true;
//----------------------------------
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);

GLuint shaderProgram; //--- 세이더 프로그램 이름
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
	//정육면제
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),

	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),

	glm::vec3(1.0f, 0.2f, 0.2f),
	glm::vec3(1.0f, 0.2f, 0.2f),
	glm::vec3(1.0f, 0.2f, 0.2f),
	glm::vec3(1.0f, 0.2f, 0.2f),

	glm::vec3(1.0f, 0.8f, 0.5f),
	glm::vec3(1.0f, 0.8f, 0.5f),
	glm::vec3(1.0f, 0.8f, 0.5f),
	glm::vec3(1.0f, 0.8f, 0.5f),

	glm::vec3(1.0f, 0.2f, 0.8f),
	glm::vec3(1.0f, 0.2f, 0.8f),
	glm::vec3(1.0f, 0.2f, 0.8f),
	glm::vec3(1.0f, 0.2f, 0.8f),

	glm::vec3(1.0f, 0.5f, 0.8f),
	glm::vec3(1.0f, 0.5f, 0.8f),
	glm::vec3(1.0f, 0.5f, 0.8f),
	glm::vec3(1.0f, 0.5f, 0.8f),
	//정사면체
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.5f),

	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),
	glm::vec3(1.0f, 0.8f, 0.8f),

	glm::vec3(1.0f, 0.2f, 0.2f),
	glm::vec3(1.0f, 0.2f, 0.2f),
	glm::vec3(1.0f, 0.2f, 0.2f),

	glm::vec3(1.0f, 0.8f, 0.5f),
	glm::vec3(1.0f, 0.8f, 0.5f),
	glm::vec3(1.0f, 0.8f, 0.5f),
};

std::vector<glm::vec3> vertexPosition = {
	//정육면체
	glm::vec3(0.5f,0.5f,0.5f),
	glm::vec3(0.5f,-0.5f,0.5f),
	glm::vec3(-0.5f,0.5f,0.5f),
	glm::vec3(-0.5f,-0.5f,0.5f),

	glm::vec3(0.5f,0.5f,0.5f),
	glm::vec3(0.5f,0.5f,-0.5f),
	glm::vec3(-0.5f,0.5f,0.5f),
	glm::vec3(-0.5f,0.5f,-0.5f),

	glm::vec3(-0.5f,-0.5f,0.5f),
	glm::vec3(-0.5f,-0.5f,-0.5f),
	glm::vec3(-0.5f,0.5f,0.5f),
	glm::vec3(-0.5f,0.5f,-0.5f),

	glm::vec3(0.5f,-0.5f,0.5f),
	glm::vec3(0.5f,-0.5f,-0.5f),
	glm::vec3(0.5f,0.5f,0.5f),
	glm::vec3(0.5f,0.5f,-0.5f),

	glm::vec3(0.5f,-0.5f,0.5f),
	glm::vec3(0.5f,-0.5f,-0.5f),
	glm::vec3(-0.5f,-0.5f,0.5f),
	glm::vec3(-0.5f,-0.5f,-0.5f),

	glm::vec3(0.5f,0.5f,-0.5f),
	glm::vec3(0.5f,-0.5f,-0.5f),
	glm::vec3(-0.5f,0.5f,-0.5f),
	glm::vec3(-0.5f,-0.5f,-0.5f),
	//정사면체
	glm::vec3(0.5f,-0.5f,0.0f),
	glm::vec3(-0.5f,-0.5f,-0.5f),
	glm::vec3(0.0f,0.5f,0.0f),

	glm::vec3(0.5f,-0.5f,0.0f),
	glm::vec3(-0.5f,-0.5f,-0.5f),
	glm::vec3(-0.5f,-0.5f,0.5f),

	glm::vec3(-0.5f,-0.5f,0.5f),
	glm::vec3(-0.5f,-0.5f,-0.5f),
	glm::vec3(0.0f,+0.5f,0.0f),

	glm::vec3(-0.5f,-0.5f,0.5f),
	glm::vec3(0.5f,-0.5f,0.0f),
	glm::vec3(0.0f,+0.5f,0.0f),

};

GLuint VAO, VBO_position, VBO_color;
//-----------------------------------


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
	GLint pAttribute = glGetAttribLocation(shaderProgram, "vPos");
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(pAttribute);

	//--- vColor 속성 변수에 값을 저장
	GLint cAttribute = glGetAttribLocation(shaderProgram, "vColor");
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glVertexAttribPointer(cAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(cAttribute);
}


GLUquadricObj* qobj;
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glUseProgram(shaderProgram);
	glm::mat4 Ry = glm::mat4(1.0f); //--- z회전 행렬 선언
	glm::mat4 Rx = glm::mat4(1.0f); //--- x회전 행렬 선언
	glm::mat4 RR = glm::mat4(1.0f); //--- 합성 변환 행렬
	Ry = glm::rotate(Ry, glm::radians(10.0f), glm::vec3(0.0, 1.0, 0.0)); //--- y축에 대하여 회전 행렬
	Rx = glm::rotate(Rx, glm::radians(10.0f), glm::vec3(1.0, 0.0, 0.0)); //--- x축에 대하여 회전 행렬
	RR = Rx*Ry;
	unsigned int modelLocation = glGetUniformLocation(shaderProgram, "modelTransform"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(RR)); //--- modelTransform 변수에 변환 값 적용하기


	//---------- 도형 그리기
	glBindVertexArray(VAO);
	/*// 정육면체
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawArrays(GL_TRIANGLES, 1, 3);

	glDrawArrays(GL_TRIANGLES, 4, 3);
	glDrawArrays(GL_TRIANGLES, 5, 3);

	glDrawArrays(GL_TRIANGLES, 8, 3);
	glDrawArrays(GL_TRIANGLES, 9, 3);

	glDrawArrays(GL_TRIANGLES, 12, 3);
	glDrawArrays(GL_TRIANGLES, 13, 3);

	glDrawArrays(GL_TRIANGLES, 16, 3);
	glDrawArrays(GL_TRIANGLES, 17, 3);

	glDrawArrays(GL_TRIANGLES, 20, 3);
	glDrawArrays(GL_TRIANGLES, 21, 3);
	*/
	//정사면체
	
	glDrawArrays(GL_TRIANGLES, 24, 3);

	glDrawArrays(GL_TRIANGLES, 27, 3);

	glDrawArrays(GL_TRIANGLES, 30, 3);

	glDrawArrays(GL_TRIANGLES, 33, 3);
	
	glutSwapBuffers();
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
	shaderProgram = make_shaderProgram();

	InitBuffer(); // 버퍼 초기화



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

void Mouse(int button, int state, int x, int y) {
	GLfloat X, Y;
	ScreenToOpenGL(x, y, X, Y); // 좌표 변환

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			isDrag = true;

		}
		else if (state == GLUT_UP && isDrag) {
			isDrag = false;

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
	}
	InitBuffer(); //버퍼 초기화
	glutPostRedisplay(); //화면 갱신
}

void TimerFunction(int value)
{
	InitBuffer(); // 버퍼 초기화
	glutPostRedisplay(); // 화면 갱신

	//glutTimerFunc(60, TimerFunction, 1); // 타이머 함수 호출
}