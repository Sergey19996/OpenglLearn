#version 330 core  //Эта строка задаёт версию GLSL.

layout (location = 0) in vec3 aPos; 
layout (location =1) in vec3 aNormal;
layout (location =2) in vec2 aTexCoord; // сюда придут координаты из масива


out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord; //они отправятся в фрагментарный шейдер 


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
FragPos = vec3(model* vec4(aPos,1.0));   //переводит из локальных (от пивота) в мировые координаты точки  (model =матрицы, Apos - позвертексов

Normal =mat3(transpose(inverse(model)))*aNormal; //  перевод нормалей из локальных в мировые 

gl_Position = projection * view * vec4(FragPos, 1.0); // Устанавливаем положение вершины
TexCoord =aTexCoord; 


}
