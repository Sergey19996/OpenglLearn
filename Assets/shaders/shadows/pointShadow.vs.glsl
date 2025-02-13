#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 4) in vec3 aOffsets;
layout (location = 5) in vec3 aSize;
 // Fragpos â geometry shadere 

void main(){

//                 model = asize & aois + aoffset 
gl_Position = vec4(aSize * aPos + aOffsets,1.0);

}
