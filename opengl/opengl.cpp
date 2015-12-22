//
//  opengl.cpp
//  study-opengl
//
//  Created by 熊 春雷 on 15/12/20.
//  Copyright © 2015年 pandaxcl. All rights reserved.
//

#include <iostream>
#include <future>

#include "opengl.hpp"

#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "glew32s.lib")

opengl::opengl(int argc, const char * argv[])
{
    glutInit(&argc, (char**)argv);
}

opengl::~opengl()
{
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    
//    glutInitContextVersion(4, 1);
//    glutInitContextProfile(GLUT_CORE_PROFILE);
//    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    
    glutInitWindowSize(512, 512);
    this->_window = glutCreateWindow("Hello World");
    glutSetWindowData(this);
    
    GLenum err = glewInit();
    if( GLEW_OK != err )
    {
        std::cerr<<"Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
    }
    
    {
        const GLubyte *renderer = glGetString( GL_RENDERER );
        const GLubyte *vendor = glGetString( GL_VENDOR );
        const GLubyte *version = glGetString( GL_VERSION );
        const GLubyte *glslVersion =
        glGetString( GL_SHADING_LANGUAGE_VERSION );
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        printf("GL Vendor    : %s\n", vendor);
        printf("GL Renderer  : %s\n", renderer);
        printf("GL Version (string)  : %s\n", version);
        printf("GL Version (integer) : %d.%d\n", major, minor);
        printf("GLSL Version : %s\n", glslVersion);
    }
    
    this->lambda_initialize();
    glutDisplayFunc([](){
        opengl*self = reinterpret_cast<opengl*>(glutGetWindowData());
        self->lambda_display();
    });
    
    std::atomic<bool> OK(true);
    
    auto f = std::async([&OK](){
        std::this_thread::sleep_for(std::chrono::seconds(10));
        OK = false;
    });
    
    while (OK) {
        glutMainLoopEvent();
    }
    
    f.get();
    
    glutExit();
}

void opengl::swap_buffers()
{
    glutSwapBuffers();
}

opengl&opengl::display(std::function<void()>&&f)
{
    this->lambda_display = std::move(f);
    return *this;
}

opengl&opengl::initialize(std::function<void()>&&f)
{
    this->lambda_initialize = std::move(f);
    return *this;
}


#pragma mark -

void shader::__move__(shader&&o)
{
    _source    = std::move(o._source);
    _kind      = std::move(o._kind);
    _theShader = std::move(o._theShader);
    _log       = std::move(o._log);
}

void shader::__copy__(const shader&o)
{
    _source    = o._source;
    _kind      = o._kind;
    _theShader = o._theShader;
    _log       = o._log;
}

shader::shader(GLint kind, const std::string&source)
:_kind(kind), _source(source)
{

}

shader&shader::compile()
{
    GLuint theShader = glCreateShader( _kind );
    if( 0 == theShader )
    {
        std::cerr << "Error creating " << kind_name() << " shader." << std::endl;
        exit(1);
    }
    
    const GLchar* codeArray[] = { _source.c_str() };
    glShaderSource( theShader, 1, codeArray, NULL );
    
    glCompileShader( theShader );
    
    
    GLint result = GL_FALSE;
    glGetShaderiv( theShader, GL_COMPILE_STATUS, &result );
    if( GL_FALSE == result )
    {
        std::cerr << kind_name() << " compilation failed!" << std::endl;
        GLint logLen = 0;
        glGetShaderiv( theShader, GL_INFO_LOG_LENGTH, &logLen );
        if( logLen > 0 )
        {
            _log.resize(logLen);
            GLsizei written;
            glGetShaderInfoLog(theShader, logLen, &written, &_log[0]);
            std::cerr << "Shader log:" << std::endl << _log << std::endl;
        }
    }
    
    _theShader = theShader;
    return *this;
}

std::string shader::kind_name()
{
    switch (_kind)
    {
        case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER"; break;
        case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER"; break;
        case GL_TESS_CONTROL_SHADER: return "GL_TESS_CONTROL_SHADER"; break;
        case GL_TESS_EVALUATION_SHADER: return "GL_TESS_EVALUATION_SHADER"; break;
            
        default:
            break;
    }
    return "Unknown";
}


#pragma mark -

program&program::shader(GLint kind, const std::string&source)
{
    return shader(shader_t(kind, source));
}

program&program::shader(shader_t&&o)
{
    _shaders.push_back(std::move(o));
    return *this;
}

program&program::link()
{
    GLuint programHandle = glCreateProgram();
    if( 0 == programHandle )
    {
        std::cerr<< "Error creating program object." << std::endl;
        exit(1);
    }
    
    for (auto&shader:_shaders)
    {
        shader.compile();
        glAttachShader( programHandle, shader.gl_shader() );
    }
    
    glLinkProgram( programHandle );
    
    GLint status = GL_FALSE;
    glGetProgramiv( programHandle, GL_LINK_STATUS, &status );
    if( GL_FALSE == status )
    {
        std::cerr<< "Failed to link shader program!" << std::endl;
        GLint logLen;
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLen);
        if( logLen > 0 )
        {
            _log.resize(logLen);
            GLsizei written = 0;
            glGetProgramInfoLog(programHandle, logLen, &written, &_log[0]);
            std::cerr << "Program log:" << std::endl << _log << std::endl;
        }
    }
    _theProgram = programHandle;
    
    return *this;
}

void program::use()
{
    glUseProgram(_theProgram);
}
