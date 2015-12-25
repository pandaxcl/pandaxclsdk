#include <catch.hpp>
#include "opengl.hpp"

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
			GLfloat positionData[] = {
				-0.8f, -0.8f, 0.0f,
				0.8f, -0.8f, 0.0f,
				0.0f,  0.8f, 0.0f };
			GLfloat colorData[] = {
				1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f };
			// Create the buffer objects
			GLuint vboHandles[2];
			glGenBuffers(2, vboHandles);
			GLuint positionBufferHandle = vboHandles[0];
			GLuint colorBufferHandle = vboHandles[1];
			// Populate the position buffer 
			glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), positionData, GL_STATIC_DRAW);
			// Populate the color buffer 
			glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
			glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colorData, GL_STATIC_DRAW);

			// Create and set-up the vertex array object 
			glGenVertexArrays(1, &local->vaoHandle);
			glBindVertexArray(local->vaoHandle);
			// Enable the vertex attribute arrays 
			glEnableVertexAttribArray(0);
			// Vertex position 
			glEnableVertexAttribArray(1);
			// Vertex color
			// Map index 0 to the position buffer 
			glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
			// Map index 1 to the color buffer 
			glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
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