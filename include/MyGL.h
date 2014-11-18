#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <string>
#include "HelperClass.h"

namespace GLLib {

	template<typename Attr_GLver, typename Attr_Size, typename ScrSize>
		class GLObject
		{
			private:
				//setAttribute
				template<typename Attr_First>
					static void setAttribute()
					{
						Attr_First::SetAttr();
					}
				template<typename Attr_First, typename Attr_Second, typename... Rests>
					static void setAttribute()
					{
						setAttribute<Attr_First>();
						setAttribute<Attr_Second,Rests...>();
					}
				//member variable
				SDL_Window* _window;
				SDL_GLContext _context;
				bool _is_initialized;
				
			public:
				GLObject() :_is_initialized(false)
				{
				}

				bool initialize(std::string window_title)
				{
					bool success = true;
					//exec at rumtime
					if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
					{
						std::cerr << "Cannot Initialize SDL!: " << SDL_GetError() << std::endl;
						success = false;
					}
					else
					{
						//setAttribute
						setAttribute<Attr_GLver>();
						//create window
						_window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScrSize::ScrWidth, ScrSize::ScrHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI);
						if(_window == nullptr)
						{
							std::cerr << "Window could not be created!: " << SDL_GetError() << std::endl;
							success = false;
						}
						else
						{
							//setAttribute
							setAttribute<Attr_Size>();
							//create context
							_context = SDL_GL_CreateContext(_window);
							if(_context == NULL)
							{
								std::cerr << "Cannot create OpenGL context: " << SDL_GetError() << std::endl;
								success = false;
							}
							else
							{
								//initialize glew
								glewExperimental = GL_TRUE;
								GLenum glewError = glewInit();
								if(glewError != GLEW_OK)
								{
									std::cerr << "cannot initialize GLEW!: " << glewGetErrorString(glewError) << std::endl;
									success = false;
								}
								else
								{
									//use vsync
									if(SDL_GL_SetSwapInterval(1) < 0)
									{
										std::cerr << "warning: unable to set vsync!: " << SDL_GetError() << std::endl;
									}
									//OpenGL initialize complete
									DEBUG_OUT("--- OpenGL initialize complete ---");
									DEBUG_OUT("OpenGL Vendor: " << glGetString(GL_VENDOR));
									DEBUG_OUT("OpenGL Renderer: " << glGetString(GL_RENDERER));
									DEBUG_OUT("OpenGL Version: " << glGetString(GL_VERSION));
									DEBUG_OUT(" ");
									_is_initialized = true;
								}
							}
						}
					}
					return success;
				}

				void close()
				{
					if(_is_initialized)
					{
						SDL_GL_DeleteContext(_context);
						SDL_DestroyWindow(_window);
						_window = nullptr;
						SDL_Quit();
						DEBUG_OUT("SDL is successfully closed.");
						_is_initialized = false;
					}
				}
		};

} // namespace GLLib


