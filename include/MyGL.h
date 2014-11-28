#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <cassert>
#include <string>
#include <functional>
#include "HelperClass.h"

namespace GLLib {

	/*
	 * Check OpenGL Error
	 */

#ifdef DEBUG
#define CHECK_GL_ERROR \
	GLenum _err_ = glGetError(); \
	assert(_err_ != GL_INVALID_ENUM); \
	assert(_err_ != GL_INVALID_VALUE); \
	assert(_err_ != GL_INVALID_OPERATION); \
	assert(_err_ != GL_STACK_OVERFLOW); \
	assert(_err_ != GL_STACK_UNDERFLOW); \
	assert(_err_ != GL_OUT_OF_MEMORY); \
	assert(_err_ != GL_TABLE_TOO_LARGE); \
	std::cout << "OK" << std::endl;
#else
#define CHECK_GL_ERROR
#endif

	//setAttribute
	template<typename Attr_First>
		void setAttribute()
		{
			Attr_First::SetAttr();
		}
	template<typename Attr_First, typename Attr_Second, typename... Rests>
		void setAttribute()
		{
			setAttribute<Attr_First>();
			setAttribute<Attr_Second,Rests...>();
		}


	class GLObject
	{
		private:
			//member variable
			SDL_GLContext _context;
			bool _is_initialized;

		public:
			GLObject() :_is_initialized(false)
		{
		}
			bool initialize(SDL_Window* _window)
			{
				bool success = true;
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
				return success;
			}

			void close()
			{
				if(_is_initialized)
				{
					SDL_GL_DeleteContext(_context);
					DEBUG_OUT("SDL_GL_Context is successfully closed.");
					_is_initialized = false;
				}
			}

			void makeCurrent(SDL_Window* window)
			{
				if(SDL_GL_MakeCurrent(window, _context) < 0)
				{
					std::cerr << "MakeCurrent failed!: " << SDL_GetError() << std::endl;
				}
				else
				{
					DEBUG_OUT("SDL_GL_MakeCurrent");
				}
			}
	};

	//shader
	template<typename Shader_type, typename Allocator = GLAllocator<Alloc_Shader>>
		class Shader
		{
			private:
				GLuint shader_id;
				Allocator a;
			public:
				Shader()
				{
					shader_id = a.construct(Shader_type::SHADER_TYPE);
					DEBUG_OUT("shader created! shader_id is " << shader_id);
					CHECK_GL_ERROR;
				}
				~Shader()
				{
					a.destruct(shader_id);
					DEBUG_OUT("shader destructed!");
				}

				Shader(const Shader<Shader_type, Allocator> &obj)
				{
					this->shader_id = obj.shader_id;
					a.overwrite(obj.a);
				}

				Shader& operator=(const Shader<Shader_type, Allocator> &obj)
				{
					a.destruct(shader_id);
					shader_id = obj.shader_id;
					a.overwrite(obj.a);
					return *this;
				}
		};



} // namespace GLLib
