#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(location = 0) out vec3 v_WorldPosition;
layout(location = 1) out vec3 v_WorldNormal;
layout(location = 2) out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_Normal;

void main(){
	gl_Position = u_Model * vec4(a_Position, 1.0);
	v_WorldPosition = (u_Model * vec4(a_Position, 1.0)).xyz;
	v_WorldNormal = (u_Normal * vec4(a_Normal, 0.0)).xyz;
	v_TexCoord = a_TexCoord;
}

#shader geometry
#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec3 v_WorldPosIn[];
layout(location = 1) in vec3 v_NormalIn[];
layout(location = 2) in vec2 v_TexCoordIn[];

layout(location = 0) out vec3 v_WorldPosOut;
layout(location = 1) out vec3 v_NormalOut;
layout(location = 2) out vec2 v_TexCoordOut;

uniform mat4 u_ViewProj;
uniform float u_Force;
uniform float u_Time;

float rand(vec3 seed, float factor) {
	return fract(sin(dot(seed, vec3(12.9898, 78.233, 53.539)) * factor) * 43758.5453);
}

float rand(vec3 seed) {
	return fract(sin(dot(seed, vec3(12.9898, 78.233, 53.539))) * 43758.5453);
}

vec3 randVec3(vec3 seed) {
	float x = rand(seed.xzy, 1.5) * 2.0 - 1.0;
	float y = rand(seed.yzx, 2.5) * 2.0 - 1.0;
	float z = rand(seed.zxy, 4.5) * 2.0 - 1.0;
	return vec3(x, y, z);
}

vec3 getTriangleNormal() {
	vec3 a = gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz;
	vec3 b = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
	return normalize(cross(a, b));
}

vec3 explode(vec3 position, vec3 normal)
{
	float randomForceFactor = rand(normal) * 3.0 + 1.0;
	float displacement = u_Time * u_Force * randomForceFactor;
	const float randomDirectionAttenuation = 0.7;
	vec3 randomAlteration = normalize(randVec3(normal)) * randomDirectionAttenuation;
	vec3 direction = (normal + randomAlteration) * displacement;

	vec3 finalPosition = position + direction;
	finalPosition.y += 2.0 * max(0.0, -(finalPosition.y + 4.0));
	return finalPosition;
}

void main() {
	vec3 triangleNormal = getTriangleNormal();

	v_WorldPosOut = explode(v_WorldPosIn[0], triangleNormal);
	gl_Position = u_ViewProj * vec4(v_WorldPosOut, 1.0);
	v_NormalOut = v_NormalIn[0];
	v_TexCoordOut = v_TexCoordIn[0];
	EmitVertex();
	v_WorldPosOut = explode(v_WorldPosIn[1], triangleNormal);
	gl_Position = u_ViewProj * vec4(v_WorldPosOut, 1.0);
	v_NormalOut = v_NormalIn[1];
	v_TexCoordOut = v_TexCoordIn[1];
	EmitVertex();
	v_WorldPosOut = explode(v_WorldPosIn[2], triangleNormal);
	gl_Position = u_ViewProj * vec4(v_WorldPosOut, 1.0);
	v_NormalOut = v_NormalIn[2];
	v_TexCoordOut = v_TexCoordIn[2];
	EmitVertex();
	EndPrimitive();
}

#shader fragment
#version 450 core

layout(location = 0) in vec3 v_WorldPosition;
layout(location = 1) in vec3 v_WorldNormal;
layout(location = 2) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_Color;

layout(binding = 0) uniform sampler2D u_DiffuseTexture;
layout(binding = 1) uniform sampler2D u_SpecularTexture;
layout(binding = 2) uniform sampler2D u_EmissiveTexture;

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
uniform float u_Time;

vec3 directional(DirLight dirLight, vec3 Ka, vec3 Kd, vec3 Ks, vec3 Ke){
	vec3 color = vec3(0.0);

	vec3 worldNormal = normalize(v_WorldNormal);
	float DdotN = dot(worldNormal, -dirLight.direction);
	if (u_Time > 0.0)
		DdotN = clamp(DdotN, smoothstep(0.0, 0.6, u_Time * 0.5), 1.0);

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

	color = ambient + diffuse + specular + emissive;
	return color;
}

void main(){
	vec3 Kd = texture(u_DiffuseTexture, v_TexCoord).rgb * u_Material.diffuse.rgb;
	vec3 Ks = texture(u_SpecularTexture, v_TexCoord).rgb * u_Material.specular.rgb;
	vec3 Ke = texture(u_EmissiveTexture, v_TexCoord).rgb * u_Material.emissive.rgb;
	vec3 Ka = Kd;

	o_Color = vec4(directional(u_DirLight, Ka, Kd, Ks, Ke), 1.0);
}
