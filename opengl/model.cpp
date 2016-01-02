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
    void obj_model::will_send_to_opengl()
    {
        model = glmReadOBJ("/Users/pandaxcl/Downloads/f7a816806b2db955e28628ec1aacd0cc/028.obj");
        glmUnitize(model);
        this->numOfTriangles = model->numtriangles;
    }
    void obj_model::did_send_to_opengl()
    {
        glmDelete(model);
        model = nullptr;
    }
    void obj_model::position_as_buffer_object()
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 3 * model->numtriangles, nullptr, GL_STATIC_DRAW);
        
        GLfloat*data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        {
            GLMgroup*group = model->groups;
            while (group)
            {
                for (int i = 0; i < group->numtriangles; i++)
                {
                    GLMtriangle*triangle = &T(group->triangles[i]);
                    *data = model->vertices[3 * triangle->vindices[0] + 0]; data++;
                    *data = model->vertices[3 * triangle->vindices[0] + 1]; data++;
                    *data = model->vertices[3 * triangle->vindices[0] + 2]; data++;
                    
                    *data = model->vertices[3 * triangle->vindices[1] + 0]; data++;
                    *data = model->vertices[3 * triangle->vindices[1] + 1]; data++;
                    *data = model->vertices[3 * triangle->vindices[1] + 2]; data++;
                    
                    *data = model->vertices[3 * triangle->vindices[2] + 0]; data++;
                    *data = model->vertices[3 * triangle->vindices[2] + 1]; data++;
                    *data = model->vertices[3 * triangle->vindices[2] + 2]; data++;
                }
                
                group = group->next;
            }
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    void obj_model::position_as_member_of_vertex_array_object()
    {
        glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
    }
    
    void obj_model::texture_as_buffer_object()
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 3 * model->numtriangles, nullptr, GL_STATIC_DRAW);
        
        GLfloat*data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        {
            GLMgroup*group = model->groups;
            while (group)
            {
                for (int i = 0; i < group->numtriangles; i++)
                {
                    GLMtriangle*triangle = &T(group->triangles[i]);
                    *data = model->texcoords[2 * triangle->tindices[0] + 0]; data++;
                    *data = model->texcoords[2 * triangle->tindices[0] + 1]; data++;
                    
                    *data = model->texcoords[2 * triangle->tindices[1] + 0]; data++;
                    *data = model->texcoords[2 * triangle->tindices[1] + 1]; data++;
                    
                    *data = model->texcoords[2 * triangle->tindices[2] + 0]; data++;
                    *data = model->texcoords[2 * triangle->tindices[2] + 1]; data++;
                }
                
                group = group->next;
            }
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    void obj_model::texture_as_member_of_vertex_array_object()
    {
        glVertexAttribPointer(texture_location, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
    }
    
    void obj_model::normal_as_buffer_object()
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 3 * model->numtriangles, nullptr, GL_STATIC_DRAW);
        
        GLfloat*data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        {
            GLMgroup*group = model->groups;
            while (group)
            {
                for (int i = 0; i < group->numtriangles; i++)
                {
                    GLMtriangle*triangle = &T(group->triangles[i]);
                    *data = model->normals[3 * triangle->nindices[0] + 0]; data++;
                    *data = model->normals[3 * triangle->nindices[0] + 1]; data++;
                    *data = model->normals[3 * triangle->nindices[0] + 2]; data++;
                    
                    *data = model->normals[3 * triangle->nindices[1] + 0]; data++;
                    *data = model->normals[3 * triangle->nindices[1] + 1]; data++;
                    *data = model->normals[3 * triangle->nindices[1] + 2]; data++;
                    
                    *data = model->normals[3 * triangle->nindices[2] + 0]; data++;
                    *data = model->normals[3 * triangle->nindices[2] + 1]; data++;
                    *data = model->normals[3 * triangle->nindices[2] + 2]; data++;
                }
                
                group = group->next;
            }
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    void obj_model::normal_as_member_of_vertex_array_object()
    {
        glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
    }
    
    void obj_model::display()
    {
        glDrawArrays(GL_TRIANGLES, 0, 3 * this->numOfTriangles);
    }
}