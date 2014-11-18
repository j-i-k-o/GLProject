#include "../include/MyGL.h"

int main(int argc, char const* argv[])
{
	using namespace GLLib;

	GLObject<Attr_GLVersion<3,1>,Attr_GLSize<5,5,5,32,1>,ScreenSize<1920,1080>> obj;
	obj.initialize("SDL_TITLE");
	obj.close();
	return 0;
}
