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

class opengl
{
public:
    opengl(int argc, const char * argv[]);
    ~opengl();
    opengl&display(std::function<void(opengl*self)>&&);
    
public:
    void swap_buffers();
private:
    int _window = 0;
    std::function<void(opengl*self)> lambda_display;
};

#endif /* opengl_hpp */
