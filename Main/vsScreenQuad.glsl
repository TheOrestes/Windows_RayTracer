
#version 400

layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_Texcoord;

out vec2 vs_outTexcoord;

void main()
{
	gl_Position = vec4(in_Position.x, in_Position.y, 0.0f, 1.0f);
	vs_outTexcoord = in_Texcoord;
}