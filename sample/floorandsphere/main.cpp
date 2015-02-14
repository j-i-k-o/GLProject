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

	Texture<Texture2D> texture;
	texture.texImage2D("texture.jpg");
	texture.setParameter<Wrap_S<GL_REPEAT>, Wrap_T<GL_REPEAT>, Wrap_R<GL_REPEAT>, Mag_Filter<GL_NEAREST>, Min_Filter<GL_NEAREST>>();

	GLfloat floor_vertex[][3] = 
	{
		{ 100.0f,  100.0f, 0.0f},
		{-100.0f,  100.0f, 0.0f},
		{-100.0f, -100.0f, 0.0f},
		{ 100.0f, -100.0f, 0.0f}
	};

	const GLfloat floor_normal[][3] = 
	{
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f}
	};

	const GLfloat floor_texcrd[][2] = 
	{
		{10.0f, 10.0f},
		{0.0f, 10.0f},
		{0.0f, 0.0f},
		{10.0f, 0.0f}
	};

	const GLushort floor_index[] = 
	{
		0,1,2,0,2,3
	};

	Mesh3D floor_mesh;
	floor_mesh.copyData(floor_vertex, floor_normal, floor_texcrd);
	floor_mesh.copyIndex(floor_index);

	RBO rbo;
	FBO fbo;
	Texture<TextureCubeMap> cubemap;
	Texture<Texture3D> _3d;
	fbo.attach<ColorAttachment<0>, CubeMapPosX>(cubemap);

	Mesh3D cube_mesh;
	MeshSample::Sphere cubehelper(5.0, 50, 50);
	cube_mesh.copyData(cubehelper.getVertex(), cubehelper.getNormal(), cubehelper.getTexcrd(), cubehelper.getNumVertex());
	cube_mesh.setPos(glm::vec3(0.0f, 0.0f, 5.0f));

	Camera camera;
	camera.setPos(glm::vec3(-40.0f, -40.0f, 13.0f));
	camera.setDrct(glm::vec3(50.0f, 50.0f, 0.0f));
	camera.setAspect(1200, 800);
	camera.setFar(1000.0f);
	camera.setUp(glm::vec3(0.0f, 0.0f, 1.0f));


	program.setUniformMatrixXtv("view", glm::value_ptr(camera.getViewMatrix()), 1, 4);
	program.setUniformMatrixXtv("projection", glm::value_ptr(camera.getProjectionMatrix()), 1, 4);

	program.setUniformXt("light.ambient", 0.75f, 0.75f, 0.75f, 1.0f);
	program.setUniformXt("light.diffuse", 1.0f, 1.0f, 1.0f, 1.0f);
	program.setUniformXt("light.specular", 1.0f, 1.0f, 1.0f, 1.0f);
	program.setUniformXt("light.position", -1.0f, -1.0f, 10.0f);

	program.setUniformXt("material.ambient", 0.3f, 0.25f, 0.4f, 1.0f);
	program.setUniformXt("material.diffuse", 0.75f, 0.0f, 1.0f, 1.0f);
	program.setUniformXt("material.specular", 1.0f, 1.0f, 1.0f, 1.0f);
	program.setUniformXt("material.shininess", 32.0f);

	program.setUniformXt("attenuation.constant", 0.0f);
	program.setUniformXt("attenuation.linear", 0.0f);
	program.setUniformXt("attenuation.quadratic", 0.05f);

	program.setUniformXt("textureobj", 0);

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
		obj.connectAttrib(program, floor_mesh, "vertex", "normal", "texcrd");
		program.setUniformMatrixXtv("model", glm::value_ptr(floor_mesh.getModelMatrix()), 1, 4);
		texture.bind(0);
		obj.draw(floor_mesh, program);
		obj.connectAttrib(program, cube_mesh, "vertex", "normal", "texcrd");
		program.setUniformMatrixXtv("model", glm::value_ptr(cube_mesh.getModelMatrix()), 1, 4);
		texture.bind(0);
		obj.draw(cube_mesh, program);
		SDL_GL_SwapWindow( window );
	}

	//	obj << End();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
