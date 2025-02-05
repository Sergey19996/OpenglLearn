#version 330 core


struct Material{
vec4 diffuse;
vec4 specular;
float shininess;

};
uniform sampler2D diffuse0;
uniform sampler2D specular0;


struct DirLight{
vec3 direction;
vec4 ambient;
vec4 diffuse;
vec4 specular;

sampler2D depthBuffer;
mat4 lightSpaceMatrix;
};
uniform DirLight dirLight;


#define  MAX_POINT_LIGHTS 20
struct PointLight {
vec3 position;

float k0;
float k1;
float k2;

vec4 ambient;
vec4 diffuse;
vec4 specular;

float farPlane;

samplerCube depthBuffer;

};
uniform PointLight pointLight[MAX_POINT_LIGHTS];
uniform int noPointLights;

#define MAX_SPOT_LIGHTS 5
struct SpotLight {

vec3 position;
vec3 direction;

float k0;
float k1;
float k2;

float nearPlane;
float farPlane;


float cutOff;
float outerCutOff;


vec4 ambient;
vec4 diffuse;
vec4 specular;

sampler2D depthBuffer;
mat4 lightSpaceMatrix; 
};
uniform SpotLight spotLight[MAX_SPOT_LIGHTS];
uniform int noSpotLights;
out vec4 FragColor;  //output

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;  // Cюда придут координаты из вертекс шейдера 

uniform Material  material;
uniform int noTex;

uniform vec3 viewPos;
uniform bool useBlinn;
uniform bool useDirLight;
uniform bool useGamma;

vec4 calcDirlight(vec3 norm, vec3 viewDir,vec4 diffMap,vec4 specMap);

vec4 calcPointLight(int idx,vec3 norm, vec3 viewDir,vec4 diffMap,vec4 specMap);

vec4 calcSpotLight(int idx,vec3 norm,vec3 viewDir,vec4 diffMap,vec4 specMap);    

float calckDirlightShadow(vec3 norm, vec3 lightDir);
float calcSpotLightShadow(int idx, vec3 norm, vec3 lightDir);
float calcPointLightShadow(int idx, vec3 norm, vec3 lightDir);
///
void main(){                           
  
vec3 norm = normalize(Normal);  //нормализуем нормаль фрагмента  (после изменений в меше, перемещений, скейла ротейта для вертексов
vec3 viewDir = normalize(viewPos-FragPos);  // нормализуем вектор направления от фрагмента к камере 
vec4 diffMap;
vec4 specMap;

if(noTex ==1){  // когда у меша нет текстур 
	diffMap = material.diffuse;
	specMap = material.specular;
}else{
	specMap =texture(specular0,TexCoord);
	diffMap =texture(diffuse0,TexCoord);
}

//placeholder
vec4 result;

//directionlight
if(useDirLight)
{
result = calcDirlight(norm,viewDir,diffMap,specMap);
//spotlights
for(int i =0; i<noSpotLights; i++)
{
result += calcSpotLight(i,norm,viewDir,diffMap,specMap);
};
}

//pointLight
for(int i =0; i<noPointLights; i++)
{
result += calcPointLight(i,norm,viewDir,diffMap,specMap);
};



//gamma correction
if(useGamma){
float gamma = 2.2;
result.rgb = pow(result.rgb,vec3(1.0/gamma));
}

// depth testing
//float near = 0.1;
//float far  = 100.0;
//float z = gl_FragCoord.z * 2.0 - 1.0;  // Преобразование в NDC  c [0,1] => [-1,1]
//
//float linearDepth = (2.0 * near * far) / (z * (far - near) - (far + near)); // Линейная глубина
//float factor = (near + linearDepth) / (near - far);  // Приведение к диапазону [0,1]
//
//result.rgb *= (1.0 - factor); // Затемнение в зависимости от глубины
//
//



FragColor = result;

}
///

vec4 calcDirlight(vec3 norm, vec3 viewDir,vec4 diffMap,vec4 specMap)
{

//ambient
vec4 ambient = dirLight.ambient * diffMap;  // каждый пиксель умножаем на свет эмбиента

//diffuse

vec3 lightDir =normalize(-dirLight.direction);   // нормализуем инвертированый вектор света  был (от света к фрагменту) !стал -от фрагмента к свету!!! 
float diff =max(dot(norm,lightDir), 0.0); // если скалярное произведение отрицательное пишем 0 (источник света позади поверхности)
vec4 diffuse = dirLight.diffuse * (diff * diffMap); // если в diff свет не поподает то и diffuse Будет 0


//specular
vec4 specular = vec4(0.0,0.0,0.0,1.0);
if(diff > 0){


float dotProd = 0.0f;
if(useBlinn){   //булева операция
	//calculate using Blinn-phong model 
	vec3 halWayDir = normalize(lightDir + viewDir);
	dotProd = dot(halWayDir,norm);   // по свойствам dot producta расчитываем сопоставленность 

}else{
	//calculate using phong model 
	
	vec3 reflectDir = reflect(-lightDir,norm);   // R =  L-2(L*N)*N указывает направление отражённого света.   
	dotProd = dot(viewDir,reflectDir);  
	
	}

float spec = pow(max(dotProd,0.0), material.shininess * 128); // используем reflectDir и камеру для отображение зеркальности
specular = dirLight.specular*(spec * specMap);
}


//vec3 reflectDir = reflect(-lightDir,norm);   // R =  L-2(L*N)*N указывает направление отражённого света.   
//float spec = pow(max(dot(viewDir,reflectDir),0.0), material.shininess * 128); // используем reflectDir и камеру для отображение зеркальности
//vec4 specular = dirLight.specular*(spec * specMap);

float shadow = calckDirlightShadow(norm,lightDir);

return vec4(ambient + (1.0-shadow) * (diffuse + specular));

}
float calckDirlightShadow(vec3 norm, vec3 lightDir){
 //                             Projection                   //FragPos only model matrix 
	vec4 FragPosLightSpace = dirLight.lightSpaceMatrix * vec4(FragPos,1.0); 

	//perspective divide (transforming coordinates NDC)
	vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w ; // [depth relative to light] => [-1, 1]

	//NDC to depth range
	projCoords = projCoords * 0.5 + 0.5; // [-1 ; 1] => [0,1]

	//if too far from light, do not return any shadow 
	if(projCoords.z > 1.0){
	 return 0.0;
	}

	//get closest depth in depth buffer
	float closestDepth = texture(dirLight.depthBuffer, projCoords.xy).r;

	//get depth of fragment
	float currentDepth = projCoords.z;

	//calculate bias
	float maxBias = 0.05;
	float minBias = 0.005;
	float bias = max(maxBias * (1.0 - dot(norm,lightDir)),minBias);

	//PCF
	float shadowSum = 0.0;
	vec2 texelSize = 1.0/textureSize(dirLight.depthBuffer,0);
	for(int y = -1; y <= 1; y++){
		for (int x = -1; x<= 1; x++){
		float pcfDepth = texture(dirLight.depthBuffer,projCoords.xy + vec2(x,y) * texelSize).r;
		shadowSum +=currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	//return average 
	return shadowSum /9.0;

}

float calcPointLightShadow(int idx, vec3 norm, vec3 lightDir){
//get vector from the light to the fragment
vec3 lightTofrag = FragPos - pointLight[idx].position;

//get depth from cubemap
float closestDepth = texture(pointLight[idx].depthBuffer,lightTofrag).r;

//[0,1] => original depth value
closestDepth *= pointLight[idx].farPlane; // обратная нормализация. нормализация  Pountshadow.fs

//get currentDepth
float currentDepth = length(lightTofrag);

//calculate bias
float minBias = 0.005;
float maxBias = 0.05;
float bias = max(maxBias * (1.0 - dot(norm,lightDir)),minBias); //dot возращает cos, а он прыгает от -1 до 1   = 0 будет если перпенидулярно 

return currentDepth - bias > closestDepth ? 1.0 :0.0;

}
vec4 calcPointLight(int idx,vec3 norm, vec3 viewDir,vec4 diffMap,vec4 specMap){


//FragColor = mix(texture(texture1,TexCoord), texture(texture2,TexCoord),blend);

//ambient
vec4 ambient = pointLight[idx].ambient * diffMap;  // каждый пиксель умножаем на свет эмбиента

//diffuse

vec3 lightDir =normalize(pointLight[idx].position - FragPos);   // нормализуем вектор направления от фрагмента к свету 
float diff =max(dot(norm,lightDir), 0.0); // если скалярное произведение отрицательное пишем 0 (источник света позади поверхности)
vec4 diffuse = pointLight[idx].diffuse * (diff * diffMap); // если в diff свет не поподает то и diffuse Будет 0

 
//specular
vec4 specular = vec4(0.0,0.0,0.0,1.0);
if(diff >0){


float dotProd = 0.0f;
if(useBlinn){   //булева операция
	//calculate using Blinn-phong model 
	vec3 halWayDir = normalize(lightDir + viewDir);
	dotProd = dot(halWayDir,norm);   // по свойствам dot producta расчитываем сопоставленность 

}else{
	//calculate using phong model 
	
	vec3 reflectDir = reflect(-lightDir,norm);   // R =  L-2(L*N)*N указывает направление отражённого света.   
	dotProd = dot(viewDir,reflectDir);  
	
	}

float spec = pow(max(dotProd,0.0), material.shininess * 128); // используем reflectDir и камеру для отображение зеркальности
specular = dirLight.specular*(spec * specMap);
}

float dist = length(pointLight[idx].position - FragPos);  // length = sqrt{x^2 + y^2 + z^2}
float attenuation = 1.0 / (pointLight[idx].k0 + pointLight[idx].k1 * dist + pointLight[idx].k2 * (dist * dist)); // для затухания света 

//apply attenuation
ambient *= attenuation;
diffuse *= attenuation;
specular *= attenuation;


float shadow = 0;

return vec4(ambient + (1.0-shadow)*(diffuse + specular));



}


float calcSpotLightShadow(int idx, vec3 norm, vec3 lightDir){
vec4 FragPosLightSpace =spotLight[idx].lightSpaceMatrix * vec4(FragPos,1.0); // преобразовали координату для экрана в clip space (координаты пока что сырыые

//perspective divide (transforming coordinates NDC)
	vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w ; // [depth relative to light] => [-1, 1] теперь xy = координат на экране, а z -глубина

	//NDC to depth range
	projCoords = projCoords * 0.5 + 0.5; // [-1 ; 1] => [0,1] теперь все xy координаты переделалилсь для текстуры, от 0 до 1
	
	// if too far from light dont return any shadow
	if(projCoords.z > 1.0){
	return 0.0;
	}

	//get closest depth in depth buffer
	float clossestDepth = texture(spotLight[idx].depthBuffer,projCoords.xy).r; // текстура с GL_DEPTH_COMPONENT имеет только red канал для оптимизации

	//llinearize depth
	float z = clossestDepth * 2.0 - 1.0;   // из [0, 1] в [-1, 1]
	clossestDepth = (2.0 * spotLight[idx].nearPlane * spotLight[idx].farPlane) /                     
	(spotLight[idx].farPlane + spotLight[idx].nearPlane - z * (spotLight[idx].farPlane - spotLight[idx].nearPlane));   // до этого ближайшим объектом доавлась 50% от 1 что было нелинейно
	                                                                                                                  //теперь всё пространство Near и far plane будет линейным
	clossestDepth /= spotLight[idx].farPlane; //far plane = 100     100 /99 = 0.99
	
	//get depth of fragment
	float currentDepth = projCoords.z;
	//calculate bias
	float maxBias = 0.05;
	float minBias = 0.005;
	float bias = max(maxBias * (1.0 - dot(norm,lightDir)),minBias);

	//PCF   
	float shadowSum = 0.0;
	vec2 texelSize = 1.0/textureSize(spotLight[idx].depthBuffer,0); //Если текстура имеет размер 1024x1024, то texelSize = (1.0 / 1024, 1.0 / 1024) 
	for(int y = -1; y <= 1; y++){
		for (int x = -1; x<= 1; x++){
		float pcfDepth = texture(spotLight[idx].depthBuffer,projCoords.xy + vec2(x,y) * texelSize).r;
		shadowSum +=currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

return  shadowSum / 9;
}

vec4 calcSpotLight(int idx, vec3 norm, vec3 viewDir,vec4 diffMap,vec4 specMap)
{

vec3 lightDir = normalize(spotLight[idx].position - FragPos);  // нормализуем вектор от фрагмента к источнику света
float theta =dot(lightDir,normalize(-spotLight[idx].direction));  // eсли theta близок к 1,  векторы идентичны (для косинусов)

//ambient
vec4 ambient = spotLight[idx].ambient*diffMap;


if(theta > spotLight[idx].outerCutOff){ // > для косинусов чем больше угол тем меньше значение  от 1 до -1, где -1 это 180 градусов а 1 =углу в 0
//если косинус theta меньше чем outercutoff значит угол между вектором от фрмнта к спотлайту и направлением спотлайта больше необходимого

//if in cutoff, render light 

//diffuse
float diff = max(dot(norm,lightDir),0.0);
vec4 diffuse = spotLight[idx].diffuse * (diff * diffMap);

//specular
vec4 specular = vec4(0.0,0.0,0.0,1.0);
if(diff >0){


float dotProd = 0.0f;
if(useBlinn){   //булева операция
	//calculate using Blinn-phong model 
	vec3 halWayDir = normalize(lightDir + viewDir);
	dotProd = dot(halWayDir,norm);   // по свойствам dot producta расчитываем сопоставленность 

}else{
	//calculate using phong model 
	
	vec3 reflectDir = reflect(-lightDir,norm);   // R =  L-2(L*N)*N указывает направление отражённого света.   
	dotProd = dot(viewDir,reflectDir);  
	
	}

float spec = pow(max(dotProd,0.0), material.shininess * 128); // используем reflectDir и камеру для отображение зеркальности
specular = dirLight.specular*(spec * specMap);
}

//calculate attenuation
float intensity = (theta - spotLight[idx].outerCutOff) / (spotLight[idx].cutOff - spotLight[idx].outerCutOff);
intensity =clamp (intensity,0.0,1.0);
diffuse *= intensity;
specular *= intensity;

float dist = length(spotLight[idx].position - FragPos);  // length = sqrt{x^2 + y^2 + z^2}
float attenuation = 1.0 / (spotLight[idx].k0 + spotLight[idx].k1 * dist + spotLight[idx].k2 * (dist * dist)); // для затухания света 

//apply attenuation
ambient *= attenuation;
diffuse *= attenuation;
specular *= attenuation;

float shadow = calcSpotLightShadow(idx,norm,lightDir);

return vec4(ambient + (1.0 -shadow) * (diffuse + specular));

}else
{//render just ambient
return ambient;

}
}
