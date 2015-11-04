//
//  main.cpp
//  study-opengl
//
//  Created by 熊 春雷 on 15/11/1.
//  Copyright © 2015年 pandaxcl. All rights reserved.
//


#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

void display();

int main(int argc, const char * argv[]) {
    // insert code here...

    glutInit(&argc, (char**)argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    
    glutInitContextVersion(4, 3);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Hello World");
    
    glewInit();
    printf("GL_VERSION = %s\n", glGetString(GL_VERSION)) ;
    glutDisplayFunc(display);
    glutMainLoop();
    
    return 0;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glutWireTeapot(0.5);
    
    glutSwapBuffers();
}
