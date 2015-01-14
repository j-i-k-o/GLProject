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

	Texture<Texture2D> texture;
	texture.texImage2D("texture.jpg");

	Mesh3D cube;
	MeshSample::Sphere sphereHelper(1.0, 30, 30);
	MeshSample::Cube cubeHelper(1.0);
	cube.copyData(cubeHelper.getVertex(), cubeHelper.getNormal(), cubeHelper.getTexcrd(), cubeHelper.getNumVertex());

	Camera camera;
	camera.setPos(glm::vec3(3.0f, 3.0f, -2.0f));
	camera.setDrct(glm::vec3(0.0f, 0.0f, 0.0f));
	camera.setUp(glm::vec3(0.0f, 1.0f, 0.0f));
	camera.setFar(10.0f);

	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	camera.setAspect(width, height);

	obj.connectAttrib(program, cube.getNormal(), cube.getVArray(), "norm");
	obj.connectAttrib(program, cube.getVertex(), cube.getVArray(), "vertex");
	obj.connectAttrib(program, cube.getTexcrd(), cube.getVArray(), "texcrd");

	program.setUniformMatrixXtv("model", glm::value_ptr(cube.getModelMatrix()), 1, 4);
	program.setUniformMatrixXtv("view", glm::value_ptr(camera.getViewMatrix()), 1, 4);
	program.setUniformMatrixXtv("projection", glm::value_ptr(camera.getProjectionMatrix()), 1, 4);

	program.setUniformXt("light.ambient", 0.25f, 0.25f, 0.25f, 1.0f);
	program.setUniformXt("light.diffuse", 1.0f, 1.0f, 1.0f, 1.0f);
	program.setUniformXt("light.specular", 1.0f, 1.0f, 1.0f, 1.0f);

	program.setUniformXt("material.ambient", 0.3f, 0.0f, 0.4f, 1.0f);
	program.setUniformXt("material.diffuse", 0.75f, 0.0f, 1.0f, 1.0f);
	program.setUniformXt("material.specular", 1.0f, 1.0f, 1.0f, 1.0f);
	program.setUniformXt("material.shininess", 3.0f);

	program.setUniformXt("attenuation.constant", 0.0f);
	program.setUniformXt("attenuation.linear", 0.0f);
	program.setUniformXt("attenuation.quadratic", 0.075f);

	program.setUniformXt("textureobj", 0);

	int poyo[] =
	{
		2,
		3,
		4,
		5,
		6
	};

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
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//texture.bind(0);
		program.setUniformXt("light.position", 0.0f, 0.7f, 0.0f);
		obj.draw(cube, program);
		program.setUniformXt("light.position", 0.0f, 0.7f, 0.0f);
		obj.draw(cube, program);
		texture.unbind();
		SDL_GL_SwapWindow( window );
	}

	//	obj << End();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
