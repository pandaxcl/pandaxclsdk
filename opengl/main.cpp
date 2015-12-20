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
    for (int i=0; i<1; i++)
    {
        program P;
        P
        .shader(GL_VERTEX_SHADER, u8R"(
#version 400
in vec3 VertexPosition;
in vec3 VertexColor;
out vec3 Color;
void main()
{
    Color = VertexColor;
    gl_Position = vec4( VertexPosition, 1.0 );
}
                       )")
        .shader(GL_FRAGMENT_SHADER, u8R"(
#version 400
in vec3 Color;
out vec4 FragColor;
void main()
{
   //FragColor = vec4(Color, 1.0);
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
                       )");
        opengl(argc, argv)
        .initialize([&P](){
            P.link();
        })
        .display([&P](){
            glClear(GL_COLOR_BUFFER_BIT);
            P.use();
            glutWireTeapot(0.5);
            glutSwapBuffers();
        });
    }
    
    return 0;
}
