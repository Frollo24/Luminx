#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

layout(location = 0) out vec3 v_WorldPosition;
layout(location = 1) out vec3 v_WorldNormal;
layout(location = 2) out vec2 v_TexCoord;
layout(location = 3) out vec3 v_ViewPosition;
layout(location = 4) out mat3 v_TBN;

uniform mat4 u_Model;
uniform mat4 u_ModelView;
uniform mat4 u_Normal;
uniform mat4 u_ModelViewProj;

void main(){
	gl_Position = u_ModelViewProj * vec4(a_Position, 1.0);
	v_WorldPosition = (u_Model * vec4(a_Position, 1.0)).xyz;
	v_ViewPosition = (u_ModelView * vec4(a_Position, 1.0)).xyz;
	v_WorldNormal = (u_Normal * vec4(a_Normal, 0.0)).xyz;
	vec3 T = normalize((u_Model * vec4(a_Tangent, 0.0)).xyz);
	vec3 B = normalize((u_Model * vec4(a_Bitangent, 0.0)).xyz);
	vec3 N = normalize(v_WorldNormal);
	v_TBN = mat3(T, B, N);
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 450 core

layout(location = 0) in vec3 v_WorldPosition;
layout(location = 1) in vec3 v_WorldNormal;
layout(location = 2) in vec2 v_TexCoord;
layout(location = 3) in vec3 v_ViewPosition;
layout(location = 4) in mat3 v_TBN;

layout(location = 0) out vec4 o_Position;
layout(location = 1) out vec4 o_Normal;
layout(location = 2) out vec4 o_Albedo;
layout(location = 3) out vec4 o_SpecGloss;
layout(location = 4) out vec4 o_Emissive;

layout(binding = 0) uniform sampler2D u_DiffuseTexture;
layout(binding = 1) uniform sampler2D u_SpecularTexture;
layout(binding = 2) uniform sampler2D u_EmissiveTexture;
layout(binding = 3) uniform sampler2D u_NormalTexture;

struct Material {
	vec4 diffuse;
	vec4 specular;
	vec4 emissive;
	float glossiness;
};

uniform Material u_Material;
uniform vec3 u_CameraPosition;
uniform bool u_UsingNormalmap;

void main(){
	vec3 Kd = texture(u_DiffuseTexture, v_TexCoord).rgb * u_Material.diffuse.rgb;
	vec3 Ks = texture(u_SpecularTexture, v_TexCoord).rgb * u_Material.specular.rgb;
	vec3 Ke = texture(u_EmissiveTexture, v_TexCoord).rgb * u_Material.emissive.rgb;

	vec3 normal = normalize(v_WorldNormal);
	if (textureSize(u_NormalTexture, 0).x > 1 && u_UsingNormalmap) {
		normal = texture(u_NormalTexture, v_TexCoord).xyz;
		normal = normal * 2.0 - 1.0;
		normal = normalize(v_TBN * normal);
	}

	o_Position = vec4(v_WorldPosition, v_ViewPosition.z);
	o_Normal = vec4(normal, 1.0);
	o_Albedo = vec4(Kd, 1.0);
	o_SpecGloss = vec4(Ks, u_Material.glossiness);
	o_Emissive = vec4(Ke, 1.0);
}
