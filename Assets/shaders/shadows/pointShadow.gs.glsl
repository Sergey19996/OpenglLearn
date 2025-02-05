#version 330 core
layout (triangles) in; // ����: ��� ������� (�����������)
layout (triangle_strip,max_vertices=18) out; // �����: 18 ������ (6 ������ * 3 �������)

uniform mat4 lightSpaceMatrices[6];  // ������� �������������� ��� ������ ����� cubemap

out vec4 FragPos; // �������� ������� ������� �� ����������� ������

void main(){
for(int face =0; face < 6; face++){
 // ��������� ����� cubemap
gl_Layer = face;

 // �������� ����������� ��� ������� �����
for(int i = 0; i < 3; i++){
	FragPos = gl_in[i].gl_Position; // ������� ������� � ������������ ���������
	gl_Position = lightSpaceMatrices[face] * FragPos; // ����������� � ������������ cube
		
		 EmitVertex(); // ���������� �������
		}
		 EndPrimitive(); // ��������� �������� (�����������)
	}

}