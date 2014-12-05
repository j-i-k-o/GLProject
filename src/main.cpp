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
	IBO buffer2;

	auto array = make_common_array(make_common_array(1.0,2.0,3.0), make_common_array(4.0,5.0,6.0), make_common_array(7.0,8.0,9.0), make_common_array(10.0,11.0,12.0));
	GLfloat array2[][3] = {{1,2,3},{4,5,6},{7,8,9},{10,100,19},{3,4,5}};

	std::vector<std::vector<GLfloat>> array3;
	array3.push_back(std::vector<GLfloat>{1,2,3});
	array3.push_back(std::vector<GLfloat>{4,5,6});
	array3.push_back(std::vector<GLfloat>{7,8,9});
	array3.push_back(std::vector<GLfloat>{1,2,3});
	array3.push_back(std::vector<GLfloat>{1,2,4});

	buffer << array3;

	VAO varray,varray2,varray3;
	varray = varray3;

	obj.disconnectAttrib(program, "poyo");
	obj.connectAttrib(program, buffer, varray3, "poyo");

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
	obj << End();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
