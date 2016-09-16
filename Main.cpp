#define GL_GLEXT_PROTOTYPES 1

#include "EGL/egl.h"
#include "GLES2/gl2.h"
#include "shader_utils.h"
#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_egl.h"
#include <assert.h>
#include <iostream>
#include <chrono>

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

	SDL_Window * window = SDL_CreateWindow("Game!", 100, 100, x, y,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL /*| SDL_WINDOW_ALLOW_HIGHDPI */);

	if (window == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	//EGLDisplay* eglDisplay;
	//EGLContext* eglContext;
	//EGLSurface* eglSurface;
	EGLint configAttribList[] =
	{
		EGL_RED_SIZE,       8,
		EGL_GREEN_SIZE,     8,
		EGL_BLUE_SIZE,      8,
		EGL_ALPHA_SIZE,     8 /*: EGL_DONT_CARE*/,
		EGL_DEPTH_SIZE,     EGL_DONT_CARE,
		EGL_STENCIL_SIZE,   EGL_DONT_CARE,
		EGL_SAMPLE_BUFFERS, 0,
		EGL_NONE
	};
	EGLint surfaceAttribList[] =
	{
		//EGL_POST_SUB_BUFFER_SUPPORTED_NV, flags & (ES_WINDOW_POST_SUB_BUFFER_SUPPORTED) ? EGL_TRUE : EGL_FALSE, 
		EGL_POST_SUB_BUFFER_SUPPORTED_NV, EGL_FALSE,
		EGL_NONE, EGL_NONE
	};

	EGLint numConfigs;
	EGLint majorVersion;
	EGLint minorVersion;
	EGLDisplay display;
	EGLContext context;
	EGLSurface surface;
	EGLConfig config;
	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version); // initialize info structure with SDL version info 
	SDL_bool get_win_info = SDL_GetWindowWMInfo(window, &info);
	SDL_assert_release(get_win_info);
	EGLNativeWindowType hWnd = info.info.win.window;

	// Get Display 
	display = eglGetDisplay(GetDC(hWnd)); // EGL_DEFAULT_DISPLAY 
	if (display == EGL_NO_DISPLAY)
	{
		return EGL_FALSE;
	}

	// Initialize EGL 
	if (!eglInitialize(display, &majorVersion, &minorVersion))
	{
		return EGL_FALSE;
	}

	// Get configs 
	if (!eglGetConfigs(display, NULL, 0, &numConfigs))
	{
		return EGL_FALSE;
	}

	// Choose config 
	if (!eglChooseConfig(display, configAttribList, &config, 1, &numConfigs))
	{
		return EGL_FALSE;
	}

	// Create a surface 
	surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hWnd, surfaceAttribList);
	if (surface == EGL_NO_SURFACE)
	{
		return EGL_FALSE;
	}

	// Create a GL context 
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
	if (context == EGL_NO_CONTEXT)
	{
		return EGL_FALSE;
	}

	// Make the context current 
	if (!eglMakeCurrent(display, surface, surface, context))
	{
		return EGL_FALSE;
	}




	printf("GL_VERSION: %s\n", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
	printf("GL_SHADING_LANGUAGE_VERSION: %s\n", reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));


	const std::string vs = SHADER_SOURCE
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
		CHECK_GL_ERROR
		return 1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	
	
	GLfloat vertices[] =
	{
		0.0f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
	};

	glViewport(0, 0, x, y);

	// Use the program object
	glUseProgram(mProgram);

	// Load the vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(0);

	auto start = std::chrono::steady_clock::now();
	long long deltaTime;
	const float PERIOD = 5000.0f;
	const float SCALE = 100.0f;

	SDL_Event event;
	int quit = 0;
	while (!quit)
	{
		deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::steady_clock::now() - start).count();
		auto r = (sin(deltaTime * 2 * M_PI / PERIOD) * (SCALE/2) + (SCALE/2))/100;
		auto g = (sin(deltaTime * 2 * M_PI / PERIOD+1000) * (SCALE / 2) + (SCALE / 2)) / 100;
		auto b = (sin(deltaTime * 2 * M_PI / PERIOD+2000) * (SCALE / 2) + (SCALE / 2)) / 100;
		//printf("r %9.6f\n", r);
		glClearColor(r,
			g,
			b, 1.0f); 

		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		eglSwapBuffers(display, surface);

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = 1;
			}
		}
	}

	SDL_Quit();
	return 0;
}
