#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(location = 0) out vec3 v_WorldPosition;
layout(location = 1) out vec3 v_WorldNormal;
layout(location = 2) out vec2 v_TexCoord;
layout(location = 3) out vec3 v_ViewPosition;
layout(location = 4) out vec4 v_LightSpacePosition;

uniform mat4 u_Model;
uniform mat4 u_ModelView;
uniform mat4 u_Normal;
uniform mat4 u_ModelViewProj;
uniform mat4 u_LightSpaceMatrix;

void main(){
	gl_Position = u_ModelViewProj * vec4(a_Position, 1.0);
	v_WorldPosition = (u_Model * vec4(a_Position, 1.0)).xyz;
	v_ViewPosition = (u_ModelView * vec4(a_Position, 1.0)).xyz;
	v_WorldNormal = (u_Normal * vec4(a_Normal, 0.0)).xyz;
	v_TexCoord = a_TexCoord;
	
	v_LightSpacePosition = u_LightSpaceMatrix * vec4(v_WorldPosition, 1.0);
}

#shader fragment
#version 450 core

layout(location = 0) in vec3 v_WorldPosition;
layout(location = 1) in vec3 v_WorldNormal;
layout(location = 2) in vec2 v_TexCoord;
layout(location = 3) in vec3 v_ViewPosition;
layout(location = 4) in vec4 v_LightSpacePosition;

layout(location = 0) out vec4 o_Color;

layout(location = 0) uniform sampler2D u_DiffuseTexture;
layout(location = 1) uniform sampler2D u_SpecularTexture;
layout(location = 2) uniform sampler2D u_EmissiveTexture;
layout(location = 3) uniform sampler2D u_ShadowMap;

struct DirLight {
	vec4 color;
	vec3 direction;
};

struct Material {
	vec4 diffuse;
	vec4 specular;
	vec4 emissive;
	float glossiness;
};

uniform DirLight u_DirLight;
uniform Material u_Material;
uniform vec3 u_CameraPosition;

float shadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    // float bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.005);
    float bias = 0.005;
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    /**/
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    //*/

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

vec3 directional(DirLight dirLight, vec3 Ka, vec3 Kd, vec3 Ks, vec3 Ke){
	vec3 color = vec3(0.0);

	vec3 worldNormal = normalize(v_WorldNormal);
	float DdotN = dot(worldNormal, -dirLight.direction);
	float intensity = dirLight.color.w;
	float glossyExp = 10.0 * u_Material.glossiness + 1.0;
	float glossyFactor = pow(2.0, glossyExp);

	// Ambient component
	vec3 ambient = 0.05 * Ka;
	
	// Diffuse component
	vec3 diffuse = Kd * dirLight.color.rgb * intensity * max(0.0, DdotN);

	// Specular component
	vec3 L = normalize(-dirLight.direction);
	vec3 V = normalize(u_CameraPosition - v_WorldPosition);
	vec3 H = normalize(L + V);
	vec3 N = worldNormal;
	vec3 specular = Ks * dirLight.color.rgb * intensity * pow(max(0.0, dot(N, H)), glossyFactor) * max(0.0, DdotN);

	// Emissive component
	float attFactor = 0.33;
	float attenuation = max(0.5, pow(attFactor, 2));
	vec3 emissive = Ke / clamp(pow(attenuation, 2), 0.9, 1.4);

    float shadow = shadowCalculation(v_LightSpacePosition, worldNormal, -dirLight.direction);
	color = ambient + (1.0 - shadow) * (diffuse + specular) + emissive;
	return color;
}

void main(){
	vec3 Kd = texture(u_DiffuseTexture, v_TexCoord).rgb * u_Material.diffuse.rgb;
	vec3 Ks = texture(u_SpecularTexture, v_TexCoord).rgb * u_Material.specular.rgb;
	vec3 Ke = texture(u_EmissiveTexture, v_TexCoord).rgb * u_Material.emissive.rgb;
	vec3 Ka = Kd;

	o_Color = vec4(0.0, 0.0, 0.0, 1.0);  // Alpha set to 1 here
	o_Color += vec4(directional(u_DirLight, Ka, Kd, Ks, Ke), 0.0);
}
