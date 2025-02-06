#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(location = 0) out vec3 v_Normal;
layout(location = 1) out vec2 v_TexCoord;

void main(){
	gl_Position = vec4(a_Position, 1.0);
	v_Normal = a_Normal;
	v_TexCoord = a_TexCoord;
}

#shader geometry
#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec3 v_NormalIn[];
layout(location = 1) in vec2 v_TexCoordIn[];

layout(location = 0) out vec3 v_NormalOut;
layout(location = 1) out vec2 v_TexCoordOut;

uniform mat4 u_ModelViewProj;
uniform float u_Magnitude;
uniform float u_Time;

vec3 getTriangleNormal() {
	vec3 a = gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz;
	vec3 b = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
	return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal)
{
	vec3 direction = normal * (sin(u_Time) * 0.5 + 0.5) * u_Magnitude; 
	return position + vec4(direction, 0.0);
}

void main() {
	vec3 triangleNormal = getTriangleNormal();

	gl_Position = u_ModelViewProj * explode(gl_in[0].gl_Position, triangleNormal);
	v_TexCoordOut = v_TexCoordIn[0];
	v_NormalOut = v_NormalIn[0];
	EmitVertex();
	gl_Position = u_ModelViewProj * explode(gl_in[1].gl_Position, triangleNormal);
	v_TexCoordOut = v_TexCoordIn[1];
	v_NormalOut = v_NormalIn[1];
	EmitVertex();
	gl_Position = u_ModelViewProj * explode(gl_in[2].gl_Position, triangleNormal);
	v_TexCoordOut = v_TexCoordIn[2];
	v_NormalOut = v_NormalIn[2];
	EmitVertex();
	EndPrimitive();
}

#shader fragment
#version 450 core

layout(location = 0) in vec3 v_Normal;
layout(location = 1) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_Color;

layout(binding = 0) uniform sampler2D u_Texture;

void main(){
	vec3 normColor = v_Normal * 0.5 + 0.5;
	o_Color = vec4(normColor, 1.0) * texture(u_Texture, v_TexCoord);
}
