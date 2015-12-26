#include <catch.hpp>
#include "opengl.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

namespace Description
{
	struct Triangle
	{
		enum {
			position_location = 0,
			color_location = 1,
		};
		static void position_as_buffer_object(GLuint bufferHandle)
		{
			GLfloat data[] = {
				-0.8f, -0.8f, 0.0f,
				0.8f, -0.8f, 0.0f,
				0.0f,  0.8f, 0.0f };
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), data, GL_STATIC_DRAW);
		}
		static void position_as_member_of_vertex_array_object(GLuint vaoHandle, GLuint bufferHandle)
		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
		}

		static void color_as_buffer_object(GLuint bufferHandle)
		{
			GLfloat data[] = {
				1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f };
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), data, GL_STATIC_DRAW);
		}
		static void color_as_member_of_vertex_array_object(GLuint vaoHandle, GLuint bufferHandle)
		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
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
		static void position_as_buffer_object(GLuint bufferHandle)
		{
			GLfloat data[] = {
				-0.8f, -0.8f, 0.0f,
				+0.8f, -0.8f, 0.0f,
				+0.8f, +0.8f, 0.0f,
				-0.8f, +0.8f, 0.0f,
			};
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 4, data, GL_STATIC_DRAW);
		}
		static void position_as_member_of_vertex_array_object(GLuint vaoHandle, GLuint bufferHandle)
		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
			glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
		}

		static void texture_as_buffer_object(GLuint bufferHandle)
		{
			GLfloat data[] = {
				0.0f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f,
				0.0f, 1.0f,
			};
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 4, data, GL_STATIC_DRAW);
		}
		static void texture_as_member_of_vertex_array_object(GLuint vaoHandle, GLuint bufferHandle)
		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
			glVertexAttribPointer(texture_location, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
		}

		static void element_as_buffer_object(GLuint bufferHandle)
		{
			GLubyte data[] = {
				0,1,2,
				2,3,0,
			};
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferHandle);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 3 * 2, data, GL_STATIC_DRAW);
		}

		//static void element_as_member_of_vertex_array_object(GLuint vaoHandle, GLuint bufferHandle)
		//{
		//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferHandle);
		//	//glVertexAttribPointer(texture_location, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
		//}

		static void display()
		{
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
		}
	};
}// namespace Description

TEST_CASE("第一个例子", "[GLSL4BOOK]")
{
	struct Local
	{
		vertex_array_object<Description::Triangle> vao;
		GLuint programHandle = 0;
	};
	auto local = std::make_shared<Local>();

	opengl render;
	render
		.initialize([local, &render]() {
			{
				struct Description
				{
					static GLchar*vertex_shader()
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
					static GLchar*fragment_shader()
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
				local->programHandle = gpu_program<Description>().send_to_opengl().report().gl_handle();
			}
			{
				local->vao.send_to_opengl();
			}
	})
		.display([local]() {
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(local->programHandle);
		local->vao.display();
	});

	window() << render;
}

TEST_CASE("通过uniform变量传递数据到顶点着色器", "[GLSL4BOOK]")
{
	struct Local
	{
		vertex_array_object<Description::Triangle> vao;
		GLuint programHandle = 0;
	};
	auto local = std::make_shared<Local>();

	opengl render;
	render
		.initialize([local, &render]() {
			{
				struct Description
				{
					static GLchar*vertex_shader()
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
					static GLchar*fragment_shader()
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
				local->programHandle = gpu_program<Description>().send_to_opengl().report().gl_handle();
			}
			{
				local->vao.send_to_opengl();
			}
	})
		.display([local]() {
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(local->programHandle);
		
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0, 0, 1));
		GLuint location = glGetUniformLocation(local->programHandle, "RotationMatrix");
		assert(location >= 0);
		glUniformMatrix4fv(location, 1, GL_FALSE, &rotationMatrix[0][0]);
		
		local->vao.display();
	});

	window() << render;
}

TEST_CASE("通过uniform块变量传递数据到片元着色器", "[GLSL4BOOK][active]")
{
	struct Local
	{
		vertex_array_object<Description::Rectangle> vao;
		GLuint programHandle = 0;
	};
	auto local = std::make_shared<Local>();

	opengl render;
	render
		.initialize([local, &render]() {
			{
				struct Description
				{
					static GLchar*vertex_shader()
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
					static GLchar*fragment_shader()
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
				local->programHandle = gpu_program<Description>().send_to_opengl().report().gl_handle();
			}
			{
				local->vao.send_to_opengl();
			}
	})
		.display([local]() {
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(local->programHandle);

		{
			GLuint programHandle = local->programHandle;
			GLuint blockIndex = glGetUniformBlockIndex(programHandle, "BlobSettings");
			GLint blockSize;
			glGetActiveUniformBlockiv(programHandle, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
			// Query for the offsets of each block variable 
			const GLchar *names[] = {
				"InnerColor",
				"OuterColor",
				"RadiusInner",
				"RadiusOuter"
			};
			GLuint indices[4];
			glGetUniformIndices(programHandle, 4, names, indices);
			GLint offset[4];
			glGetActiveUniformsiv(programHandle, 4, indices, GL_UNIFORM_OFFSET, offset);

			GLubyte * blockBuffer = (GLubyte *)malloc(blockSize);
			GLfloat outerColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			GLfloat innerColor[] = { 1.0f, 1.0f, 0.75f, 1.0f };
			GLfloat innerRadius = 0.25f;
			GLfloat outerRadius = 0.45f;
			memcpy(blockBuffer + offset[0], innerColor, 4 * sizeof(GLfloat));
			memcpy(blockBuffer + offset[1], outerColor, 4 * sizeof(GLfloat));
			memcpy(blockBuffer + offset[2], &innerRadius, sizeof(GLfloat));
			memcpy(blockBuffer + offset[3], &outerRadius, sizeof(GLfloat));

			GLuint uboHandle;
			glGenBuffers(1, &uboHandle);
			glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
			glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);

			free(blockBuffer);

			glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uboHandle);
		}
		local->vao.display();
	});

	window() << render;
}
