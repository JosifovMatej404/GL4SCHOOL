#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float blendFactor;

void main()
{
	FragColor = mix(texture(texture2, TexCoord), texture(texture1, TexCoord), blendFactor);
}
