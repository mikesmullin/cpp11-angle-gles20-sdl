#define GL_GLEXT_PROTOTYPES 1

//#include "EGL/egl.h"
//#include "GLES2/gl2.h"
//#include "shader_utils.h"
#include "SDL.h"
#include <assert.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "glad/glad.h"
#include "KHR/khrplatform.h"

using std::chrono::duration;

#define CHECK_GL_ERROR \
	while ((error = glGetError()) != 0) { \
		printf("GL Error %x: %s:%d\n", error, __FILE__, __LINE__); \
		assert(false); \
	}

int main(int argc, char* argv[])
{
	GLenum error;

	int x = 1024;
	int y = 768;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
	}

	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)) {
		printf("set major version failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)) {
		printf("set attribute failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)) {
		printf("set attribute failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// use SDL_GL_ACCELERATED_VISUAL = 1 to require accelerated rendering

	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
		printf("set attribute failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// don't need depth buffer, not doing any 3d
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	//****
	// BEFORE RE-ENABLING HIGHDPI, THE CODE FOR GOING FULLSCREEN PROBABLY NEEDS TO RECREATE THE WINDOW AND CALL SDL_GL_MAKECURRENT() TO
	//   BIND THE CONTEXT TO THE NEW WINDOW - NO WAY TO TURN OFF HIGHDPI WHICH DOENS'T WORK ON IN TRUE FULLSCREEN MODE
	//****
	auto window = SDL_CreateWindow("Game!", 100, 100, x, y,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL /*| SDL_WINDOW_ALLOW_HIGHDPI */);

	if (window == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	auto opengl_context = SDL_GL_CreateContext(window);
	if (opengl_context == NULL) {
		printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// power saving mode
	SDL_GL_SetSwapInterval(2);

	gladLoadGL();

	glEnable(GL_BLEND);
	CHECK_GL_ERROR
	glEnable(GL_CULL_FACE);
	CHECK_GL_ERROR
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	CHECK_GL_ERROR

	if (SDL_SetWindowDisplayMode(window, 0) != 0) {
		assert(false);
	}
	CHECK_GL_ERROR
	

	SDL_GL_SetSwapInterval(0);
	CHECK_GL_ERROR

	SDL_GL_GetDrawableSize(window, &x, &y);
	CHECK_GL_ERROR
	glViewport(0, 0, x, y);
	CHECK_GL_ERROR

	/*const std::string vs = SHADER_SOURCE
		(
			attribute vec4 vPosition;
	void main()
	{
		gl_Position = vPosition;
	}
	);

	const std::string fs = SHADER_SOURCE
		(
			precision mediump float;
	void main()
	{
		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
	);

	GLuint mProgram = CompileProgram(vs, fs);
	if (!mProgram)
	{
		return false;
	}*/


//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLfloat vertices[] =
	{
		0.0f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
	};

	// Use the program object
//	glUseProgram(mProgram);

	while (true) {
		glClearColor(1,0,1,1); // green bg
		CHECK_GL_ERROR
											  
        // Clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT);
		CHECK_GL_ERROR

		SDL_GL_SwapWindow(window); // swap double buffer to foregound
		CHECK_GL_ERROR

		// Load the vertex data
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		//glEnableVertexAttribArray(0);

		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//std::this_thread::sleep_for(std::chrono::seconds(10));
	}

	return 0;
}
