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
		{0.5, 0.5},
		{-0.5, 0.5},
		{-0.5, -0.5},
		{0.5, -0.5}
	};

	GLfloat texc[][2] = 
	{
		{1.0, 1.0},
		{0.0, 1.0},
		{0.0, 0.0},
		{1.0, 0.0}
	};

	GLushort ind[] = {0,2,3};

	VBO vertex;
	vertex << vert;
	VBO texcoord;
	texcoord << texc;
	IBO indi;
	indi << ind;

	VAO vao;

	//connect attrib
	obj.connectAttrib(program, vertex, vao, "position");
	obj.connectAttrib(program, texcoord, vao, "texcoord");
	

	Texture<Texture2D> texture;
	texture.texImage2D("arch-linux-226331.jpg");

	texture.bind();
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	 texture.unbind();
	program.setUniformXt("surftexture",(GLint)texture.getUnit());


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
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		//glEnable(GL_CULL_FACE);
		obj.draw<rm_Triangles>(vao, program, indi, {texture});
		SDL_GL_SwapWindow( window );
	}

//	obj << End();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
