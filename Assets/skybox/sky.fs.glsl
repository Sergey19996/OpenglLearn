#version 330 core

out vec4 FragColor;

uniform float time;  //delta time

uniform vec3 min;
uniform vec3 max;

void main(){
// sinusoid
// color[i](t) = A * sin(f * t - h) + k
// A = amplitude = (max - min) / 2
// k = frequency; 2pi / T  T =f; T =24 (seconds); T is a perios = 24 for full cicle
// h = horizontal shift (in radiasn) = pi/2
// k = vertical shift = average of min/max = (min + max) /2


float pi = atan(1.0) * 4;  // более точная запись pi

for (int i = 0; i < 3; i++)
{

float A = (max[i] - min[i]) / 2;
float f = 2 * pi / 24;
float h = pi / 2;
float k = (max[i] + min[i]) / 2;

FragColor[i] = A * sin(f * time - h) + k;

}
FragColor[3] =1.0;
}
  