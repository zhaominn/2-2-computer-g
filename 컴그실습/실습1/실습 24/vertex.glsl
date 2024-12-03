#version 330 core

layout (location = 0) in vec3 vPos;      // 버텍스 위치
layout (location = 1) in vec3 vNormal;   // 버텍스 법선
layout (location = 2) in vec2 vTexCoord; //--- 텍스처 좌표

out vec2 TexCoord;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

void main()
{
    gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vPos, 1.0);
    TexCoord = vTexCoord; //--- 텍스처 좌표 전달
}
