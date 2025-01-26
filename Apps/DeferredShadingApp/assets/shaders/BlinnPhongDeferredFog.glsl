#shader vertex
#version 450 core

layout(location = 0) out vec2 v_TexCoord;

const vec2 ScreenCoords[6] = vec2[](
	vec2(-1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(1.0, 1.0),
	vec2(1.0, 1.0),
	vec2(-1.0, 1.0),
	vec2(-1.0, -1.0)
);

void main(){
	v_TexCoord = ScreenCoords[gl_VertexID];
	gl_Position = vec4(v_TexCoord, 0.0, 1.0);
	// v_TexCoord.y = -v_TexCoord.y;
	v_TexCoord = 0.5 * v_TexCoord + 0.5;
}

#shader fragment
#version 450 core

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_Color;

layout(binding = 0) uniform sampler2D u_PositionTexture;
layout(binding = 1) uniform sampler2D u_NormalTexture;
layout(binding = 2) uniform sampler2D u_AlbedoTexture;
layout(binding = 3) uniform sampler2D u_SpecGlossTexture;
layout(binding = 4) uniform sampler2D u_EmissiveTexture;
layout(binding = 5) uniform sampler2D u_DepthTexture;

struct PointLight {
	vec4 color;
	vec3 position;
	float radius;
};

#define NR_POINT_LIGHTS 1024
layout (std140, binding = 0) uniform Lights{
    uniform PointLight u_PointLights[NR_POINT_LIGHTS];
};

layout (std140, binding = 1) uniform Fog{
    uniform vec3 u_FogColor;
    uniform float u_FogDensity;
};

uniform vec3 u_CameraPosition;
uniform int u_GBufferTexture;

vec3 point(PointLight pointLight, vec3 Kd, vec3 Ks, vec3 worldNormal, vec3 worldPos, float glossiness){
	vec3 color = vec3(0.0);

	vec3 direction = normalize(pointLight.position - worldPos);
	float DdotN = dot(worldNormal, direction);
	float intensity = pointLight.color.w;
	float glossyExp = 10.0 * glossiness + 1.0;
	float glossyFactor = pow(2.0, glossyExp);

	float dist = distance(pointLight.position, worldPos);
	float attenuation = 1.0 / (0.1 + dist * dist);
	attenuation = mix(attenuation, 0, clamp(dist / pointLight.radius, 0, 1));
		
	// Diffuse component
	vec3 diffuse = Kd * pointLight.color.rgb * intensity * max(0.0, DdotN);
	diffuse *= attenuation;

	// Specular component
	vec3 L = normalize(direction);
	vec3 V = normalize(u_CameraPosition - worldPos);
	vec3 H = normalize(L + V);
	vec3 N = worldNormal;
	vec3 specular = Ks * pointLight.color.rgb * intensity * pow(max(0.0, dot(N, H)), glossyFactor) * max(0.0, DdotN);
	specular *= attenuation;

	color = diffuse + specular;
	return color;
}

vec3 emissive(vec3 Ke, vec3 worldPos) {
	float dist = distance(u_CameraPosition, worldPos);

	// Emissive component
	float attFactor = 1.5;
	float emmAttenuation = 0.1 + dist * dist;
	vec3 emissive = Ke * (-attFactor * smoothstep(0, 10, emmAttenuation) + 2.0 * attFactor);
	return emissive;
}

vec3 fog(vec3 color){
	float zPos = -texture(u_PositionTexture, v_TexCoord).w;

	float f = clamp(exp(-pow(u_FogDensity * zPos, 2)), 0.00, 0.97);
	color = f * color + (1 - f) * u_FogColor;
	return color;
}

vec4 selectGBufferTexture() {
	switch (u_GBufferTexture) {
		case 1:
			return texture(u_PositionTexture, v_TexCoord);
		case 2:
			vec3 normal = texture(u_NormalTexture, v_TexCoord).xyz;
			normal = normal * 0.5 + 0.5;
			normal = pow(normal, vec3(2.2)); // Gamma correction for display on screen
			return vec4(normal, 1.0);
		case 3:
			return texture(u_AlbedoTexture, v_TexCoord);
		case 4:
			return texture(u_SpecGlossTexture, v_TexCoord);
		case 5:
			return texture(u_EmissiveTexture, v_TexCoord);
		default:
			return vec4(1.0, 0.0, 1.0, 1.0);
	}
}

void main(){
	vec3 Kd = texture(u_AlbedoTexture, v_TexCoord).rgb;
	vec3 Ks = texture(u_SpecGlossTexture, v_TexCoord).rgb;
	vec3 Ke = texture(u_EmissiveTexture, v_TexCoord).rgb;
	vec3 Ka = Kd;

	if (texture(u_AlbedoTexture, v_TexCoord).a != 1.0)
		discard;

	gl_FragDepth = texture(u_DepthTexture, v_TexCoord).r;
	o_Color = vec4(0.0, 0.0, 0.0, 1.0);  // Alpha set to 1 here

	vec3 worldNormal = texture(u_NormalTexture, v_TexCoord).xyz;
	vec3 worldPos = texture(u_PositionTexture, v_TexCoord).xyz;
	float glossiness = texture(u_SpecGlossTexture, v_TexCoord).a;

	// Ambient component
	vec3 ambient = 0.05 * Ka;
	o_Color += vec4(ambient, 0.0);

	if (u_GBufferTexture == 0) {
		for(int i = 0; i < NR_POINT_LIGHTS; i++)
			o_Color += vec4(point(u_PointLights[i], Kd, Ks, worldNormal, worldPos, glossiness), 0.0);

		o_Color += vec4(emissive(Ke, worldPos), 0.0);
		if (u_FogDensity > 0.0)
			o_Color = vec4(fog(o_Color.rgb), 1.0);
	} else {
		o_Color = selectGBufferTexture();
	}
}
