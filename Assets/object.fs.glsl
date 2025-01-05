#version 330 core
out vec4 FragColor;  //output

in vec3 ourColor;
in vec2 TexCoord;  // C��� ������ ���������� �� ������� ���� 

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float blend;

void main(){

FragColor = mix(texture(texture1,TexCoord), texture(texture2,TexCoord),blend);


}