//
//  main.cpp
//  study-opengl
//
//  Created by 熊 春雷 on 15/11/1.
//  Copyright © 2015年 pandaxcl. All rights reserved.
//

#include "opengl.hpp"


int main(int argc, const char * argv[]) {
    // insert code here...
    for (int i=0; i<300; i++)
    {
        opengl(argc, argv)
        .display([](opengl*self){
            glClear(GL_COLOR_BUFFER_BIT);
            glutWireTeapot(0.5);
            glutSwapBuffers();
        });
    }
    
    return 0;
}
