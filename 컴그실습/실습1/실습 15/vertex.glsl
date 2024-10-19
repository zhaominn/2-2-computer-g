#version 330 core

layout (location = 0) in vec3 vPos;    // 버텍스 위치 정보
layout (location = 1) in vec3 vColor;  // 버텍스 색상 정보

out vec3 color;  // 프래그먼트 셰이더로 전달할 색상

uniform mat4 modelTransform;

void main()
{
    gl_Position = modelTransform * vec4(vPos, 1.0);  // 위치 변환
    color = vColor;  // 색상 전달
}
