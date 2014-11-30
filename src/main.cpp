#include "../include/MyGL.h"

GLLib::GLObject obj;

int threadfunction(void* data)
{
	using namespace GLLib;

	obj.initialize(static_cast<SDL_Window*>(data));
	obj.makeCurrent(static_cast<SDL_Window*>(data));
	return 0;
}

int main(int argc, char const* argv[])
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

	obj.initialize(window);
	obj.makeCurrent(window);

	//Shader test
	using VShader = Shader<VertexShader>;
	using FShader = Shader<FragmentShader>;
	VShader shader;
	VShader shader2;
	shader = std::move(shader2);

	VShader shader3 = VShader();

	ShaderProg<> program;

	program << shader << (shader3 << "poyopoyo") << link_these();

	VertexBuffer<ArrayBuffer, StaticDraw> buffer;

	auto array = make_common_array(make_common_array(1,2,3), make_common_array(4,5,6), make_common_array(7,8,9), make_common_array(10,11,12));
	buffer.setArray<int, 4, 3>(array);

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
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(1.0, 1.0, 0.0, 1.0);
		SDL_GL_SwapWindow( window );
	}
	obj.close();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
