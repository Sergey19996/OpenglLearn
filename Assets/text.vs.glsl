#version 330 core
layout (location = 0) in vec4 vertex; //pos and tex coord
out vec2 TexCoord;  // выходное в фрагментарный шейдер (пиксельный) 

uniform mat4 projection;

void main (){
 gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);   //для орто проекции 
 TexCoord = vertex.zw;    // передаем texcoord из запаковоного вертекса, последние 2 колонки

}
