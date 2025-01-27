#version 330 core

in vec2 TexCoord;   // �������� �� ����������� ������� � ����������� ��������� z � while
out vec4 FragColor;  // ������� ������������� ������



uniform sampler2D Character; //�����  
uniform vec3 texColor;  // ���� �������� ���� color.x, color.y, color,z

void main (){
vec4 sampled = vec4(1.0,1.0,1.0,texture(Character,TexCoord).r); // �� ���������� ����������� ������ ������� ����� � ����������� � ����� ����� - ������� �����
 FragColor = vec4(texColor,1.0f)*sampled;   //sample �������� �� ������������, ���� 0 ��, ����� �� �����, ���� 1, �� ������� ���������                                              

}
