#shader vertex
#version 450 core

layout (location = 0) in vec3 a_Pos;

uniform mat4 u_ViewProj;

layout (location = 0) out vec3 v_TexCoords;

void main()
{
    v_TexCoords = a_Pos;
    vec4 pos = u_ViewProj * vec4(a_Pos, 1.0);
    gl_Position = pos.xyww;
} 

#shader fragment
#version 450 core

layout (location = 0) in vec3 v_TexCoords;

uniform samplerCube u_Skybox;

layout (location = 0) out vec4 o_Color;

void main()
{    
    o_Color = texture(u_Skybox, v_TexCoords);
}