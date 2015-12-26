#include <catch.hpp>
#include "opengl.hpp"
#include <array>

TEST_CASE("第一个例子", "[GLSL4BOOK]")
{
	struct Local
	{
		GLuint vaoHandle = 0;
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

TEST_CASE("第二个例子", "[GLSL4BOOK][active]")
{

}