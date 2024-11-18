#version 330 core

in vec3 color;       // 버텍스 셰이더에서 전달된 색상
out vec4 FragColor;  // 최종 출력 색상

void main()
{
    FragColor = vec4(color, 1.0);  // 전달받은 색상을 최종 색으로 출력
}
