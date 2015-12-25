#include <catch.hpp>
#include "opengl.hpp"
#include <array>

TEST_CASE("GLSL4BOOK")
{
	SECTION("第一个例子")
	{
		struct Local
		{
			GLuint vaoHandle = 0;
			GLuint programHandle = 0;
		};
		auto local = std::make_shared<Local>();
		program gpu; 
		gpu
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
    FragColor = vec4(Color, 1.0);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
                       )")
			.on_stream_out([local,&gpu]() {
			local->programHandle = gpu.gl_program();
		});


		opengl render;
		render
			.initialize([local,&render,&gpu]() {
			render << gpu;
			{
				struct Description
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
				};
				local->vaoHandle = vertex_array_object<Description>().send_to_opengl().gl_handle();
			}
		})
			.display([local]() {
			glClear(GL_COLOR_BUFFER_BIT);
			glUseProgram(local->programHandle);
			glBindVertexArray(local->vaoHandle);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		});

		window() << render;
	}
}