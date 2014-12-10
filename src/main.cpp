#include "../include/MyGL.h"
#include "../include/MyGL.h"
#include <vector>

GLLib::GLObject obj;

int threadfunction(void* data)
{
	using namespace GLLib;

//	obj.initialize(static_cast<SDL_Window*>(data));
//	obj.makeCurrent(static_cast<SDL_Window*>(data));
	return 0;
}

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

	if(GLEW_VERSION_4_1)
	{
		std::cout << "4.1 supported" << std::endl;
	}

	//Shader test
	using VShader = Shader<VertexShader>;
	using FShader = Shader<FragmentShader>;
	VShader vshader;
	FShader fshader;

	ShaderProg<> program;

	vshader << "#version 120\nattribute vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }";
	fshader << "#version 120\nvoid main() { gl_FragColor = vec4( 1.0, 1.0, 1.0, 1.0 ); }";

	program << vshader << fshader << link_these();

	VBO vertex;
	IBO index;
	GLfloat vertexData[][2] = 
	{
		{-0.5f, -0.5f},
		{0.5f, -0.5f},
		{0.5f, 0.5f},
		{-0.5f, 0.5f}
	};

	GLuint indexData[][1] ={{0},{1},{2},{3}};

	vertex << vertexData;
	index << indexData;

	VAO varray;

	obj.connectAttrib(program, vertex, varray, "LVertexPos2D");

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
		obj.drawElements(varray, program, index);
		SDL_GL_SwapWindow( window );
	}
	obj << End();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
