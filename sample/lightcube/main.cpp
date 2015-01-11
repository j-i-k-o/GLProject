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


	//SDL_Window* window = SDL_CreateWindow("SDL_Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 200, 200, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_Window* window = SDL_CreateWindow("SDL_Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 300, SDL_WINDOW_OPENGL);
	if(window == NULL)
	{
		std::cerr << "Window could not be created!: " << SDL_GetError() << std::endl;
	}

	SDL_GLContext context;

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

	Mesh3D cube;
	MeshSample::Cube cubeHelper(1.0);
	cube.copyData(cubeHelper.getVertex(), cubeHelper.getNormal(), cubeHelper.getNumVertex());

	Camera camera;
	camera.setPos(glm::vec3(3.0f, 3.0f, 3.0f));
	camera.setDrct(glm::vec3(0.0f, 0.0f, 0.0f));
	camera.setFar(100.0f);

	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	camera.setAspect(width, height);

	obj.connectAttrib(program, cube.getNormal(), cube.getVArray(), "norm");
	obj.connectAttrib(program, cube.getVertex(), cube.getVArray(), "vertex");

	program.setUniformMatrixXtv("model", glm::value_ptr(cube.getModelMatrix()), 1, 4);
	program.setUniformMatrixXtv("view", glm::value_ptr(camera.getViewMatrix()), 1, 4);
	program.setUniformMatrixXtv("projection", glm::value_ptr(camera.getProjectionMatrix()), 1, 4);

	program.setUniformXt("light.ambient", 0.25f, 0.25f, 0.25f, 1.0f);
	program.setUniformXt("light.diffuse", 1.0f, 1.0f, 1.0f, 1.0f);
	program.setUniformXt("light.specular", 1.0f, 1.0f, 1.0f, 1.0f);
	program.setUniformXt("light.position", 0.0f, 0.6f, 0.0f);

	program.setUniformXt("material.ambient", 0.3f, 0.25f, 0.4f, 1.0f);
	program.setUniformXt("material.diffuse", 0.75f, 0.0f, 1.0f, 1.0f);
	program.setUniformXt("material.specular", 1.0f, 1.0f, 1.0f, 1.0f);
	program.setUniformXt("material.shininess", 50.0f);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_CULL_FACE);
		obj.draw(cube, program);
		SDL_GL_SwapWindow( window );
	}

	//	obj << End();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
