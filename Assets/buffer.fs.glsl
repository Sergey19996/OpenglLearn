#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D bufferTex;

float near = 0.1;
float far = 100.0;



void main(){
  //Depth map
  float depthValue = texture(bufferTex, TexCoord).r; // получаем экземпляр красного канала согласно позиции текстурных координат [0,1]- значение глубины 
  float z = depthValue * 2.0 - 1.0; // переводим на экран [ -1 ; +1]
  float linearDepth = (2.0 * near * far) / (z * (far - near) - (far + near)); // Линейная глубина
  float factor = (near + linearDepth) / (near - far);  // Приведение к диапазону [0,1]

  FragColor = vec4(vec3(1-factor), 1.0); //инвертируем фактор 



}