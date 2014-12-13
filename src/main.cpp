#include "../include/gl_all.h"
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

jikoLib::GLLib::GLObject obj;

int threadfunction(void* data)
{
	using namespace jikoLib::GLLib;

	return 0;
}

const std::string vshader_source = 
#include "shader.vert"
;
const std::string fshader_source = 
#include "shader.frag"
;


int main(int argc, char* argv[])
{
	using namespace jikoLib::GLLib;


	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cerr << "Cannot Initialize SDL!: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 


	SDL_Window* window = SDL_CreateWindow("SDL_Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 200, 200, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(window == NULL)
	{
		std::cerr << "Window could not be created!: " << SDL_GetError() << std::endl;
	}
//	SDL_Thread* threadID = SDL_CreateThread(threadfunction, "MyThread", (void*)(window));
//
//
//
//	setAttribute<Attr_GLVersion<2,1>>();

	SDL_GLContext context;

	/*
	obj << Begin(window);
	obj << MakeCurrent(window);
	*/
	obj.initialize([&](){context = SDL_GL_CreateContext(window);});

	SDL_GL_SetSwapInterval(1);

	obj.makeCurrent([&](){SDL_GL_MakeCurrent(window, context);});

	VShader vshader;
	FShader fshader;

	ShaderProgram program;

	vshader << vshader_source;
	fshader << fshader_source;

	program << vshader << fshader << link_these();

	VBO vertex;
	VBO vertex2;
	VBO color;
	IBO index;

	GLfloat vertexData[][2] = 
	{
		{-1.0f, -1.0f},
		{0.5f, -0.5f},
		{0.5f, 1.0f},
	};

	GLfloat vertexData2[][2] = 
	{
		{-1.0f, 1.0f},
		{-1.0f, 0.5f},
		{-0.5f, 1.0f},
	};

	GLfloat colorData[][3] =
	{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f}
	};

	GLushort indexData[] ={0,1,2};

	vertex << vertexData;
	vertex2 << vertexData2;
	index << indexData;

	vertex = vertex + vertex2;

	color << colorData;
	color = color + color;

	VAO varray;

	obj.connectAttrib(program, vertex, varray, "LVertexPos2D");
	obj.connectAttrib(program, color, varray, "Color");

	bool quit = false;
	SDL_Event e;
//	SDL_WaitThread(threadID, NULL);

	while( !quit )
	{
		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit
			if( e.type == SDL_QUIT )
			{
				quit = true;
			}
		}
		CHECK_GL_ERROR;
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		obj.draw<rm_Triangles>(varray, program, vertex);
		SDL_GL_SwapWindow( window );
	}
//	obj << End();

	obj.finalize([&](){SDL_GL_DeleteContext(context);});
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
