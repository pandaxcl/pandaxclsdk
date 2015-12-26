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

#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "glew32s.lib")

window::window()
{
	int argc = 0;
	char*argv[] = { "opengl" };
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	//glutInitContextVersion(4, 1);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Hello World");

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

	while (OK) {
		glutMainLoopEvent();
	}

	f.get();

	glutExit();
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
	glutSetWindowData(&o);
	glutDisplayFunc([]() {
		opengl*self = reinterpret_cast<opengl*>(glutGetWindowData());
		self->lambda_display();
		glutSwapBuffers();
	});
	return w;
}