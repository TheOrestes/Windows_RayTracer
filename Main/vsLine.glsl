
#version 400

layout(location=0) in vec3 in_Position;
layout(location=1) in vec4 in_Color;

out vec4 vs_outColor;

uniform mat4 matView;
uniform mat4 matProj;

void main()
{
	mat4 matViewProj = matProj * matView;
	gl_Position = matViewProj * vec4(in_Position, 1.0f);
	vs_outColor = in_Color;
}