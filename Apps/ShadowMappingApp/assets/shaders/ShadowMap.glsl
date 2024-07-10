#shader vertex
#version 450

layout (location = 0) in vec3 a_Position;

uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

void main()
{
    gl_Position = u_LightSpaceMatrix * u_Model * vec4(a_Position, 1.0);
} 

#shader fragment
#version 450

layout(location = 0) out vec4 o_Color;

void main()
{             
    //float depthValue = gl_FragCoord.z;
    //o_Color = vec4(vec3(depthValue), 1.0);
} 