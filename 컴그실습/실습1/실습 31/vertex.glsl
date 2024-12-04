
#version 330 core
layout (location = 0) in vec3 vPos;      // ���ؽ� ��ġ
layout (location = 1) in vec3 vNormal;   // ���ؽ� ����
layout (location = 2) in vec2 vTexCoord; //--- �ؽ�ó ��ǥ

out vec3 FragPos;        // �����׸�Ʈ ���̴��� ���޵� ��ġ ��
out vec3 Normal;         // �����׸�Ʈ ���̴��� ���޵� ���� ����
out vec2 TexCoord;


uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

void main()
{
    gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vPos, 1.0);
    FragPos = vec3(modelTransform * vec4(vPos, 1.0));
    
    // ���� ��ȯ (���� ��� ���)
    mat3 normalMatrix = transpose(inverse(mat3(projectionTransform * viewTransform * modelTransform)));
    Normal = normalize(normalMatrix * vNormal);

    // �ؽ�ó ��ǥ ����
    TexCoord = vTexCoord; 
}
