//
//  model.cpp
//  study-opengl
//
//  Created by 熊 春雷 on 16/1/1.
//  Copyright © 2016年 pandaxcl. All rights reserved.
//
#include <GLFW/glfw3.h>
#include "model.hpp"
#include <glm.c>

namespace model
{
    void Rabbit::position_as_buffer_object()
    {
        GLfloat data[] = {
            -0.8f, -0.8f, 0.0f,
            +0.8f, -0.8f, 0.0f,
            +0.8f, +0.8f, 0.0f,
            -0.8f, +0.8f, 0.0f,
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 4, data, GL_STATIC_DRAW);
    }
    void Rabbit::position_as_member_of_vertex_array_object()
    {
        glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
    }
    
    void Rabbit::texture_as_buffer_object()
    {
        GLfloat data[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 4, data, GL_STATIC_DRAW);
    }
    void Rabbit::texture_as_member_of_vertex_array_object()
    {
        glVertexAttribPointer(texture_location, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
    }
    
    void Rabbit::normal_as_buffer_object()
    {
        
    }
    void Rabbit::normal_as_member_of_vertex_array_object()
    {
        
    }
    
    void Rabbit::element_as_buffer_object()
    {
        GLubyte data[] = {
            0,1,2,
            2,3,0,
        };
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 3 * 2, data, GL_STATIC_DRAW);
    }
    
    //static void element_as_member_of_vertex_array_object(GLuint vaoHandle, GLuint bufferHandle)
    //{
    //	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferHandle);
    //	//glVertexAttribPointer(texture_location, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
    //}
    
    void Rabbit::display()
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
    }
}