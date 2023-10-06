#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;

uniform sampler2D image_texture;

out vec4 FragColor;

void main()
{
	vec4 color = texture(image_texture, TexCoords) * ParticleColor;
	color = color / (1.0 + color);
//	color = pow(color, vec4(1.0 / 2.2));

	FragColor = color;
}