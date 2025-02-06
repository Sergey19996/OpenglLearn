#version 330 core
layout ( location = 0) in vec3 aPos;
layout ( location = 3) in vec3 aOffset;
layout ( location = 4) in vec3 asize;


uniform mat4 lightSpaceMatrix;

out vec4 FragPos;


void main() {
	
	//vec3 pos = aPos * asize;
	FragPos = vec4(asize * aPos + aOffset,1.0);

	gl_Position = lightSpaceMatrix * FragPos;  // нет  + оввсета 
	

}

