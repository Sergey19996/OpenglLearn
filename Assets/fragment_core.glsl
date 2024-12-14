#version 330 core
out vec4 FragColor;  //output

in vec3 ourColor;

//��� �������� �������� ������������ �������:
//out ��������, ��� ��� �������� ����� ���������� ��� ��������� ���������� �������
//vec4 � ������������������ ������ (RGBA), �������������� ���� ���������:
//���� �������� ���� ������������ � ����� ����� (Framebuffer) ��� �������� �������.
void main(){
//FragColor = vec4(1.0f,0.5f,0.0f,1.0f);  //������������� �������� ��������� �����:
FragColor = vec4(ourColor,1.0f);  // c arrya->Vertex_core->here
//���� ���� ����� �������� ������� �� ������.

}

//��� ������ ������ ? 
//������ �������, ������� �������������� ���� ��������, �������� ������������� ���� 
//(1.0,0.2,0.6,1.0).