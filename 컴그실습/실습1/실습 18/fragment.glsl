#version 330 core

in vec3 color;  // ���ؽ� ���̴����� ���� ����

out vec4 FragColor;

void main()
{
    FragColor = vec4(color, 1.0);  // ���� ���� ���
}
