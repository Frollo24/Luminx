#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

layout(location = 0) out vec3 v_Color;
layout(location = 1) out vec2 v_TexCoord;

uniform mat4 u_ModelViewProj;

void main(){
	gl_Position = u_ModelViewProj * vec4(a_Position, 1.0);
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
}

#shader fragment
#version 450 core

layout(location = 0) in vec3 v_Color;
layout(location = 1) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_Color;

layout(location = 0) uniform sampler2D u_Texture;

uniform bool u_EnableColoring;
uniform bool u_ShowTexCoords;

void main(){
	if (u_ShowTexCoords){
		o_Color = vec4(v_TexCoord, 0.0, 1.0);
	} else {
		vec3 normColor = v_Color * 0.75 + 0.25;
		vec4 texColor = texture(u_Texture, v_TexCoord);

		if (u_EnableColoring)
			o_Color = vec4(normColor, 1.0) * texColor;
		else
			o_Color = texColor;
	}
}
