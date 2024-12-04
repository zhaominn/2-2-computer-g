
#version 330 core
in vec3 FragPos;      // ���ؽ� ���̴����� �޾ƿ� ��ġ ��
in vec3 Normal;       // ���ؽ� ���̴����� �޾ƿ� ���� ����
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightPos;      // ������ ��ġ
uniform vec3 lightColor;    // ������ ����
uniform vec3 viewPos;       // �������� ��ġ
uniform vec3 ambientLight;


uniform sampler2D outTexture; //--- �ؽ�ó ���÷�


void main()
{
    // Ambient Lighting (�ֺ���)
    vec3 ambient = ambientLight * lightColor;

    // Diffuse Lighting (���ݻ籤)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular Lighting (�ſ�ݻ籤)
    float shininess = 64.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = spec * lightColor;

    // ���� ���� ���
    vec3 result = (ambient + diffuse+specular);
    
    // �ؽ�ó ����
    FragColor = vec4 (result, 0.7f);
    FragColor = texture(outTexture, TexCoord) * FragColor;
    
  }

