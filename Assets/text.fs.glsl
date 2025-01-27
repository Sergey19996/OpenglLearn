#version 330 core

in vec2 TexCoord;   // получаем из вертексного шейдера с запаковоных вертексов z и while
out vec4 FragColor;  // главный фрагментарный аутпут



uniform sampler2D Character; //Буква  
uniform vec3 texColor;  // сюда приходит цвет color.x, color.y, color,z

void main (){
vec4 sampled = vec4(1.0,1.0,1.0,texture(Character,TexCoord).r); // по текстурным координатам берётся пиксель буквы и записвается в альфа канал - красный канал
 FragColor = vec4(texColor,1.0f)*sampled;   //sample отвечает за прозрачность, если 0 то, цвета не будет, если 1, то пиксель заполняем                                              

}
