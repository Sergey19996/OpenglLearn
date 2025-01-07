#version 330 core


struct Material{
vec3 ambient;
vec3 diffuse;
vec3 specular;
float shininess;

};

struct Light {
vec3 position;

vec3 ambient;
vec3 diffuse;
vec3 specular;


};

out vec4 FragColor;  //output

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;  // Cюда придут координаты из вертекс кора 

//uniform sampler2D texture1;
//uniform sampler2D texture2;

//uniform float blend;

uniform Material  material;
uniform Light light;

uniform vec3 viewPos;


void main(){

//FragColor = mix(texture(texture1,TexCoord), texture(texture2,TexCoord),blend);

//ambient
vec3 ambient = light.ambient * material.ambient;

//diffuse
vec3 norm = normalize(Normal);  //нормализуем нормаль фрагмента 
vec3 lightDir =normalize(light.position - FragPos);   // нормализуем вектор направлени€ от фрагмента к свету 
float diff =max(dot(norm,lightDir), 0.0); // если скал€рное произведение отрицательное пишем 0 (источник света позади поверхности)
vec3 diffuse = light.diffuse * (diff * material.diffuse); // если в diff свет не поподает то и diffuse Ѕудет 0


//specular
vec3 viewDir =normalize(viewPos-FragPos);    // нормализуем вектор направлени€ от фрагмента к камере 
vec3 reflectDir = reflect(-lightDir,norm);   // R =  L-2(L*N)*N указывает направление отражЄнного света.
float spec = pow(max(dot(viewDir,reflectDir),0.0), material.shininess * 128); // используем reflectDir и камеру дл€ отображение зеркальности
vec3 specular = light.specular*(spec * material.specular);

FragColor = vec4(vec3(ambient + diffuse + specular),1.0);

}
