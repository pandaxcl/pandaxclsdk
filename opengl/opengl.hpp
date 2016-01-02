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
#include <GLFW/glfw3.h>
#include <functional>
#include <string>
#include <vector>
#include <iostream>

class opengl;
class window
{
	friend window&operator<<(window&&w, opengl&o);
public:
	window();
	~window();
private:
	std::function<void()> lambda_display;
	GLFWwindow* theWindow = nullptr;
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

template<typename Description>class gpu_program
{
	struct detect
	{
#define GPU_PROGRAM_DETECT(vertex_shader, shaderType)                                                                               \
        struct vertex_shader                                                                                                        \
        {                                                                                                                           \
            template<typename D>static typename std::enable_if < !std::is_void<decltype(D::vertex_shader)>::value> ::type test(int);\
            template<typename D>static int test(...);                                                                               \
            template<typename D>static constexpr bool exist() { return std::is_void<decltype(test<D>(0))>::value; }                 \
                                                                                                                                    \
            template<typename D>static void send_to_opengl_as_shader(...) { }                                                       \
            template<typename D>static typename std::enable_if <vertex_shader::exist<D>()>::type                                    \
                send_to_opengl_as_shader(GLuint programHandle)                                                                      \
            {                                                                                                                       \
                GLuint shaderHandle = glCreateShader(shaderType);                                                                   \
                if (0 == shaderHandle)                                                                                              \
                {                                                                                                                   \
                    std::cerr << "Error creating " << #shaderType << " shader." << std::endl;                                       \
                    exit(1);                                                                                                        \
                }                                                                                                                   \
                                                                                                                                    \
                const GLchar* codeArray[] = { D::vertex_shader() };                                                                 \
                glShaderSource(shaderHandle, 1, codeArray, NULL);                                                                   \
                glCompileShader(shaderHandle);                                                                                      \
                                                                                                                                    \
                GLint result = GL_FALSE;                                                                                            \
                glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);                                                            \
                if (GL_FALSE == result)                                                                                             \
                {                                                                                                                   \
                    std::cerr << shaderType << " compilation failed!" << std::endl;                                                 \
                    std::string log;                                                                                                \
                    GLint logLen = 0;                                                                                               \
                    glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLen);                                                       \
                    if (logLen > 0)                                                                                                 \
                    {                                                                                                               \
                        log.resize(logLen);                                                                                         \
                        GLsizei written;                                                                                            \
                        glGetShaderInfoLog(shaderHandle, logLen, &written, &log[0]);                                                \
                        std::cerr << "Shader log:" << std::endl << log << std::endl;                                                \
                    }                                                                                                               \
                }                                                                                                                   \
                glAttachShader(programHandle, shaderHandle);                                                                        \
            }                                                                                                                       \
        };                                                                                                                          \
		/* 上一行是宏的续行符，所以这一行不能有代码，但是注释却是可以的 */
		GPU_PROGRAM_DETECT(vertex_shader, GL_VERTEX_SHADER);
		GPU_PROGRAM_DETECT(fragment_shader, GL_FRAGMENT_SHADER);

#undef GPU_PROGRAM_DETECT

		static void send_to_opengl_as_shaders(GLuint programHandle)
		{
			vertex_shader::template send_to_opengl_as_shader<Description>(programHandle);
			fragment_shader::template send_to_opengl_as_shader<Description>(programHandle);
		}
	};
	GLuint programHandle = 0;
public:
	GLuint gl_handle() { return programHandle; }
	gpu_program& send_to_opengl()
	{
		this->programHandle = glCreateProgram();
		if (0 == programHandle)
		{
			std::cerr << "Error creating program object." << std::endl;
			exit(1);
		}

		detect::send_to_opengl_as_shaders(programHandle);

		glLinkProgram(programHandle);

		GLint status = GL_FALSE;
		glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
		if (GL_FALSE == status)
		{
			std::string log;
			std::cerr << "Failed to link shader program!" << std::endl;
			GLint logLen;
			glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLen);
			if (logLen > 0)
			{
				log.resize(logLen);
				GLsizei written = 0;
				glGetProgramInfoLog(programHandle, logLen, &written, &log[0]);
				std::cerr << "Program log:" << std::endl << log << std::endl;
			}
		}
		return *this;
	}

	gpu_program&report()
	{
		report_vertex_locations();
		report_uniform_variables();
		return *this;
	}
private:
	void report_vertex_locations()
	{
		GLint maxLength, nAttribs;
		glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &nAttribs);
		glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

		GLchar * name = (GLchar *)malloc(maxLength);

		GLint written, size, location;
		GLenum type; printf(" Index | Name\n");
		printf("------------------------------------------------\n");
		for (int i = 0; i < nAttribs; i++)
		{
			glGetActiveAttrib(programHandle, i, maxLength, &written, &size, &type, name);
			location = glGetAttribLocation(programHandle, name);
			printf(" %-5d | %s\n", location, name);
		}
		free(name);
	}
	void report_uniform_variables()
	{
		GLint nUniforms, maxLen; 
		glGetProgramiv(programHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen); 
		glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &nUniforms);
		GLchar * name = (GLchar *)malloc(maxLen);
		GLint size, location;
		GLsizei written; 
		GLenum type; 
		printf(" Location | Name\n"); 
		printf("------------------------------------------------\n"); 
		for (int i = 0; i < nUniforms; ++i) 
		{
			glGetActiveUniform(programHandle, i, maxLen, &written, &size, &type, name);   
			location = glGetUniformLocation(programHandle, name);    
			printf(" %-8d | %s\n", location, name); 
		}
		free(name);
	}
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
				send_to_opengl_as_buffer_object(GLuint bufferType/* GL_ARRAY_BUFFER etc. */, GLuint vboHandles[])                         \
			{                                                                                                                             \
                glBindBuffer(bufferType, vboHandles[D::position##_location]);                                                             \
				D::position##_as_buffer_object();                                                                                         \
			}                                                                                                                             \
			template<typename D>static void send_to_opengl_as_buffer_object(...) { }                                                      \
                                                                                                                                          \
			template<typename D>static typename std::enable_if <position::exist<D>()>::type                                               \
				send_to_opengl_as_member_of_vertex_array_object(GLuint vaoHandle, GLuint vboHandles[])                                    \
			{                                                                                                                             \
				glEnableVertexAttribArray(D::position##_location);                                                                        \
                glBindBuffer(GL_ARRAY_BUFFER, vboHandles[D::position##_location]);                                                        \
				D::position##_as_member_of_vertex_array_object();                                                                         \
			}                                                                                                                             \
			template<typename D>static void send_to_opengl_as_member_of_vertex_array_object(...) { }                                      \
		}                                                                                                                                 \
		/* 由于上一行是宏定义的续行符，所以这一行上不能有任何代码，但是注释却是可以的 */
		VERTEX_ARRAY_OBJECT_DETECT(position);
		VERTEX_ARRAY_OBJECT_DETECT(color);
		VERTEX_ARRAY_OBJECT_DETECT(normal);
		VERTEX_ARRAY_OBJECT_DETECT(texture);
		VERTEX_ARRAY_OBJECT_DETECT(element);
#undef VERTEX_ARRAY_OBJECT_DETECT

		static constexpr GLuint N_vbo_handles() {
			return position::template exist<Description>()
				+ color::template exist<Description>()
				+ normal::template exist<Description>()
				+ texture::template exist<Description>()
				+ element::template exist<Description>();
		}
		static void send_to_opengl_as_buffer_objects(GLuint vboHandles[])
		{
			// Create the buffer objects
			glGenBuffers(detect::N_vbo_handles(), vboHandles);
			position::template send_to_opengl_as_buffer_object<Description>(GL_ARRAY_BUFFER, vboHandles);
			color   ::template send_to_opengl_as_buffer_object<Description>(GL_ARRAY_BUFFER, vboHandles);
			normal  ::template send_to_opengl_as_buffer_object<Description>(GL_ARRAY_BUFFER, vboHandles);
			texture ::template send_to_opengl_as_buffer_object<Description>(GL_ARRAY_BUFFER, vboHandles);
			element ::template send_to_opengl_as_buffer_object<Description>(GL_ELEMENT_ARRAY_BUFFER, vboHandles);
		}
		static void send_to_opengl_as_vertex_array_object(GLuint vaoHandle, GLuint vboHandles[])
		{
			// Create the buffer objects
			glBindVertexArray(vaoHandle);
			position::template send_to_opengl_as_member_of_vertex_array_object<Description>(vaoHandle, vboHandles);
			color   ::template send_to_opengl_as_member_of_vertex_array_object<Description>(vaoHandle, vboHandles);
			normal  ::template send_to_opengl_as_member_of_vertex_array_object<Description>(vaoHandle, vboHandles);
			texture ::template send_to_opengl_as_member_of_vertex_array_object<Description>(vaoHandle, vboHandles);
			//element::template send_to_opengl_as_member_of_vertex_array_object<Description>(vaoHandle, vboHandles);
		}

		template<typename D> static void _bind_element_array_buffer(...) {}
		template<typename D> static typename std::enable_if<element::template exist<D>()>::type
			_bind_element_array_buffer(GLuint vboHandles[])
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandles[D::element_location]);
		}

		template<typename D> static void display(...) {}
		template<typename D> static typename std::enable_if<std::is_void<decltype(D::display())>::value>::type 
			display(GLuint vboHandles[])
		{
			_bind_element_array_buffer<D>(vboHandles);
			D::display();
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
	void display()
	{
		glBindVertexArray(vaoHandle);
		detect::template display<Description>(vboHandles);
	}
};

#endif /* opengl_hpp */
