

struct Material{
vec4 diffuse;
vec4 specular;
float shininess;

};
uniform Material  material;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;




uniform sampler2D dirLightBuffer;
uniform samplerCube pointLightBuffers[MAX_POINT_LIGHTS];
uniform sampler2D spotLightBuffers[MAX_SPOT_LIGHTS];
out vec4 FragColor;  //output

in VS_OUT {
 vec3 FragPos;
 vec2 TexCoord;  // C��� ������ ���������� �� ������� ������� 
 TangentLights tanLights;
} fs_in;




uniform bool noTex;
uniform bool noNormalMap;



uniform bool useBlinn;
uniform bool useDirLight;
uniform bool useGamma;

vec4 calcDirlight(vec3 norm, vec3 viewVec,vec3 viewDir,vec4 diffMap,vec4 specMap);

vec4 calcPointLight(int idx,vec3 norm,vec3 viewVec, vec3 viewDir,vec4 diffMap,vec4 specMap);

vec4 calcSpotLight(int idx,vec3 norm,vec3 viewVec,vec3 viewDir,vec4 diffMap,vec4 specMap);    

float calckDirlightShadow(vec3 norm,vec3 viewVec, vec3 lightDir);
float calcSpotLightShadow(int idx, vec3 norm, vec3 viewVec,vec3 lightDir);
float calcPointLightShadow(int idx, vec3 norm,vec3 viewVec, vec3 lightDir,vec3 viewVec);

#define NUM_SAMPLES 20
vec3 sampleOffsetDirections[NUM_SAMPLES] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  0,  0)
);   


///
void main(){                           
  
vec3 norm = normalize(fs_in.tanLights.Normal);  //����������� ������� ���������  (����� ��������� � ����, �����������, ������ ������� ��� ���������
if(!noNormalMap){
// take normal vector from texture (in tangent space)
norm = texture(normal0,fs_in.TexCoord).rgb;
norm = normalize(norm * 2.0 - 1.0); // map form [0,1] to [-1, 1]
}

// calculate view vectors in tangent space
vec3 viewVec = fs_in.tanLights.ViewPos - fs_in.tanLights.FragPos;

vec3 viewDir = normalize(viewVec);  // ����������� ������ ����������� �� ��������� � ������ 

// diffuse and specular textures
vec4 diffMap;
vec4 specMap;

if(noTex == true){  // ����� � ���� ��� ������� 
	diffMap = material.diffuse;
	specMap = material.specular;
}else{
	specMap =texture(specular0,fs_in.TexCoord);
	diffMap =texture(diffuse0,fs_in.TexCoord);
}

//placeholder
vec4 result;

//directionlight
//if(useDirLight){
result = calcDirlight(norm,viewVec,viewDir,diffMap,specMap);

//}


//spotlights
//for(int i =0; i<noSpotLights; i++){
//result += calcSpotLight(i,norm,viewVec,viewDir,diffMap,specMap);
//};
////pointLight
//for(int i = 0; i<noPointLights; i++)
//{
//result += calcPointLight(i,norm,viewVec,viewDir,diffMap,specMap);
//};
//
//

//gamma correction
//if(useGamma){
//float gamma = 2.2;
//result.rgb = pow(result.rgb,vec3(1.0/gamma));
//}

// depth testing
//float near = 0.1;
//float far  = 100.0;
//float z = gl_FragCoord.z * 2.0 - 1.0;  // �������������� � NDC  c [0,1] => [-1,1]
//
//float linearDepth = (2.0 * near * far) / (z * (far - near) - (far + near)); // �������� �������
//float factor = (near + linearDepth) / (near - far);  // ���������� � ��������� [0,1]
//
//result.rgb *= (1.0 - factor); // ���������� � ����������� �� �������
//
//



FragColor = result;

}
///

vec4 calcDirlight(vec3 norm,vec3 viewVec, vec3 viewDir,vec4 diffMap,vec4 specMap)
{

//ambient
vec4 ambient = dirLight.ambient * diffMap;  // ������ ������� �������� �� ���� ��������

//diffuse

vec3 lightDir = normalize(-fs_in.tanLights.dirLightDirection);   // ����������� �������������� ������ �����  ��� (�� ����� � ���������) !���� -�� ��������� � �����!!! 
float diff = max(dot(norm,lightDir), 0.0); // cos(1) = 0 �������� cos(0) = 90 ��������
vec4 diffuse = dirLight.diffuse * (diff * diffMap); // ���� � diff ���� �� �������� �� � diffuse ����� 0


//specular
vec4 specular = vec4(0.0,0.0,0.0,1.0);
if(diff > 0){


float dotProd = 0.0f;
if(useBlinn){   //������ ��������
	//calculate using Blinn-phong model 
	vec3 halWayDir = normalize(lightDir + viewDir); //�������� ���� �������� ��� ��� ������� ������ ����� ���� 
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

float shadow = calckDirlightShadow(norm,viewVec,lightDir);

return vec4(ambient + (1.0-shadow) * (diffuse + specular));

}
float calckDirlightShadow(vec3 norm,vec3 viewVec, vec3 lightDir){
 //                             Projection                   //FragPos only model matrix 
	vec4 FragPosLightSpace = dirLight.lightSpaceMatrix * vec4(fs_in.FragPos,1.0);  //������� ��������� � ������������ ����� (light space).

	//perspective divide (transforming coordinates NDC)
	vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w ; // [depth relative to light] => [-1, 1]

	//NDC to depth range
	projCoords = projCoords * 0.5 + 0.5; // [-1 ; 1] => [0,1]

	//if too far from light, do not return any shadow 
	if(projCoords.z > 1.0){
	 return 0.0f;
	}



	//get depth of fragment
	float currentDepth = projCoords.z;

	//calculate bias
	float maxBias = 0.05;
	float minBias = 0.005;
	float bias = max(maxBias * (1.0 - dot(norm,lightDir)),minBias);

	//PCF
	float shadowSum = 0.0;
	vec2 texelSize = 1.0 / textureSize(dirLightBuffer,0);
	 float viewDist = length(viewVec);
	float diskRadius = (1.0 + (currentDepth / dirLight.farPlane)) / 10.0;

	for(int y = -1; y <= 1; y++){
		for (int x = -1; x<= 1; x++){
		float pcfDepth = texture(dirLightBuffer,projCoords.xy + vec2(x,y) * texelSize ).r;

		shadowSum += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	//return average 
	return shadowSum / 9.0;

}

float calcPointLightShadow(int idx, vec3 norm, vec3 lightDir,vec3 viewVec){
//get vector from the light to the fragment
vec3 lightTofrag = fs_in.FragPos - pointLights[idx].position;

//get currentDepth
float currentDepth = length(lightTofrag);

//calculate bias
float minBias = 0.005;
float maxBias = 0.05;
float bias = max(maxBias * (1.0 - dot(norm,lightDir)),minBias); //dot ��������� cos, � �� ������� �� -1 �� 1   = 0 ����� ���� �������������� 

 // pcf
 float shadow = 0.0;
 float viewDist = length(viewVec);
 float diskRadius = (1.0 + (viewDist / pointLights[idx].farPlane)) / 30.0;

 for (int i =0; i <NUM_SAMPLES ; i++){
	float pcfDepth = texture(pointLightBuffers[idx], lightTofrag + sampleOffsetDirections[i] * diskRadius).r;
	//[0,1] => original depth value
	pcfDepth *= pointLights[idx].farPlane;

	if(currentDepth - bias > pcfDepth){
	shadow += 1.0;
	 
	}

}

 shadow /= float(NUM_SAMPLES);
return shadow;

}
vec4 calcPointLight(int idx,vec3 norm, vec3 viewVec,vec3 viewDir,vec4 diffMap,vec4 specMap){


//FragColor = mix(texture(texture1,TexCoord), texture(texture2,TexCoord),blend);

//ambient
vec4 ambient = pointLights[idx].ambient * diffMap;  // ������ ������� �������� �� ���� ��������

//diffuse

vec3 lightDir = normalize(fs_in.tanLights.pointLightPositions[idx] - fs_in.tanLights.FragPos);   // ����������� ������ ����������� �� ��������� � ����� 
float diff = max(dot(norm,lightDir), 0.0); // ���� ��������� ������������ ������������� ����� 0 (�������� ����� ������ �����������)
vec4 diffuse = pointLights[idx].diffuse * (diff * diffMap); // ���� � diff ���� �� �������� �� � diffuse ����� 0

 
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
specular = pointLights[idx].specular * (spec * specMap);
}

float dist = length(pointLights[idx].position - fs_in.FragPos);  // length = sqrt{x^2 + y^2 + z^2}
float attenuation = 1.0 / (pointLights[idx].k0 + pointLights[idx].k1 * dist + pointLights[idx].k2 * (dist * dist)); // ��� ��������� ����� 

//apply attenuation
ambient *= attenuation;
diffuse *= attenuation;
specular *= attenuation;


float shadow = 0; //calcPointLightShadow(idx, norm, lightDir, viewVec)


return vec4(ambient + (1.0-shadow)*(diffuse + specular));



}


float calcSpotLightShadow(int idx, vec3 norm,vec3 viewVec, vec3 lightDir){
vec4 FragPosLightSpace =spotLights[idx].lightSpaceMatrix * vec4(fs_in.FragPos,1.0); // ������������� ���������� ��� ������ � clip space (���������� ���� ��� ������

//perspective divide (transforming coordinates NDC)
	vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w ; // [depth relative to light] => [-1, 1] ������ xy = ��������� �� ������, � z -�������

	//NDC to depth range
	projCoords = projCoords * 0.5 + 0.5; // [-1 ; 1] => [0,1] ������ ��� xy ���������� ������������� ��� ��������, �� 0 �� 1
	
	// if too far from light dont return any shadow
	if(projCoords.z > 1.0){
	return 0.0;
	}


	//get depth of fragment
	float currentDepth = projCoords.z;
	//calculate bias
	float maxBias = 0.05;
	float minBias = 0.005;
	float bias = max(maxBias * (1.0 - dot(norm,lightDir)),minBias);

	//PCF   
	float shadowSum = 0.0;
	vec2 texelSize = 1.0 / textureSize(spotLightBuffers[idx],0); //���� �������� ����� ������ 1024x1024, �� texelSize = (1.0 / 1024, 1.0 / 1024) 
	 float viewDist = length(viewVec);
	 float diskRadius = (1.0 + (viewDist /spotLights[idx].farPlane)) / 30.0;
	for(int y = -1; y <= 1; y++){
		for (int x = -1; x<= 1; x++){
		float pcfDepth = texture(spotLightBuffers[idx],projCoords.xy + vec2(x,y) * texelSize ).r * diskRadius;
		pcfDepth *= spotLights[idx].farPlane;
		shadowSum +=currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

return  shadowSum / 9;
}

vec4 calcSpotLight(int idx, vec3 norm,vec3 viewVec, vec3 viewDir,vec4 diffMap,vec4 specMap)
{

vec3 lightDir = normalize(fs_in.tanLights.spotLightPositions[idx] - fs_in.tanLights.FragPos);  // ����������� ������ �� ��������� � ��������� �����
float theta =dot(lightDir,normalize(-fs_in.tanLights.spotLightDirections[idx]));  // e��� theta ������ � 1,  ������� ��������� (��� ���������)

//ambient
vec4 ambient = spotLights[idx].ambient*diffMap;


if(theta > spotLights[idx].outerCutOff){ // > ��� ��������� ��� ������ ���� ��� ������ ��������  �� 1 �� -1, ��� -1 ��� 180 �������� � 1 =���� � 0
//���� ������� theta ������ ��� outercutoff ������ ���� ����� �������� �� ������ � ��������� � ������������ ��������� ������ ������������



//diffuse
float diff = max(dot(norm,lightDir),0.0);
vec4 diffuse = spotLights[idx].diffuse * (diff * diffMap);

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
specular = spotLights[idx].specular*(spec * specMap);
}

//calculate attenuation
float intensity = (theta - spotLights[idx].outerCutOff) / (spotLights[idx].cutOff - spotLights[idx].outerCutOff);
intensity =clamp (intensity,0.0,1.0);
diffuse *= intensity;
specular *= intensity;

float dist = length(spotLights[idx].position - fs_in.FragPos);  // length = sqrt{x^2 + y^2 + z^2}
float attenuation = 1.0 / (spotLights[idx].k0 + spotLights[idx].k1 * dist + spotLights[idx].k2 * (dist * dist)); // ��� ��������� ����� 

//apply attenuation
ambient *= attenuation;
diffuse *= attenuation;
specular *= attenuation;

float shadow = calcSpotLightShadow(idx,norm,viewVec,lightDir);

return vec4(ambient + (1.0 -shadow) * (diffuse + specular));

}else
{//render just ambient
return ambient;

}
}
