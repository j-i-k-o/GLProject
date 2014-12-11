#include "../include/MyGL.h"
#include "../include/MyGL.h"
#include <vector>

GLLib::GLObject obj;

int threadfunction(void* data)
{
	using namespace GLLib;

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
	using namespace GLLib;

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cerr << "Cannot Initialize SDL!: " << SDL_GetError() << std::endl;
		return -1;
	}
	setAttribute<Attr_GLVersion<3,1>>();

	SDL_Window* window = SDL_CreateWindow("SDL_Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 200, 200, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(window == NULL)
	{
		std::cerr << "Window could not be created!: " << SDL_GetError() << std::endl;
	}
	setAttribute<Attr_GLSize<5,5,5,32,1>>();
//	SDL_Thread* threadID = SDL_CreateThread(threadfunction, "MyThread", (void*)(window));

	obj << Begin(window);
	obj << MakeCurrent(window);

	//Shader test
	using VShader = Shader<VertexShader>;
	using FShader = Shader<FragmentShader>;
	VShader vshader;
	FShader fshader;

	ShaderProg<> program;

	vshader << vshader_source;
	fshader << fshader_source;

	program << vshader << fshader << link_these();

	VBO vertex;
	VBO color;
	IBO index;

	GLfloat vertexData[][2] = 
	{
		{-1.0f, -1.0f},
		{0.5f, -0.5f},
		{0.5f, 1.0f},
	};

	GLfloat colorData[][3] =
	{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	};

	GLushort indexData[] ={0,1,2};

	index << indexData;

	VAO varray;

	obj.connectAttrib(program, vertex << vertexData, varray, "LVertexPos2D");
	obj.connectAttrib(program, color << colorData, varray, "Color");

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
		obj.drawElements<rm_LineLoop>(varray, program, index);
		SDL_GL_SwapWindow( window );
	}
	obj << End();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
