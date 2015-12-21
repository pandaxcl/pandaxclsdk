//
//  main.cpp
//  study-opengl
//
//  Created by 熊 春雷 on 15/11/1.
//  Copyright © 2015年 pandaxcl. All rights reserved.
//

#include "opengl.hpp"
#include <memory>

int main(int argc, const char * argv[]) {
    // insert code here...
    for (int i=0; i<1; i++)
    {
		struct Local
		{
			GLuint vaoHandle = 0;
			program P;
		};
		auto local = std::make_shared<Local>();
        local->P
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
                       )");

		
        opengl(argc, argv)
        .initialize([local](){
            local->P.link();

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
			glGenVertexArrays( 1, &local->vaoHandle );
			glBindVertexArray(local->vaoHandle);
			// Enable the vertex attribute arrays 
			glEnableVertexAttribArray(0); 
			// Vertex position 
			glEnableVertexAttribArray(1);  
			// Vertex color
			// Map index 0 to the position buffer 
			glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL ); 
			// Map index 1 to the color buffer 
			glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle); 
			glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );
        })
        .display([local](){
            glClear(GL_COLOR_BUFFER_BIT);
			local->P.use();
            //glutWireTeapot(0.5);
			glBindVertexArray(local->vaoHandle);
			glDrawArrays(GL_TRIANGLES, 0, 3);
            glutSwapBuffers();
        });
    }
    
    return 0;
}
