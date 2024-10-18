#version 330 core

in vec3 vPos; //--- ���� ���α׷����� �Է� ����
in vec3 vColor; //--- ���� ���α׷����� �Է� ����
out vec3 passColor; //--- fragment shader�� ����

uniform mat4 modelTransform; //--- �𵨸� ��ȯ ���: uniform ������ ����

void main()
{
//gl_Position = vec4 (vPos.x, vPos.y, vPos.z, 1.0);
gl_Position = modelTransform * vec4(vPos, 1.0); //--- ��ǥ���� modelTransform ��ȯ�� �����Ѵ�.

passColor = vColor;
}