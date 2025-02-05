#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main(){
//get distance between the fragment and the light position
float lightDist = length(FragPos.xyz - lightPos); // не будут значения больше чем farPlane

//map to [o,1]
lightDist /=farPlane;

//write ti depth map
gl_FragDepth = lightDist;


}