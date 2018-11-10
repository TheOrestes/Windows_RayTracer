
#version 400

in vec2 vs_outTexcoord;
out vec4 outColor;

uniform sampler2D screenTexture;

void main()
{
	outColor = texture(screenTexture, vs_outTexcoord);
}