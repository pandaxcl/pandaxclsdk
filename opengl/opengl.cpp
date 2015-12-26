//
//  opengl.cpp
//  study-opengl
//
//  Created by 熊 春雷 on 15/12/20.
//  Copyright © 2015年 pandaxcl. All rights reserved.
//

#include <iostream>
#include <future>

#include "opengl.hpp"

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

window::window()
{
	/* Initialize the library */
	if (!glfwInit())
		return;

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    
	/* Create a windowed mode window and its OpenGL context */
	theWindow = glfwCreateWindow(640, 480, "Hello World GLFW", NULL, NULL);
	if (!theWindow)
	{
		glfwTerminate();
		return;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(theWindow);

    glewExperimental = true;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
	}

	printf("GL_VERSION = %s\n", glGetString(GL_VERSION));
}

window::~window()
{
	std::atomic<bool> OK(true);

	auto f = std::async([&OK]() {
		std::this_thread::sleep_for(std::chrono::seconds(10));
		OK = false;
	});

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(theWindow))
	{
        if (!OK)
            glfwSetWindowShouldClose(theWindow, true);
        
        std::cout<<"-=--=-=--"<<std::endl;
		/* Render here */
		this->lambda_display();

		/* Swap front and back buffers */
		glfwSwapBuffers(theWindow);

		/* Poll for and process events */
		glfwPollEvents();
	}

	f.get();

	glfwTerminate();
}

opengl::opengl()
{
	
}

opengl::~opengl()
{

}

opengl&opengl::display(std::function<void()>&&f)
{
    this->lambda_display = std::move(f);
    return *this;
}

opengl&opengl::initialize(std::function<void()>&&f)
{
    this->lambda_initialize = std::move(f);
    return *this;
}

#pragma mark -

window&operator<<(window&&w, opengl&o)
{
	o.lambda_initialize();
	w.lambda_display = [&o]() {
		o.lambda_display();
	};
	return w;
}