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
template<typename T> class buffer;
class window
{
public:
	window();
	~window();
	friend window&operator<<(window&&w, opengl&o);
};

class opengl
{
	friend window&operator<<(window&&w, opengl&o);
public:
    opengl();
    ~opengl();
    opengl&display(std::function<void()>&&);
    opengl&initialize(std::function<void()>&&);
private:
    std::function<void()> lambda_display    = nullptr;
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
	friend opengl&operator<<(opengl&gl, program&o);
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

/*************************************************************************************************************
	struct Description
	{
		enum { 
			position_location = 0,
			color_location = 1,
		};
		static void position_as_buffer_object(GLuint bufferHandle)
		{
			GLfloat data[] = {
				-0.8f, -0.8f, 0.0f,
				0.8f, -0.8f, 0.0f,
				0.0f,  0.8f, 0.0f };
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), data, GL_STATIC_DRAW);
		}
		static void position_as_member_of_vertex_array_object(GLuint vaoHandle, GLuint bufferHandle)
		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
		}

		static void color_as_buffer_object(GLuint bufferHandle)
		{
			GLfloat data[] = {
				1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f };
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), data, GL_STATIC_DRAW);
		}
		static void color_as_member_of_vertex_array_object(GLuint vaoHandle, GLuint bufferHandle)
		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
			glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
		}
	};
**************************************************************************************************************/
template<typename Description>
class vertex_array_object
{
	struct detect
	{
#define VERTEX_ARRAY_OBJECT_DETECT(position)                                                                                              \
		struct position                                                                                                                   \
		{                                                                                                                                 \
			template<typename D>static typename std::enable_if < !std::is_void<decltype(D::position##_location)>::value> ::type test(int);\
			template<typename D>static int test(...);                                                                                     \
			template<typename D>static constexpr bool exist() { return std::is_void<decltype(test<D>(0))>::value; }                       \
                                                                                                                                          \
			template<typename D>static typename std::enable_if <position::exist<D>()>::type                                               \
				send_to_opengl_as_buffer_object(GLuint vboHandles[])                                                                      \
			{                                                                                                                             \
				D::position##_as_buffer_object(vboHandles[D::position##_location]);                                                       \
			}                                                                                                                             \
			template<typename D>static void send_to_opengl_as_buffer_object(...) { }                                                      \
                                                                                                                                          \
			template<typename D>static typename std::enable_if <position::exist<D>()>::type                                               \
				send_to_opengl_as_member_of_vertex_array_object(GLuint vaoHandle, GLuint vboHandles[])                                    \
			{                                                                                                                             \
				glEnableVertexAttribArray(D::position##_location);                                                                        \
				D::position##_as_member_of_vertex_array_object(vaoHandle, vboHandles[D::position##_location]);                            \
			}                                                                                                                             \
			template<typename D>static void send_to_opengl_as_member_of_vertex_array_object(...) { }                                      \
		}                                                                                                                                 \
		/* 由于上一行是宏定义的续行符，所以这一行上不能有任何代码，但是注释却是可以的 */
		VERTEX_ARRAY_OBJECT_DETECT(position);
		VERTEX_ARRAY_OBJECT_DETECT(color);
		VERTEX_ARRAY_OBJECT_DETECT(normal);
		VERTEX_ARRAY_OBJECT_DETECT(index);
#undef VERTEX_ARRAY_OBJECT_DETECT

		static constexpr GLuint N_vbo_handles() {
			return position::template exist<Description>()
				+ color::template exist<Description>()
				+ normal::template exist<Description>()
				+ index::template exist<Description>();
		}
		static void send_to_opengl_as_buffer_objects(GLuint vboHandles[])
		{
			// Create the buffer objects
			glGenBuffers(detect::N_vbo_handles(), vboHandles);
			position::template send_to_opengl_as_buffer_object<Description>(vboHandles);
			color::template send_to_opengl_as_buffer_object<Description>(vboHandles);
			normal::template send_to_opengl_as_buffer_object<Description>(vboHandles);
			index::template send_to_opengl_as_buffer_object<Description>(vboHandles);
		}
		static void send_to_opengl_as_vertex_array_object(GLuint vaoHandle, GLuint vboHandles[])
		{
			// Create the buffer objects
			glBindVertexArray(vaoHandle);
			position::template send_to_opengl_as_member_of_vertex_array_object<Description>(vaoHandle, vboHandles);
			color::template send_to_opengl_as_member_of_vertex_array_object<Description>(vaoHandle, vboHandles);
			normal::template send_to_opengl_as_member_of_vertex_array_object<Description>(vaoHandle, vboHandles);
			index::template send_to_opengl_as_buffer_object<Description>(vaoHandle, vboHandles);
		}
	};
	GLuint vboHandles[detect::N_vbo_handles()] = {0};
	GLuint vaoHandle = 0;
public:
	GLuint gl_handle() { return vaoHandle; }
	vertex_array_object& send_to_opengl()
	{
		glGenVertexArrays(1, &vaoHandle);
		detect::send_to_opengl_as_buffer_objects(vboHandles);
		detect::send_to_opengl_as_vertex_array_object(vaoHandle, vboHandles);
		return *this;
	}
};

#endif /* opengl_hpp */
