#version 330 core

layout (location = 0) in vec3 vPos;    // ���ؽ� ��ġ ����
layout (location = 1) in vec3 vColor;  // ���ؽ� ���� ����

out vec3 color;  // �����׸�Ʈ ���̴��� ������ ����

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

void main()
{
    gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vPos, 1.0);  // ��ġ ��ȯ
    color = vColor;  // ���� ����
}
