#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexture;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;
uniform float time;

out vec2 TexCoords;
out vec2 FragPos;

void main()
{
	vec2 tempTexture = aTexture;
	gl_Position = vec4(aPos.xy, 0.0, 1.0);
	FragPos = aPos;

	if(chaos)
	{
		float strength = 0.3;
		vec2 pos = vec2(tempTexture.x + cos(time) * strength, tempTexture.y + sin(time) * strength);
		TexCoords = pos;
	}
	else if(confuse)
	{
		TexCoords = vec2(1.0 - tempTexture.x, 1.0 - tempTexture.y);
	}
	else
	{
		TexCoords = tempTexture;
	}

	if(shake)
	{
		float strength = 0.01;
		gl_Position.x += cos(time * 10.0) * strength;
		gl_Position.y += cos(time * 10.0) * strength;
	}

}