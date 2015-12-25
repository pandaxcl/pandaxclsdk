//
//  opengl.hpp
//  study-opengl
//
//  Created by 熊 春雷 on 15/12/20.
//  Copyright © 2015年 pandaxcl. All rights reserved.
//

#ifndef opengl_hpp
#define opengl_hpp

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <functional>
#include <string>
#include <vector>
class opengl;
class program;
class window
{
public:
	window();
	~window();
	friend window&operator<<(window&w, opengl&o);
	friend window&operator<<(window&w, program&o);
};

class opengl
{
	friend window&operator<<(window&w, opengl&o);
public:
    opengl();
    ~opengl();
    opengl&display(std::function<void()>&&);
    opengl&initialize(std::function<void()>&&);
private:
    std::function<void()> lambda_display    = [](){};
    std::function<void()> lambda_initialize = [](){};
};

class shader
{
    shader() = delete;
    
public:
    shader(GLint kind, const std::string&source);
    
    shader(const shader&o) { __copy__(o); }
    shader&operator=(const shader&o) { __copy__(o); return *this; }
    shader(shader&&o) { __move__(std::move(o)); }
    shader&operator=(shader&&o) { __move__(std::move(o)); return *this; }
    shader&     compile();
    
    std::string kind_name();
    GLuint      gl_shader() { return _theShader; }
private:
    std::string _source;
    GLint       _kind;
    GLuint      _theShader;
    std::string _log;
    
    void __move__(shader&&o);
    void __copy__(const shader&o);
};

typedef shader shader_t;

class program
{
	friend window&operator<<(window&w, program&o);
public:
    program&shader(GLint kind, const std::string&source);
    program&shader(shader_t&&o);
    program&link();
    GLuint gl_program() { return _theProgram; }
	void   on_stream_out(std::function<void()>&&f) { this->lambda_on_stream_out = std::move(f); }
private:
    std::vector<shader_t> _shaders;
    GLuint                _theProgram;
    std::string           _log;
	std::function<void()> lambda_on_stream_out = nullptr;
};

#endif /* opengl_hpp */
