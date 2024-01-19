#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

//fog
uniform float fogDensity;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 spotLightDir;
uniform vec3 spotLightPos;
uniform int initSpot;
vec3 spotLightColor= vec3(1.0f,0.0f,0.0f);

uniform vec3 pointlightPos;
uniform int turnOnLight;


// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
float diffuseStrength = 0.8f;
vec3 specular;
float specularStrength = 0.5f;
float constant = 0.0f;
float linear = 0.045f;
float quadratic = 0.1175f;
float shininess = 64.0f;

float computeFog()
{
     vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
     float fragmentDistance = length(fPosEye);
     float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

     return clamp(fogFactor, 0.0f, 1.0f);
}

float computeShadow()
{
    // perform perspective divide
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0,1] range
    normalizedCoords = normalizedCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;

    // Get depth of current fragment from light's perspective
    float currentDepth = normalizedCoords.z;

    //Check wheter current frag pos is in shadow
    float bias= 0.005f;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    

    return shadow;
}

vec3 computeDirLight()
{

    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * diffuseStrength * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;

    
	return (ambient + diffuse + specular);
}


uniform vec3 pointLightColor;
vec3 computePointLight() {
    
    
    vec4 lightPosEye = view * vec4(pointlightPos, 1.0f);
    vec4 fragPosEye = view * model * vec4(fPosition, 1.0f);

    vec3 cameraPosEye = vec3(0.0f);
    vec3 normalEye = normalize(fNormal);
    
    vec3 lightDirN = normalize(lightPosEye.xyz - fragPosEye.xyz);
    vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);

    vec3 ambient = ambientStrength * pointLightColor;

    vec3 diffuse = max(dot(normalEye, lightDirN), 0.0f) * pointLightColor;
    
    vec3 halfVector = normalize(lightDirN + viewDirN);

    float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
    vec3 specular = specularStrength * specCoeff * pointLightColor;
    float distance = length(lightPosEye.xyz - fragPosEye.xyz);

    float att = 1.0f / (constant + linear * distance + quadratic * distance * distance);
    return (ambient + diffuse + specular) * att;
}

void main() 
{
    vec3 light = computeDirLight();
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

    if(turnOnLight == 1){
	    light = light + computePointLight();
	}

	//modulate with shadow
	
    float shadow = computeShadow();
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);

	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.8f,0.8f,0.8f,1.0f);
	

    fColor = mix(fogColor, min(vec4(color, 1.0f) * vec4(light, 1.0f), 1.0f), fogFactor);
}
