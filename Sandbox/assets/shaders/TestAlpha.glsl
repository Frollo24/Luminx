#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(location = 0) out vec3 v_Normal;
layout(location = 1) out vec2 v_TexCoord;

uniform mat4 u_ModelViewProj;

void main(){
	gl_Position = u_ModelViewProj * vec4(a_Position, 1.0);
	v_Normal = a_Normal;
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 450 core

layout(location = 0) in vec3 v_Normal;
layout(location = 1) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_Color;
layout(location = 1) out vec4 o_TexColor;

layout(std140, binding = 0) uniform TestData{
	vec4 testVector;
};

layout(binding = 0) uniform sampler2D u_Texture;

uniform float u_AlphaValue;

void main(){
	vec3 normColor = v_Normal * 0.5 + 0.5;
	normColor *= testVector.rgb;
	o_Color = vec4(normColor, u_AlphaValue);

	o_TexColor = texture(u_Texture, v_TexCoord);
}
