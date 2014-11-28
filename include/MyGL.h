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
	std::cout << "CHECK_GL_ERROR: OK at  " << __FILE__ << ": " << __LINE__ << std::endl;
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

	//getID
	template<typename T>
		inline GLuint getID(const T &obj)
		{
			return obj.getID();
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
				bool is_compiled;
				Allocator a;
			public:
				Shader() : is_compiled(false)
				{
					shader_id = a.construct(Shader_type::SHADER_TYPE);
					DEBUG_OUT("shader created! shader_id is " << shader_id);
					CHECK_GL_ERROR;
				}
				~Shader()
				{
					a.destruct(shader_id);
					CHECK_GL_ERROR;
					DEBUG_OUT("shader destructed!");
				}

				Shader(const Shader<Shader_type, Allocator> &obj)
				{
					this->shader_id = obj.shader_id;
					a.overwrite(obj.a);
					CHECK_GL_ERROR;
				}

				Shader& operator=(const Shader<Shader_type, Allocator> &obj)
				{
					a.destruct(shader_id);
					shader_id = obj.shader_id;
					a.overwrite(obj.a);
					CHECK_GL_ERROR;
					DEBUG_OUT("shader copied! shader_id is " << shader_id);
					return *this;
				}

				Shader& operator<<(const std::string& str)
					//read shader source
				{
					const char *c_str = str.c_str();
					const int length = str.length();
					glShaderSource(shader_id, 1, (const GLchar**)&c_str, &length);
					CHECK_GL_ERROR;
					GLint compiled, size;
					GLsizei len;
					char* buf;
					glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
					if(compiled == GL_FALSE)
					{
						//compile failed
						glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &size);
						if(size > 0)
						{
							glGetShaderInfoLog(shader_id, size, &len, buf);
							std::cerr << "Compile Error!: " << buf << std::endl;
						}
					}
					else
					{
						is_compiled = true;
						DEBUG_OUT("shader compiled!");
					}
				}

				inline GLuint getID()
				{
					return this->shader_id;
				}
		};

	template<typename Allocator = GLAllocator<Alloc_ShaderProg>> 
		class ShaderProg
		{
			private:
				GLuint shaderprog_id;
				Allocator a;
			public:
				ShaderProg()
				{
					shaderprog_id = a.construct();
					CHECK_GL_ERROR;
					DEBUG_OUT("shaderprog created! shaderprog id is " << shaderprog_id);
				}

		};



} // namespace GLLib
