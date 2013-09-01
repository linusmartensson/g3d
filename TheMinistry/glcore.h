#pragma once

#define GLEW_STATIC
#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<map>
#include<unordered_map>
#include<string>
#include<sstream>
#define generator(name, func) inline void gen##name (std::string s) { glGen##func ( 1, &id(s) ); }
#define binder(name, func, defaultTarget) inline void bind##name (std::string s, GLenum target = defaultTarget) {glBind##func (target, id(s) ); }
#define bindernt(name, func) inline void bind##name (std::string s) {glBind##func ( id(s) ); }

class glcore {
	
	unsigned long uid;

	std::unordered_map<std::string, GLuint> ids;
	
	glcore(const glcore &rhs);
	glcore& operator=(const glcore &rhs);
public:
	glcore() : uid(0) {}
	inline GLuint& id(std::string s){ return ids[s]; }
	
	static glcore& core();


	inline std::string genuid(){
		std::ostringstream oss;

		//Fair attempt at a unique id that should be likely to hold between executions.
		oss<<glfwGetTime()<<":"<<uid++;

		return oss.str();
	}

	generator(Buffer, Buffers);
	binder(Buffer, Buffer, GL_ARRAY_BUFFER);
	inline void uploadBuffer(GLuint size, void *data, GLenum usage = GL_STATIC_DRAW, GLenum target = GL_ARRAY_BUFFER) { glBufferData(target, size, data, usage); }

	generator(VArray, VertexArrays);
	bindernt(VArray, VertexArray);
	inline void linkBuffer(int index=0, int elems = 3, GLenum type = GL_FLOAT, std::string b = "", int stride=0, int offset=0, std::string va = "") {
		if(va != "") 
			bindVArray(va); 
		if(b != "")
			bindBuffer(b);
		
		glVertexAttribPointer(index, elems, type, GL_FALSE, stride, (void*)offset); 
		glEnableVertexAttribArray(index);
	}
	
	generator(Texture, Textures);
	binder(Texture, Texture, GL_TEXTURE_2D);
	inline void uploadTexture(int w, int h, void *data = 0, GLenum iformat = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, GLenum format = GL_RGBA, GLenum target = GL_TEXTURE_2D, int level = 0, std::string tex = ""){
		if(tex != "") bindTexture(tex); glTexImage2D(target, level, iformat, w, h, 0, format, type, data);
	}
	
	generator(FB, Framebuffers);
	binder(FB, Framebuffer, GL_FRAMEBUFFER);
	inline void linkFBTexture(std::string tex, int attachment = 0, int level = 0) {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+attachment, id(tex), level);
	}
};