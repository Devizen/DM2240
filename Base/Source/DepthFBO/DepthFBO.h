#ifndef DEPTH_FBO_H
#define DEPTH_FBO_H

#include <GL/glew.h>
#include "Mtx44.h"

class DepthFBO
{
	static DepthFBO* s_instance;
public:
    DepthFBO();
    ~DepthFBO();

	static DepthFBO* GetInstance(void);
    bool Init(unsigned width, unsigned height);
    void BindForWriting();
    void BindForReading(GLenum textureUnit);

	GLuint GetFBO() {return m_fbo;}
	GLuint GetTexture() {return m_texture;}
	unsigned GetWidth() {return m_width;}
	unsigned GetHeight() {return m_height;}

	enum RENDER_PASS
	{
		RENDER_PASS_PRE,
		RENDER_PASS_MAIN,
	};
	Mtx44 m_lightDepthProj;
	Mtx44 m_lightDepthView;
	RENDER_PASS m_renderPass;

private:
    GLuint m_fbo;		// The frame buffer object, to be written to
    GLuint m_texture;	// Depth texture, to be sampled later in your shader
	unsigned m_width;
	unsigned m_height;
};

#endif
