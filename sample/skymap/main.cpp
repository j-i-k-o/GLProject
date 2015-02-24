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
#include "simple2.vert"
;
const std::string fshader_source = 
#include "simple2.frag"
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
	SDL_Window* window = SDL_CreateWindow("SDL_Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1200, 800, SDL_WINDOW_OPENGL);
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

	Mesh3D mesh;
	MeshSample::CubeMap cube(1024);
	mesh.copyData(cube.getVertex(), cube.getNormal(), cube.getTexcrd(), cube.getNumVertex());

	Camera camera;
	camera.setPos(glm::vec3(0.0f, 0.0f, 0.0f));
	camera.setUp(glm::vec3(0.0f, 1.0f, 0.0f));
	camera.setFar(1024);

	Texture<TextureCubeMap> texture;
	texture.texImage2D("negx.jpg","posx.jpg","negy.jpg","posy.jpg","negz.jpg","posz.jpg");
	texture.setParameter<Mag_Filter<GL_LINEAR>, Min_Filter<GL_LINEAR>>();
	obj.connectAttrib(program, mesh, "vertex", "normal", "texcrd");
	program.setUniformMatrixXtv("model", glm::value_ptr(mesh.getModelMatrix()), 1, 4);
	program.setUniformMatrixXtv("projection", glm::value_ptr(camera.getProjectionMatrix()), 1, 4);

	program.setUniformXt("textureobj", 0);


	bool quit = false;
	SDL_Event e;
	//	SDL_WaitThread(threadID, NULL);

	float theta = M_PI/2.0f;
	float phi = 0;
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
		const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
		if(currentKeyStates[SDL_SCANCODE_UP])
		{
			theta-=0.05f;
		}
		if(currentKeyStates[SDL_SCANCODE_DOWN])
		{
			theta+=0.05f;
		}
		if(currentKeyStates[SDL_SCANCODE_RIGHT])
		{
			phi-=0.05f;
		}
		if(currentKeyStates[SDL_SCANCODE_LEFT])
		{
			phi+=0.05f;
		}
		if(theta <= 0.0f) theta=0.0f;
		if(theta >= M_PI) theta=M_PI;

		camera.setDrct(glm::vec3(sin(theta)*sin(phi), cos(theta), sin(theta)*cos(phi)));
		program.setUniformMatrixXtv("view", glm::value_ptr(camera.getViewMatrix()), 1, 4);
		
		obj.clearColor(0.0f, 0.0f, 0.2f, 1.0f);
		obj.clearDepth(1.0);
		obj.viewport(0,0,1200,800);
		obj.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		texture.bind(0);
		obj.draw(mesh, program);
		texture.unbind();
		SDL_GL_SwapWindow( window );
	}

	//	obj << End();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
