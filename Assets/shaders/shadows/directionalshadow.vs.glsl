#version 330 core
layout ( location = 0) in vec3 aPos;
layout ( location = 3) in vec3 aOffset;
layout ( location = 4) in vec3 asize;


uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() {
	
	vec3 pos = aPos * asize;
	gl_Position = lightSpaceMatrix * model * vec4(pos + aOffset,1.0);  // нет  + оввсета 
	

}

