#version 330 core

in vec3 color;  // 버텍스 셰이더에서 받은 색상

out vec4 FragColor;

void main()
{
    FragColor = vec4(color, 1.0);  // 최종 색상 출력
}
