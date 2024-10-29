#version 330 core

layout (location = 0) in vec3 vPos;    // ���ؽ� ��ġ ����
layout (location = 1) in vec3 vColor;  // ���ؽ� ���� ����

out vec3 color;  // �����׸�Ʈ ���̴��� ������ ����

uniform mat4 modelTransform;

void main()
{
    gl_Position = modelTransform * vec4(vPos, 1.0);  // ��ġ ��ȯ
    color = vColor;  // ���� ����
}
