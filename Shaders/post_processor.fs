#version 330 core
in vec2 TexCoords;
in vec2 FragPos;

uniform sampler2D scene;
uniform vec2 offset[9];
uniform int edge_kernel[9];
uniform float blur_kernel[9];

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

out vec4 FragColor;

void main()
{
	vec4 color = vec4(0.0);

	vec3 sampler[9];
	if(chaos || shake)
	{
		for(int i = 0; i < 9; i++)
		{
			sampler[i] = vec3(texture(scene, TexCoords + offset[i]));
		}
	}

	if(chaos)
	{
		for(int i = 0; i < 9; i++)
		{
			color += vec4(edge_kernel[i] * sampler[i], 0.0);
		}
		color.a = 1.0;
	}
	else if(confuse)
	{
		color = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0);
	}

	else if(shake)
	{
		for(int i = 0; i < 9; i++)
		{
			color += vec4(blur_kernel[i] * sampler[i], 0.0);
		}
		color.a = 1.0;
	}
	else
	{
		color = texture(scene, TexCoords);
	}

//	color.rgb = color.rgb / (1.0 + color.rgb);
	color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
	FragColor = color;
}