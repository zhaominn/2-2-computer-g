#version 330 core

layout (location = 0) in vec3 aPos; //--- 위치 변수: attribute position 0
layout (location = 1) in vec3 aColor; //--- 컬러 변수: attribute position 1
out vec3 passColor; //--- 컬러를 fragment shader로 출력

void main()
{
gl_Position = vec4 (aPos, 1.0);
passColor = aColor;
}