#version 330 core

layout (location = 0) in vec3 aPos; //--- ��ġ ����: attribute position 0
layout (location = 1) in vec3 aColor; //--- �÷� ����: attribute position 1
out vec3 passColor; //--- �÷��� fragment shader�� ���

void main()
{
gl_Position = vec4 (aPos, 1.0);
passColor = aColor;
}