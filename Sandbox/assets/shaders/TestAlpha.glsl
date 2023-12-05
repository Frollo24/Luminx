#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

layout(location = 0) out vec3 v_Normal;

uniform mat4 u_ModelViewProj;

void main(){
	gl_Position = u_ModelViewProj * vec4(a_Position, 1.0);
	v_Normal = a_Normal;
}

#shader fragment
#version 450 core

layout(location = 0) in vec3 v_Normal;

layout(location = 0) out vec4 o_Color;

uniform float u_AlphaValue;

void main(){
	vec3 normColor = v_Normal * 0.5 + 0.5;
	o_Color = vec4(normColor, u_AlphaValue);
}
