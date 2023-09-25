#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;

uniform sampler2D image_texture;

out vec4 FragColor;

void main()
{
	FragColor = texture(image_texture, TexCoords) * ParticleColor;
}