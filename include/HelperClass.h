#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "gl_debug.h"

namespace GLLib
{

	/*
	 * SetAttribute helper
	 */
	template<int Major=3, int Minor=1>
		struct Attr_GLVersion
		{
			static void SetAttr()
			{
				//exec at rumtime
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, Major);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, Minor);
				SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
				DEBUG_OUT("Set OpenGL major version: " << Major);
				DEBUG_OUT("Set OpenGL minor version: " << Minor);
			}
		};
	template<int RED_SIZE=5,
		int GREEN_SIZE=5,
		int BLUE_SIZE=5,
		int DEPTH_SIZE=32,
		int DOUBLEBUFFER=1>
		struct Attr_GLSize
		{
			static void SetAttr()
			{
				//exec at rumtime
				SDL_GL_SetAttribute(SDL_GL_RED_SIZE, RED_SIZE);
				SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, GREEN_SIZE);
				SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, BLUE_SIZE);
				SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, DEPTH_SIZE);
				SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, DOUBLEBUFFER);
				DEBUG_OUT("Set OpenGL Buffer Size and Doublebuffer");
				DEBUG_OUT("RED_SIZE: " << RED_SIZE);
				DEBUG_OUT("GREEN_SIZE: " << GREEN_SIZE);
				DEBUG_OUT("BLUE_SIZE: " << BLUE_SIZE);
				DEBUG_OUT("DEPTH_SIZE: " << DEPTH_SIZE);
				DEBUG_OUT("DOUBLEBUFFER: " << DOUBLEBUFFER);
			}
		};

	/*
	 *	ScrenSize helper
	 */
	template<int32_t Width, int32_t Height>
		struct ScreenSize
		{
			constexpr static auto ScrHeight = Height;
			constexpr static auto ScrWidth = Width;
		};
}
