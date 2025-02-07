layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord; // сюда придут координаты из масива
layout(location = 3) in vec3  aOffset;
layout (location = 4) in vec3 aSize;



out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;

} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){

vec3 pos = aPos*aSize;  // для инстансеров мы каждый вертекс умножаем на сайз
pos = pos + aOffset; // добавляем оффсет
vs_out.FragPos = vec3(model* vec4(pos,1.0));   //переводит из локальных (от пивота) в мировые координаты точки  (model =матрицы, Apos - позвертексов

vs_out.Normal =mat3(transpose(inverse(model)))*aNormal; //  перевод нормалей из локальных в мировые 

gl_Position = projection * view * vec4(vs_out.FragPos, 1.0); // Устанавливаем положение вершины
vs_out.TexCoord = aTexCoord; 


}
