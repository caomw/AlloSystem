#include "allocore/graphics/al_FBO.hpp"
#include <stdio.h>
namespace al{

RBO::RBO(Graphics::Format format)
:	mFormat(format)
{}

void RBO::onCreate(){
	GLuint i;
	glGenRenderbuffersEXT(1,&i);
	mID=i;
}

void RBO::onDestroy(){
	GLuint i=id();
	glDeleteRenderbuffersEXT(1,&i);
}

Graphics::Format RBO::format() const { return mFormat; }

RBO& RBO::format(Graphics::Format v){ mFormat=v; return *this; }

void RBO::bind(){ validate(); bind(id()); }

void RBO::unbind(){ bind(0); }

bool RBO::resize(unsigned w, unsigned h){
	bind();
	bool r = resize(format(), w, h);
	unbind();
	return r;
}

// static functions
unsigned RBO::maxSize(){
	int s;
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &s);
	return s;
}

void RBO::bind(unsigned id){ glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, id); }

bool RBO::resize(Graphics::Format format, unsigned w, unsigned h){
	unsigned mx = maxSize();
	if(w > mx || h > mx) return false;
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, format, w, h);
	return true;
}




void FBO::onCreate(){
	GLuint i;
	glGenFramebuffersEXT(1,&i);
	mID=i;
}

void FBO::onDestroy(){
	GLuint i=id();
	glDeleteFramebuffersEXT(1,&i);
}

FBO& FBO::attachRBO(const RBO& rbo, Attachment att){
	bind();
	renderBuffer(rbo.id(), att);
	unbind();
	return *this;
}

FBO& FBO::detachRBO(Attachment att){
	bind();
	renderBuffer(0, att);
	unbind();
	return *this;
}

FBO& FBO::attachTexture2D(unsigned texID, Attachment att, int level){
	bind();
	texture2D(texID, att, level);
	unbind();
	return *this;
}

FBO& FBO::detachTexture2D(Attachment att, int level){
	attachTexture2D(0,att,level);
	return *this;
}

void FBO::bind(){ validate(); bind(id()); }

void FBO::unbind(){ bind(0); }

GLenum FBO::status(){
	begin();
	int r=glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	end();
	return r;
}

const char * FBO::statusString(){ return statusString(status()); }

const char * FBO::statusString(GLenum stat){
	#define CS(v) case v: return #v;
	switch(stat){
	CS(GL_FRAMEBUFFER_COMPLETE_EXT)
	CS(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT)
	CS(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT)
	CS(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)
	CS(GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)
	CS(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT)
	CS(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT)
	CS(GL_FRAMEBUFFER_UNSUPPORTED_EXT)
	default: return "GL_FRAMEBUFFER_UNKNOWN";
	};
}

// static functions
void FBO::bind(unsigned fboID){
	AL_GRAPHICS_ERROR("(before FBO::bind)", fboID);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
	AL_GRAPHICS_ERROR("binding FBO", fboID);
}

void FBO::end(){
	AL_GRAPHICS_ERROR("(before FBO::end)",-1);
	bind(0);
	AL_GRAPHICS_ERROR("unbinding FBO",-1);
}

void FBO::renderBuffer(unsigned rboID, Attachment att){
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, att, GL_RENDERBUFFER_EXT, rboID);
}

void FBO::texture2D(GLuint texID, Attachment att, int level){
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, att, GL_TEXTURE_2D, texID, level);
}

} // al::
