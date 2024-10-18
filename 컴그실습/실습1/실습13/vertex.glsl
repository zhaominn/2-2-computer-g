#version 330 core

in vec3 vPos; //--- 메인 프로그램에서 입력 받음
in vec3 vColor; //--- 메인 프로그램에서 입력 받음
out vec3 passColor; //--- fragment shader로 전달

uniform mat4 modelTransform; //--- 모델링 변환 행렬: uniform 변수로 선언

void main()
{
//gl_Position = vec4 (vPos.x, vPos.y, vPos.z, 1.0);
gl_Position = modelTransform * vec4(vPos, 1.0); //--- 좌표값에 modelTransform 변환을 적용한다.

passColor = vColor;
}