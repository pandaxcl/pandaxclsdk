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
#include <future>

template<typename Description> class study_opengl
{
    typedef study_opengl self_t;
    struct detect
    {
#define STUDY_OPENGL_DETECT(initialize, ...)                                                                                          \
        struct initialize##_t                                                                                                         \
        {                                                                                                                             \
            template<typename D>static int test(...);                                                                                 \
            template<typename D>static std::enable_if_t<std::is_function<decltype(&D::initialize)(D,##__VA_ARGS__)>::value> test(int);\
            template<typename D>static constexpr bool exist() { return std::is_void<decltype(test<D>(0))>::value; }                   \
        };                                                                                                                            \
        /* 上面是宏的续行符，这里不能有代码 */
        STUDY_OPENGL_DETECT(initialize)
        STUDY_OPENGL_DETECT(display)
        STUDY_OPENGL_DETECT(terminate)
#undef STUDY_OPENGL_DETECT
        
        template<typename D>static void initialize(...) {}
        template<typename D>static std::enable_if_t<initialize_t::template exist<D>()> initialize(self_t*self) { self->d.initialize(); }
        
        template<typename D>static void display(...) {}
        template<typename D>static std::enable_if_t<display_t::template exist<D>()> display(self_t*self) { self->d.display(); }
        
        template<typename D>static void terminate(...) {}
        template<typename D>static std::enable_if_t<terminate_t::template exist<D>()> terminate(self_t*self) { self->d.terminate(); }
    };
    
    static_assert(detect::initialize_t::template exist<Description>(), "");
    static_assert(detect::display_t   ::template exist<Description>(), "");
    //static_assert(detect::terminate ::template exist<Description>(), "");
    
    Description d;
public:
    study_opengl()
    {
        /* Initialize the library */
        if (!glfwInit())
            return;
        
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        
        /* Create a windowed mode window and its OpenGL context */
        GLFWwindow*theWindow = glfwCreateWindow(640, 480, "Hello World GLFW", NULL, NULL);
        if (!theWindow)
        {
            glfwTerminate();
            return;
        }
        
        /* Make the window's context current */
        glfwMakeContextCurrent(theWindow);
        
        glfwSetWindowUserPointer(theWindow, this);
        glfwSetWindowRefreshCallback(theWindow, [](GLFWwindow*theWindow){
            self_t*self = static_cast<self_t*>(glfwGetWindowUserPointer(theWindow));
            detect::template display<Description>(self);
            /* Swap front and back buffers */
            glfwSwapBuffers(theWindow);
        });
        
        glewExperimental = true;
        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
        }
        
        std::cout << "GLFW version                : " << glfwGetVersionString() << std::endl;
        std::cout << "GL_VERSION                  : " << glGetString( GL_VERSION ) << std::endl;
        std::cout << "GL_VENDOR                   : " << glGetString( GL_VENDOR ) << std::endl;
        std::cout << "GL_RENDERER                 : " << glGetString( GL_RENDERER ) << std::endl;
        std::cout << "GL_SHADING_LANGUAGE_VERSION : " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;
    }
    ~study_opengl()
    {
        std::atomic<bool> OK(true);
        
        auto f = std::async([&OK]() {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            OK = false;
        });
        detect::template initialize<Description>(this);
        GLFWwindow*theWindow = glfwGetCurrentContext();
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(theWindow))
        {
            if (!OK)
                glfwSetWindowShouldClose(theWindow, true);
            
            /* Poll for and process events */
            glfwPollEvents();
        }
        
        f.get();
        detect::template terminate<Description>(this);
        glfwTerminate();
    }
};

template<typename Description>class gpu_program
{
    typedef gpu_program self_t;
	struct detect
	{
#define GPU_PROGRAM_DETECT(vertex_shader, shaderType)                                                                               \
        struct vertex_shader                                                                                                        \
        {                                                                                                                           \
            template<typename D>static typename std::enable_if<!std::is_void<decltype(std::declval<D>().vertex_shader())>::value>::type test(int);\
            template<typename D>static int test(...);                                                                               \
            template<typename D>static constexpr bool exist() { return std::is_void<decltype(test<D>(0))>::value; }                 \
                                                                                                                                    \
            template<typename D>static void send_to_opengl_as_shader(...) { }                                                       \
            template<typename D>static typename std::enable_if <vertex_shader::exist<D>()>::type                                    \
                send_to_opengl_as_shader(self_t*self)                                                                               \
            {                                                                                                                       \
                GLuint shaderHandle = glCreateShader(shaderType);                                                                   \
                if (0 == shaderHandle)                                                                                              \
                {                                                                                                                   \
                    std::cerr << "Error creating " << #shaderType << " shader." << std::endl;                                       \
                    exit(1);                                                                                                        \
                }                                                                                                                   \
                                                                                                                                    \
                const GLchar* codeArray[] = { self->d.vertex_shader() };                                                            \
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
                glAttachShader(self->programHandle, shaderHandle);                                                                  \
            }                                                                                                                       \
        };                                                                                                                          \
		/* 上一行是宏的续行符，所以这一行不能有代码，但是注释却是可以的 */
		GPU_PROGRAM_DETECT(vertex_shader, GL_VERTEX_SHADER);
		GPU_PROGRAM_DETECT(fragment_shader, GL_FRAGMENT_SHADER);

#undef GPU_PROGRAM_DETECT

		static void send_to_opengl_as_shaders(self_t*self)
		{
			vertex_shader::template send_to_opengl_as_shader<Description>(self);
			fragment_shader::template send_to_opengl_as_shader<Description>(self);
		}
	};
	GLuint programHandle = 0;
    Description d;
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

		detect::send_to_opengl_as_shaders(this);

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
    typedef vertex_array_object self_t;
	struct detect
	{
#define VERTEX_ARRAY_OBJECT_DETECT(position)                                                                                \
		struct position                                                                                                     \
		{                                                                                                                   \
			template<typename D>static std::enable_if_t < !std::is_void<decltype(D::position##_location)>::value> test(int);\
			template<typename D>static int test(...);                                                                       \
			template<typename D>static constexpr bool exist() { return std::is_void<decltype(test<D>(0))>::value; }         \
                                                                                                                            \
			template<typename D>static std::enable_if_t <position::exist<D>()>                                              \
				send_to_opengl_as_buffer_object(GLuint bufferType/* GL_ARRAY_BUFFER etc. */, self_t*self)                   \
			{                                                                                                               \
                glBindBuffer(bufferType, self->vboHandles[D::position##_location]);                                         \
				self->d.position##_as_buffer_object();                                                                      \
			}                                                                                                               \
			template<typename D>static void send_to_opengl_as_buffer_object(...) { }                                        \
                                                                                                                            \
			template<typename D>static std::enable_if_t <position::exist<D>()>                                              \
				send_to_opengl_as_member_of_vertex_array_object(self_t*self)                                                \
			{                                                                                                               \
				glEnableVertexAttribArray(D::position##_location);                                                          \
                glBindBuffer(GL_ARRAY_BUFFER, self->vboHandles[D::position##_location]);                                    \
				self->d.position##_as_member_of_vertex_array_object();                                                      \
			}                                                                                                               \
			template<typename D>static void send_to_opengl_as_member_of_vertex_array_object(...) { }                        \
		}                                                                                                                   \
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
		static void send_to_opengl_as_buffer_objects(self_t*self)
		{
			// Create the buffer objects
			glGenBuffers(detect::N_vbo_handles(), self->vboHandles);
			position::template send_to_opengl_as_buffer_object<Description>(GL_ARRAY_BUFFER, self);
			color   ::template send_to_opengl_as_buffer_object<Description>(GL_ARRAY_BUFFER, self);
			normal  ::template send_to_opengl_as_buffer_object<Description>(GL_ARRAY_BUFFER, self);
			texture ::template send_to_opengl_as_buffer_object<Description>(GL_ARRAY_BUFFER, self);
			element ::template send_to_opengl_as_buffer_object<Description>(GL_ELEMENT_ARRAY_BUFFER, self);
		}
		static void send_to_opengl_as_vertex_array_object(self_t*self)
		{
			// Create the buffer objects
			glBindVertexArray(self->vaoHandle);
			position::template send_to_opengl_as_member_of_vertex_array_object<Description>(self);
			color   ::template send_to_opengl_as_member_of_vertex_array_object<Description>(self);
			normal  ::template send_to_opengl_as_member_of_vertex_array_object<Description>(self);
			texture ::template send_to_opengl_as_member_of_vertex_array_object<Description>(self);
			//element::template send_to_opengl_as_member_of_vertex_array_object<Description>(self);
		}

		template<typename D> static void _bind_element_array_buffer(...) {}
		template<typename D> static std::enable_if_t<element::template exist<D>()>
			_bind_element_array_buffer(self_t*self)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->vboHandles[D::element_location]);
		}

		template<typename D> static void display(...) {}
        template<typename D> static std::enable_if_t<std::is_void<decltype(std::declval<D>().display())>::value>
			display(self_t*self)
		{
			_bind_element_array_buffer<D>(self);
			self->d.display();
		}
        
        template<typename D> static void will_send_to_opengl(...){}
        template<typename D> static std::enable_if_t<std::is_void<decltype(std::declval<D>().will_send_to_opengl())>::value>
        will_send_to_opengl(self_t*self)
        {
            self->d.will_send_to_opengl();
        }
        
        template<typename D> static void did_send_to_opengl(...){}
        template<typename D> static std::enable_if_t<std::is_void<decltype(std::declval<D>().did_send_to_opengl())>::value>
        did_send_to_opengl(self_t*self)
        {
            self->d.did_send_to_opengl();
        }
	};
	GLuint vboHandles[detect::N_vbo_handles()] = {0};
	GLuint vaoHandle = 0;
    Description d;
public:
	GLuint gl_handle() { return vaoHandle; }
	vertex_array_object& send_to_opengl()
	{
        detect::template will_send_to_opengl<Description>(this);
		glGenVertexArrays(1, &vaoHandle);
		detect::send_to_opengl_as_buffer_objects(this);
		detect::send_to_opengl_as_vertex_array_object(this);
        detect::template did_send_to_opengl<Description>(this);
		return *this;
	}
	void display()
	{
		glBindVertexArray(vaoHandle);
		detect::template display<Description>(this);
	}
};

#endif /* opengl_hpp */
