#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

struct DirLight{
	sampler2D depthbuffer;
};
uniform DirLight dirLight;

uniform sampler2D bufferTex;

float near = 0.1;
float far = 100.0;



void main(){
  //Depth map (perspective)
//  float depthValue = texture(bufferTex, TexCoord).r; // �������� ��������� �������� ������ �������� ������� ���������� ��������� [0,1]- �������� ������� 
//  float z = depthValue * 2.0 - 1.0; // ��������� �� ����� [ -1 ; +1]
//  float linearDepth = (2.0 * near * far) / (z * (far - near) - (far + near)); // �������� �������
//  float factor = (near + linearDepth) / (near - far);  // ���������� � ��������� [0,1]
  //FragColor = vec4(vec3(1-factor), 1.0); //����������� ������ 

  //Depth map (ortographic)
  FragColor = vec4(vec3(texture(dirLight.depthbuffer,TexCoord).r),1.0);

 // Color Map
// FragColor = texture(bufferTex,TexCoord); 

}