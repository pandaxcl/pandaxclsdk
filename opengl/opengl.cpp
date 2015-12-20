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

opengl::opengl(int argc, const char * argv[])
{
    glutInit(&argc, (char**)argv);
}

opengl::~opengl()
{
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    
    glutInitContextVersion(4, 3);
    glutInitWindowSize(512, 512);
    this->_window = glutCreateWindow("Hello World");
    glutSetWindowData(this);
    
    glewInit();
    
    printf("GL_VERSION = %s\n", glGetString(GL_VERSION)) ;
    glutDisplayFunc([](){
        opengl*self = reinterpret_cast<opengl*>(glutGetWindowData());
        self->lambda_display(self);
    });
    
    std::atomic<bool> OK(true);
    
    auto f = std::async([&OK](){
        std::this_thread::sleep_for(std::chrono::microseconds(10));
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

opengl&opengl::display(std::function<void(opengl*self)>&&f)
{
    this->lambda_display = std::move(f);
    return *this;
}
