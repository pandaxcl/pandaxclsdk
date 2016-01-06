#include <catch.hpp>
#include "opengl.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include "model.hpp"

namespace block
{
    struct uniform_description
    {
    //protected:
        GLuint      indices[4] = {0};
        GLint       offset [4] = {0};
        GLuint      blockIndex = -1;
        GLint       blockSize  = 0;
        GLuint      uboHandle  = -1;
    };
    template<typename Block>struct uniform:public Block
    {
        static_assert(std::is_base_of<uniform_description, Block>::value, "");
        static_assert(!std::is_void<decltype(Block::N_fields)>::value, "");
        static_assert(!std::is_void<decltype(Block::name)>::value, "");
        static_assert(!std::is_void<decltype(Block::names)>::value, "");
        static_assert( std::is_void<decltype(std::declval<Block>().initialize())>::value, "");
        static_assert( std::is_void<decltype(std::declval<Block>().update())>::value, "");
        

        void send_to_opengl(GLuint programHandle)
        {
            uniform_description::blockIndex = glGetUniformBlockIndex(programHandle, Block::name);
            glGetActiveUniformBlockiv(programHandle, uniform_description::blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &this->uniform_description::blockSize);
            glGetUniformIndices(programHandle, Block::N_fields, Block::names, uniform_description::indices);
            glGetActiveUniformsiv(programHandle, Block::N_fields, uniform_description::indices, GL_UNIFORM_OFFSET, uniform_description::offset);
            
            glGenBuffers(1, &this->uniform_description::uboHandle);
            glBindBuffer(GL_UNIFORM_BUFFER, uniform_description::uboHandle);

            Block::initialize();
            
            glBindBufferBase(GL_UNIFORM_BUFFER, uniform_description::blockIndex, uniform_description::uboHandle);
        }
//        void initialize()
//        {
//            
//        }
//        void update()
//        {
//            GLubyte * blockBuffer = (GLubyte *)malloc(blockSize);
//            GLfloat outerColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
//            GLfloat innerColor[] = { 1.0f, 1.0f, 0.75f, 1.0f };
//            GLfloat innerRadius = 0.25f;
//            GLfloat outerRadius = 0.45f;
//            memcpy(blockBuffer + offset[0], innerColor, 4 * sizeof(GLfloat));
//            memcpy(blockBuffer + offset[1], outerColor, 4 * sizeof(GLfloat));
//            memcpy(blockBuffer + offset[2], &innerRadius, sizeof(GLfloat));
//            memcpy(blockBuffer + offset[3], &outerRadius, sizeof(GLfloat));
//            
//            glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
//            glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
//            
//            free(blockBuffer);
//        }
    };
}

TEST_CASE(u8"第一个例子 ", "[GLSL4BOOK]")
{
    struct Content
    {
        vertex_array_object<model::Triangle> vao;
        GLuint programHandle = 0;
        void initialize()
        {
            {
                struct Description
                {
                    static const GLchar*vertex_shader()
                    {
                        return u8R"(
#version 400
                        in vec3 VertexPosition;
                        in vec3 VertexColor;
                        out vec3 Color;
                        void main()
                        {
                            Color = VertexColor;
                            gl_Position = vec4( VertexPosition, 1.0 );
                        }
                        )";
                    }
                    static const GLchar*fragment_shader()
                    {
                        return u8R"(
#version 400
                        in vec3 Color;
                        out vec4 FragColor;
                        void main()
                        {
                            FragColor = vec4(Color, 1.0);
                        }
                        )";
                    }
                };
                this->programHandle = gpu_program<Description>().send_to_opengl().report().gl_handle();
            }
            {
                this->vao.send_to_opengl();
            }
        }
        void display()
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(this->programHandle);
            this->vao.display();
        }
    };
    
    study_opengl<Content>();
}

TEST_CASE(u8"通过uniform变量传递数据到顶点着色器", "[GLSL4BOOK]")
{
    struct Content
    {
        vertex_array_object<model::Triangle> vao;
        GLuint programHandle = 0;
        
        void initialize()
        {
            {
                struct Description
                {
                    static const GLchar*vertex_shader()
                    {
                        return u8R"(
#version 400
                        layout (location = 0) in vec3 VertexPosition;
                        layout (location = 1) in vec3 VertexColor;
                        out vec3 Color;
                        uniform mat4 RotationMatrix;
                        void main()
                        {
                            Color = VertexColor;
                            gl_Position = RotationMatrix * vec4( VertexPosition, 1.0 );
                        }
                        )";
                    }
                    static const GLchar*fragment_shader()
                    {
                        return u8R"(
#version 400
                        in vec3 Color;
                        layout (location = 0) out vec4 FragColor;
                        void main()
                        {
                            FragColor = vec4(Color, 1.0);
                        }
                        )";
                    }
                };
                this->programHandle = gpu_program<Description>().send_to_opengl().report().gl_handle();
            }
            {
                this->vao.send_to_opengl();
            }
        }
        
        void display()
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(this->programHandle);
            
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0, 0, 1));
            GLuint location = glGetUniformLocation(this->programHandle, "RotationMatrix");
            assert(location >= 0);
            glUniformMatrix4fv(location, 1, GL_FALSE, &rotationMatrix[0][0]);
            
            this->vao.display();
        }
    };
    
    study_opengl<Content>();
}

TEST_CASE(u8"通过uniform块变量传递数据到片元着色器", "[GLSL4BOOK]")
{
    struct BlobSettings:block::uniform_description
    {
        enum{  N_fields = 4 };
        const GLchar*name = "BlobSettings";
        const GLchar *names[N_fields] = {
            "InnerColor",
            "OuterColor",
            "RadiusInner",
            "RadiusOuter",
        };
        void initialize()
        {

        }
        void update()
        {
            GLubyte * blockBuffer = (GLubyte *)malloc(blockSize);
            GLfloat outerColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            GLfloat innerColor[] = { 1.0f, 1.0f, 0.75f, 1.0f };
            GLfloat innerRadius = 0.25f;
            GLfloat outerRadius = 0.45f;
            memcpy(blockBuffer + offset[0], innerColor, 4 * sizeof(GLfloat));
            memcpy(blockBuffer + offset[1], outerColor, 4 * sizeof(GLfloat));
            memcpy(blockBuffer + offset[2], &innerRadius, sizeof(GLfloat));
            memcpy(blockBuffer + offset[3], &outerRadius, sizeof(GLfloat));

            glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
            glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
            
            free(blockBuffer);
        }
    };
    
    struct Content
    {
        vertex_array_object<model::Rectangle> vao;
        block::uniform<BlobSettings> blobSettings;
        GLuint programHandle = 0;
        void initialize()
        {
            {
                struct Description
                {
                    static const GLchar*vertex_shader()
                    {
                        return u8R"(
#version 400
                        layout (location = 0) in vec3 VertexPosition;
                        layout (location = 1) in vec3 VertexTexCoord;
                        out vec3 TexCoord;
                        void main()
                        {
                            TexCoord = VertexTexCoord;
                            gl_Position = vec4(VertexPosition,1.0);
                        }
                        )";
                    }
                    static const GLchar*fragment_shader()
                    {
                        return u8R"(
#version 400
                        in vec3 TexCoord;
                        layout (location = 0) out vec4 FragColor;
                        uniform BlobSettings
                        {
                            vec4 InnerColor;
                            vec4 OuterColor;
                            float RadiusInner;
                            float RadiusOuter;
                        };
                        void main()
                        {
                            float dx = TexCoord.x - 0.5;
                            float dy = TexCoord.y - 0.5;
                            float dist = sqrt(dx * dx + dy * dy);
                            FragColor = mix( InnerColor, OuterColor, smoothstep( RadiusInner, RadiusOuter, dist ) );
                        }
                        )";
                    }
                };
                this->programHandle = gpu_program<Description>().send_to_opengl().report().gl_handle();
            }
            {
                this->vao.send_to_opengl();
            }
            {
                this->blobSettings.send_to_opengl(this->programHandle);
            }
        }
        void display()
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(this->programHandle);
            this->blobSettings.update();
            this->vao.display();
        }
    };
    
    study_opengl<Content>();
}

TEST_CASE(u8"绘制兔子模型", "[objmesh][rabbit][active]")
{
    struct Content
    {
        GLuint programHandle = 0;
        vertex_array_object<model::Rabbit> vao;
        void initialize()
        {
            {
                struct Description
                {
                    const GLchar*vertex_shader()
                    {
                        return u8R"(
#version 400
                        layout (location = 0) in vec3 VertexPosition;
                        layout (location = 1) in vec3 VertexNormal;
                        
                        out vec3 LightIntensity;
                        
                        uniform vec4 LightPosition;
                        uniform vec3 Kd;
                        uniform vec3 Ld;
                        
                        uniform mat4 ModelViewMatrix;
                        uniform mat3 NormalMatrix;
                        uniform mat4 ProjectMatrix;
                        uniform mat4 MVP; // Projection * ModelView
                        void main()
                        {
                            vec3 tnorm = normalize (NormalMatrix * VertexNormal);
                            vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition, 1.0);
                            vec3 s = normalize(vec3(LightPosition - eyeCoords));
                            
                            LightIntensity = Ld*Kd*max(dot(s, tnorm), 0.0);
                            
                            gl_Position = vec4(VertexPosition, 1);
                        }
                        )";
                    }
                    const GLchar*fragment_shader()
                    {
                        return u8R"(
#version 400
                        in vec3 LightIntensity;
                        layout (location = 0) out vec4 FragColor;
                        void main()
                        {
                            FragColor = vec4(LightIntensity, 1.0);
                        }
                        )";
                    }
                };
                this->programHandle = gpu_program<Description>().send_to_opengl().report().gl_handle();
            }
            {
                this->vao.send_to_opengl();
            }
        }
        
        void display()
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(this->programHandle);
            {// uniform vec4 LightPosition;
                GLfloat v[] = {0.0f, 0.0f, 10.0f, 1.0f};
                GLint location = glGetUniformLocation(this->programHandle, "LightPosition");
                glUniform4fv(location, 1, v);
            }

            {// uniform vec3 Kd;
                GLfloat v[] = {1.0f, 1.0f, 1.0f};
                GLint location = glGetUniformLocation(this->programHandle, "Kd");
                glUniform3fv(location, 1, v);
            }
            {// uniform vec3 Ld;
                GLfloat v[] = {1.0f, 1.0f, 1.0f};
                GLint location = glGetUniformLocation(this->programHandle, "Ld");
                glUniform3fv(location, 1, v);
            }
            
            {// uniform mat4 ModelViewMatrix;
                auto v = glm::mat4(1.0f);
                GLint location = glGetUniformLocation(this->programHandle, "ModelViewMatrix");
                glUniformMatrix4fv(location, 1, GL_FALSE, &v[0][0]);
            }
            {// uniform mat3 NormalMatrix;
                auto v = glm::mat3(1.0f);
                GLint location = glGetUniformLocation(this->programHandle, "NormalMatrix");
                glUniformMatrix3fv(location, 1, GL_FALSE, &v[0][0]);
            }
            {// uniform mat4 ProjectMatrix;
                auto v = glm::perspective(glm::degrees(60.0f), 4.0f/3.0f, 0.01f, 100.f);
                GLint location = glGetUniformLocation(this->programHandle, "ProjectMatrix");
                glUniformMatrix4fv(location, 1, GL_FALSE, &v[0][0]);
            }
            {// uniform mat4 MVP;
                auto v = glm::mat4(1.0f);
                GLint location = glGetUniformLocation(this->programHandle, "MVP");
                glUniformMatrix4fv(location, 1, GL_FALSE, &v[0][0]);
            }

            this->vao.display();
        }
    };
    
    study_opengl<Content>();
}