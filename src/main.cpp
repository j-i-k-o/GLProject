#include "../include/MyGL.h"

int main(int argc, char const* argv[])
{
	using namespace GLLib;
	GLObject obj;

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cerr << "Cannot Initialize SDL!: " << SDL_GetError() << std::endl;
		return -1;
	}
	setAttribute<Attr_GLVersion<3,1>>();

	SDL_Window* window = SDL_CreateWindow("SDL_Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI);
	if(window == NULL)
	{
		std::cerr << "Window could not be created!: " << SDL_GetError() << std::endl;
	}
	setAttribute<Attr_GLSize<5,5,32,1>>();
	obj.initialize(window);
	obj.close();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
