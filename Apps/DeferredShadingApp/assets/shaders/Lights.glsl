#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(location = 0) out vec3 v_ViewPosition;

uniform mat4 u_ModelView;
uniform mat4 u_ModelViewProj;

void main(){
	v_ViewPosition = (u_ModelView * vec4(a_Position, 1.0)).xyz;
	gl_Position = u_ModelViewProj * vec4(a_Position, 1.0);
}

#shader fragment
#version 450 core
layout(location = 0) in vec3 v_ViewPosition;

layout(location = 0) out vec4 o_Color;

struct PointLight {
	vec4 color;
	vec3 position;
};

#define NR_POINT_LIGHTS 16
layout (std140, binding = 0) uniform Lights{
    uniform PointLight u_PointLights[NR_POINT_LIGHTS];
};

layout (std140, binding = 1) uniform Fog{
    uniform vec3 u_FogColor;
    uniform float u_FogDensity;
};

uniform int u_LightIndex;

vec3 fog(vec3 color){
	float zPos = -v_ViewPosition.z;

	float f = clamp(exp(-pow(u_FogDensity * zPos, 2)), 0.00, 0.97);
	color = f * color + (1 - f) * u_FogColor;
	return color;
}

void main(){
	o_Color = vec4(u_PointLights[u_LightIndex].color.rgb * u_PointLights[u_LightIndex].color.a, 1.0);  // Alpha set to 1 here

	if (u_FogDensity > 0.0)
		o_Color = vec4(fog(o_Color.rgb), 1.0);
}
