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


float cutOff;
float outerCutOff;


vec4 ambient;
vec4 diffuse;
vec4 specular;
};
uniform SpotLight spotLight[MAX_SPOT_LIGHTS];
uniform int noSpotLights;
out vec4 FragColor;  //output

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;  // C��� ������ ���������� �� ������� ������� 

uniform Material  material;
uniform int noTex;

uniform vec3 viewPos;
uniform bool useBlinn;
uniform bool useDirLight;
uniform bool useGamma;

vec4 calcDirlight(vec3 norm, vec3 viewDir,vec4 diffMap,vec4 specMap);

vec4 calcPointLight(int idx,vec3 norm, vec3 viewDir,vec4 diffMap,vec4 specMap);

vec4 calcSpotLight(int idx,vec3 norm,vec3 viewDir,vec4 diffMap,vec4 specMap);    
///
void main(){                           
  
vec3 norm = normalize(Normal);  //����������� ������� ���������  (����� ��������� � ����, �����������, ������ ������� ��� ���������
vec3 viewDir = normalize(viewPos-FragPos);  // ����������� ������ ����������� �� ��������� � ������ 
vec4 diffMap;
vec4 specMap;

if(noTex ==1){  // ����� � ���� ��� ������� 
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
result = calcDirlight(norm,viewDir,diffMap,specMap);

//pointLight
for(int i =0; i<noPointLights; i++)
{
result += calcPointLight(i,norm,viewDir,diffMap,specMap);
};

//spotlights
for(int i =0; i<noSpotLights; i++)
{
result += calcSpotLight(i,norm,viewDir,diffMap,specMap);
};

if(useGamma){
float gamma = 2.2;
result.rgb = pow(result.rgb,vec3(1.0/gamma));
}
FragColor = result;

}
///

vec4 calcDirlight(vec3 norm, vec3 viewDir,vec4 diffMap,vec4 specMap)
{

//ambient
vec4 ambient = dirLight.ambient * diffMap;  // ������ ������� �������� �� ���� ��������

//diffuse

vec3 lightDir =normalize(-dirLight.direction);   // ����������� �������������� ������ �����  ��� (�� ����� � ���������) !���� -�� ��������� � �����!!! 
float diff =max(dot(norm,lightDir), 0.0); // ���� ��������� ������������ ������������� ����� 0 (�������� ����� ������ �����������)
vec4 diffuse = dirLight.diffuse * (diff * diffMap); // ���� � diff ���� �� �������� �� � diffuse ����� 0


//specular
vec4 specular = vec4(0.0,0.0,0.0,1.0);
if(diff >0){


float dotProd = 0.0f;
if(useBlinn){   //������ ��������
	//calculate using Blinn-phong model 
	vec3 halWayDir = normalize(lightDir + viewDir);
	dotProd = dot(halWayDir,norm);   // �� ��������� dot producta ����������� ���������������� 

}else{
	//calculate using phong model 
	
	vec3 reflectDir = reflect(-lightDir,norm);   // R =  L-2(L*N)*N ��������� ����������� ���������� �����.   
	dotProd = dot(viewDir,reflectDir);  
	
	}

float spec = pow(max(dotProd,0.0), material.shininess * 128); // ���������� reflectDir � ������ ��� ����������� ������������
specular = dirLight.specular*(spec * specMap);
}


//vec3 reflectDir = reflect(-lightDir,norm);   // R =  L-2(L*N)*N ��������� ����������� ���������� �����.   
//float spec = pow(max(dot(viewDir,reflectDir),0.0), material.shininess * 128); // ���������� reflectDir � ������ ��� ����������� ������������
//vec4 specular = dirLight.specular*(spec * specMap);

return vec4(ambient + diffuse + specular);

}



vec4 calcPointLight(int idx,vec3 norm, vec3 viewDir,vec4 diffMap,vec4 specMap){


//FragColor = mix(texture(texture1,TexCoord), texture(texture2,TexCoord),blend);

//ambient
vec4 ambient = pointLight[idx].ambient * diffMap;  // ������ ������� �������� �� ���� ��������

//diffuse

vec3 lightDir =normalize(pointLight[idx].position - FragPos);   // ����������� ������ ����������� �� ��������� � ����� 
float diff =max(dot(norm,lightDir), 0.0); // ���� ��������� ������������ ������������� ����� 0 (�������� ����� ������ �����������)
vec4 diffuse = pointLight[idx].diffuse * (diff * diffMap); // ���� � diff ���� �� �������� �� � diffuse ����� 0

 
//specular
vec4 specular = vec4(0.0,0.0,0.0,1.0);
if(diff >0){


float dotProd = 0.0f;
if(useBlinn){   //������ ��������
	//calculate using Blinn-phong model 
	vec3 halWayDir = normalize(lightDir + viewDir);
	dotProd = dot(halWayDir,norm);   // �� ��������� dot producta ����������� ���������������� 

}else{
	//calculate using phong model 
	
	vec3 reflectDir = reflect(-lightDir,norm);   // R =  L-2(L*N)*N ��������� ����������� ���������� �����.   
	dotProd = dot(viewDir,reflectDir);  
	
	}

float spec = pow(max(dotProd,0.0), material.shininess * 128); // ���������� reflectDir � ������ ��� ����������� ������������
specular = dirLight.specular*(spec * specMap);
}

float dist = length(pointLight[idx].position - FragPos);  // length = sqrt{x^2 + y^2 + z^2}
float attenuation = 1.0 / (pointLight[idx].k0 + pointLight[idx].k1 * dist + pointLight[idx].k2 * (dist * dist)); // ��� ��������� ����� 


return vec4(ambient + diffuse + specular) * attenuation;



}

vec4 calcSpotLight(int idx, vec3 norm, vec3 viewDir,vec4 diffMap,vec4 specMap)
{

vec3 lightDir = normalize(spotLight[idx].position - FragPos);  // ����������� ������ �� ��������� � ���������
float theta =dot(lightDir,normalize(-spotLight[idx].direction));  // e��� theta ������ � 1,  ������� ��������� (��� ���������)

//ambient
vec4 ambient =spotLight[idx].ambient*diffMap;


if(theta > spotLight[idx].outerCutOff){ // > ��� ��������� ��� ������ ���� ��� ������ ��������  �� 1 �� -1, ��� -1 ��� 180 �������� � 1 =���� � 0
//���� ������� theta ������ ��� outercutoff ������ ���� ����� �������� �� ������ � ��������� � ������������ ��������� ������ ������������

//if in cutoff, render light 

//diffuse
float diff = max(dot(norm,lightDir),0.0);
vec4 diffuse = spotLight[idx].diffuse * (diff * diffMap);

//specular
vec4 specular = vec4(0.0,0.0,0.0,1.0);
if(diff >0){


float dotProd = 0.0f;
if(useBlinn){   //������ ��������
	//calculate using Blinn-phong model 
	vec3 halWayDir = normalize(lightDir + viewDir);
	dotProd = dot(halWayDir,norm);   // �� ��������� dot producta ����������� ���������������� 

}else{
	//calculate using phong model 
	
	vec3 reflectDir = reflect(-lightDir,norm);   // R =  L-2(L*N)*N ��������� ����������� ���������� �����.   
	dotProd = dot(viewDir,reflectDir);  
	
	}

float spec = pow(max(dotProd,0.0), material.shininess * 128); // ���������� reflectDir � ������ ��� ����������� ������������
specular = dirLight.specular*(spec * specMap);
}

float intensity = (theta - spotLight[idx].outerCutOff) / (spotLight[idx].cutOff - spotLight[idx].outerCutOff);
intensity =clamp (intensity,0.0,1.0);
diffuse *= intensity;
specular *= intensity;

float dist = length(spotLight[idx].position - FragPos);  // length = sqrt{x^2 + y^2 + z^2}
float attenuation = 1.0 / (spotLight[idx].k0 + spotLight[idx].k1 * dist + spotLight[idx].k2 * (dist * dist)); // ��� ��������� ����� 




return vec4(ambient + diffuse + specular) * attenuation;

}else
{//render just ambient
return ambient;

}
}
