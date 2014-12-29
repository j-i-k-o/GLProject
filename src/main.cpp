#include "../include/gl_all.h"
#include <vector>
#include <SDL2/SDL.h>
#include <IL/ilu.h>
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
	context = SDL_GL_CreateContext(window);

	obj << Begin();

	SDL_GL_SetSwapInterval(1);

	SDL_GL_MakeCurrent(window, context);

	VShader vshader;
	FShader fshader;

	ShaderProgram program;

	vshader << vshader_source;
	fshader << fshader_source;

	program << vshader << fshader << link_these();

	GLfloat vert[][2] = 
	{
		{1.0, 0.5},
		{-0.5, 0.5},
		{-1.0, -0.5},
		{0.5, -0.5}
	};

	GLfloat texc[][2] = 
	{
		{1.0, 0.0},
		{0.0, 0.0},
		{0.0, 1.0},
		{1.0, 1.0}
	};

	GLushort ind[] = {0,2,3,0,1,2};

	VBO vertex;
	vertex << vert;
	VBO texcoord;
	texcoord << texc;
	IBO index;
	index << ind;
	
	VAO v_array;

	obj.connectAttrib(program, vertex, v_array, "position");
	obj.connectAttrib(program, texcoord, v_array, "texcoord");

	Texture<Texture2D> texture;
	texture.texImage2D("arch-linux-226331.jpg");

	texture.bind(0);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	texture.unbind();
	program.setUniformXt("surftexture", 1);

//	AssimpLoader al("Porsche_911_GT2.obj");

	std::vector<std::tuple<Texture<Texture2D>, std::size_t>> tex_array;
	tex_array.push_back(std::make_tuple(texture, 1));

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
		//glEnable(GL_CULL_FACE);
		obj.draw(v_array, program, index, tex_array);
		SDL_GL_SwapWindow( window );
	}

	//	obj << End();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
