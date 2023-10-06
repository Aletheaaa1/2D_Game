#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "SpriteRenderer.h"
#include "../Headers/Shader.h"
#include "../Headers/Texture.h"
#include "../Headers/VertexArrayObject.h"

class PostProcessor
{
public:
	Shader post_processor_shader;
	unsigned int post_processor_texture;
	unsigned int height;
	unsigned int width;
	bool is_confuse;
	bool is_chaos;
	bool is_shake;
	float shake_time;
private:
	unsigned int msaa_fbo;
	unsigned int temp_fbo;
	unsigned int msaa_rbo;
	VertexArrayObject vao;
	unsigned int vbo;

public:
	PostProcessor(Shader shader, unsigned int width, unsigned int height);
	~PostProcessor();

	void BeginRender();
	void EndRender();
	void Render(float time);
private:
	void InitRenderData();
};
