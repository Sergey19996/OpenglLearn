#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;


uniform float time;  //delta time

uniform vec3 min;
uniform vec3 max;

void main()
{    
    FragColor = texture(skybox, TexCoords);
//
//    
//float pi = atan(1.0) * 4;  // более точная запись pi
//
//for (int i = 0; i < 3; i++)
//{
//
//float A = (max[i] - min[i]) / 2;
//float f = 2 * pi / 24;
//float h = pi / 2;
//float k = (max[i] + min[i]) / 2;
//
//FragColor[i] = A * sin(f * time - h) + k * texture(skybox, TexCoords)[i];
//
//}
//FragColor[3] =1.0;
}