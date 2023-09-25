#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexture;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;

out vec2 TexCoords;
out vec4 ParticleColor;

void main()
{
	TexCoords = aTexture;
	ParticleColor = color;

	float scale = 10.0f;
	vec4 position = vec4((aPos * scale) + offset, 0.0, 1.0);
	gl_Position = projection * position;
}