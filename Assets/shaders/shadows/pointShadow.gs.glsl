#version 330 core
layout (triangles) in; // Вход: три вершины (треугольник)
layout (triangle_strip,max_vertices=18) out; // Выход: 18 вершин (6 граней * 3 вершины)

uniform mat4 lightSpaceMatrices[6];  // Матрицы преобразования для каждой грани cubemap

out vec4 FragPos; // Передача позиции вершины во фрагментный шейдер

void main(){
for(int face =0; face < 6; face++){
 // Указываем грань cubemap
gl_Layer = face;

 // Рендерим треугольник для текущей грани
for(int i = 0; i < 3; i++){
	FragPos = gl_in[i].gl_Position; // Позиция вершины в пространстве отсечения
	gl_Position = lightSpaceMatrices[face] * FragPos; // Преобразуем в пространство cube
		
		 EmitVertex(); // Генерируем вершину
		}
		 EndPrimitive(); // Завершаем примитив (треугольник)
	}

}