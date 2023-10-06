#include "PostProcessor.h"

PostProcessor::PostProcessor(Shader shader, unsigned int width, unsigned int height) :post_processor_shader(shader), width(width), height(height), is_chaos(false), is_confuse(false), is_shake(false)
{
	glGenFramebuffers(1, &this->msaa_fbo);
	glGenFramebuffers(1, &this->temp_fbo);
	glGenRenderbuffers(1, &this->msaa_rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, this->msaa_fbo);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, this->width, this->height, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);

	//glBindRenderbuffer(GL_RENDERBUFFER, this->msaa_rbo);
	//glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA, this->width, this->height);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaa_rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Failed to initialize MSAA FBO \n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, this->temp_fbo);
	glGenTextures(1, &post_processor_texture);
	glBindTexture(GL_TEXTURE_2D, post_processor_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, post_processor_texture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Failed to initialize TEMP FBO \n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	this->InitRenderData();

	float offset = 1.0f / 300.0f;
	float offsets[9][2] = {
		   { -offset,  offset  },  // top-left
		   {  0.0f,    offset  },  // top-center
		   {  offset,  offset  },  // top-right
		   { -offset,  0.0f    },  // center-left
		   {  0.0f,    0.0f    },  // center-center
		   {  offset,  0.0f    },  // center - right
		   { -offset, -offset  },  // bottom-left
		   {  0.0f,   -offset  },  // bottom-center
		   {  offset, -offset  }   // bottom-right
	};
	int edge_kernel[9] = {
		1, 1, 1,
		1, -8, 1,
		1, 1, 1
	};
	float blur_kernel[9] = {
	   1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
	   2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
	   1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
	};

	this->post_processor_shader.Bind();
	this->post_processor_shader.SetUniform1i("scene", 0);
	this->post_processor_shader.SetUniformfs("blur_kernel", 9, blur_kernel);
	this->post_processor_shader.SetUniformis("edge_kernel", 9, edge_kernel);
	for (unsigned int i = 0; i < 9; i++)
	{
		this->post_processor_shader.SetUniform2v("offset[" + std::to_string(i) + "]", glm::vec2(offsets[i][0], offsets[i][1]));
	}
}

PostProcessor::~PostProcessor()
{
	glDeleteTextures(1, &this->post_processor_texture);
}

void PostProcessor::BeginRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->msaa_fbo);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void PostProcessor::EndRender()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->msaa_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->temp_fbo);
	glBlitFramebuffer(0, 0, this->width, this->height, 0, 0, this->width, this->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void PostProcessor::Render(float time)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, post_processor_texture);
	this->post_processor_shader.Bind();
	this->post_processor_shader.SetUniform1f("time", time);
	this->post_processor_shader.SetUniform1i("confuse", this->is_confuse);
	this->post_processor_shader.SetUniform1i("chaos", this->is_chaos);
	this->post_processor_shader.SetUniform1i("shake", this->is_shake);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	this->vao.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void PostProcessor::InitRenderData()
{
	float vertices[] = {
		// pos			// tex
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f,
	};

	glGenBuffers(1, &this->vbo);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	this->vao.Bind();
	this->vao.AddBuffer(0, 2, GL_FLOAT, false, 4 * sizeof(float), 0);
	this->vao.AddBuffer(1, 2, GL_FLOAT, false, 4 * sizeof(float), 2 * sizeof(float));
}