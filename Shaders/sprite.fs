#version 330 core
in vec2 TexCoords;

uniform sampler2D image_texture;
uniform vec3 spriteColor;

out vec4 FragColor;
void main()
{
	vec4 texture_color = texture(image_texture, TexCoords).rgba;
	if(texture_color.a < 0.1)
	{
		discard;
	}
	vec3 color = texture_color.rgb * spriteColor;
//	color = pow(color, vec3(1.0 / 2.0));
	FragColor = vec4(color, 1.0);
}