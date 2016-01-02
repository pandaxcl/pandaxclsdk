//
//  model.hpp
//  study-opengl
//
//  Created by 熊 春雷 on 16/1/1.
//  Copyright © 2016年 pandaxcl. All rights reserved.
//

#ifndef model_h
#define model_h

#include "glm.h"

namespace model
{
    struct Triangle
    {
        enum {
            position_location = 0,
            color_location = 1,
        };
        static void position_as_buffer_object()
        {
            GLfloat data[] = {
                -0.8f, -0.8f, 0.0f,
                0.8f, -0.8f, 0.0f,
                0.0f,  0.8f, 0.0f };
            glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), data, GL_STATIC_DRAW);
        }
        static void position_as_member_of_vertex_array_object()
        {
            glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
        }
        
        static void color_as_buffer_object()
        {
            GLfloat data[] = {
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f };
            glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), data, GL_STATIC_DRAW);
        }
        static void color_as_member_of_vertex_array_object()
        {
            glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
        }
        static void display()
        {
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    };
    
    struct Rectangle
    {
        enum {
            position_location = 0,
            texture_location = 1,
            element_location = 2,
        };
        void position_as_buffer_object()
        {
            GLfloat data[] = {
                -0.8f, -0.8f, 0.0f,
                +0.8f, -0.8f, 0.0f,
                +0.8f, +0.8f, 0.0f,
                -0.8f, +0.8f, 0.0f,
            };
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 4, data, GL_STATIC_DRAW);
        }
        void position_as_member_of_vertex_array_object()
        {
            glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
        }
        
        void texture_as_buffer_object()
        {
            GLfloat data[] = {
                0.0f, 0.0f,
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 1.0f,
            };
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 4, data, GL_STATIC_DRAW);
        }
        void texture_as_member_of_vertex_array_object()
        {
            glVertexAttribPointer(texture_location, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
        }
        
        void element_as_buffer_object()
        {
            GLubyte data[] = {
                0,1,2,
                2,3,0,
            };
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 3 * 2, data, GL_STATIC_DRAW);
        }

        void display()
        {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        }
    };
    
    struct obj_model
    {
        enum {
            position_location = 0,
            normal_location   = 1,
            texture_location  = 2,
        };
        void position_as_buffer_object();
        void position_as_member_of_vertex_array_object();
        
        void texture_as_buffer_object();
        void texture_as_member_of_vertex_array_object();
        
        void normal_as_buffer_object();
        void normal_as_member_of_vertex_array_object();
        
        void display();
        
        void will_send_to_opengl();
        void did_send_to_opengl();
    private:
        GLMmodel*model = nullptr;
        GLuint numOfTriangles = 0;
    };
}// namespace Description

#endif /* model_h */
