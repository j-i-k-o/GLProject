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
	GLenum _err_;
#endif
#ifdef DEBUG
#define CHECK_GL_ERROR \
	_err_ = glGetError(); \
	assert(_err_ != GL_INVALID_ENUM && "GL_INVALID_ENUM"); \
	assert(_err_ != GL_INVALID_VALUE && "GL_INVALID_VALUE"); \
	assert(_err_ != GL_INVALID_OPERATION && "GL_INVALID_OPERATION"); \
	assert(_err_ != GL_STACK_OVERFLOW && "GL_STACK_OVERFLOW"); \
	assert(_err_ != GL_STACK_UNDERFLOW && "GL_STACK_UNDERFLOW"); \
	assert(_err_ != GL_OUT_OF_MEMORY && "GL_OUT_OF_MEMORY"); \
	assert(_err_ != GL_TABLE_TOO_LARGE && "GL_TABLE_TOO_LARGE");
#else
#define CHECK_GL_ERROR
#endif


	//setAttribute
	template<typename Attr_First>
		inline void setAttribute()
		{
			Attr_First::SetAttr();
		}
	template<typename Attr_First, typename Attr_Second, typename... Rests>
		inline void setAttribute()
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
				Shader()
					: is_compiled(false)
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
					: is_compiled(false)
				{
					this->shader_id = obj.shader_id;
					a.copy(obj.a);
					DEBUG_OUT("shader copied! shader_id is " << shader_id);
					CHECK_GL_ERROR;
				}

				Shader(Shader<Shader_type, Allocator>&& obj)
					: is_compiled(false)
				{
					this->shader_id = obj.shader_id;
					a.move(std::move(obj.a));
					DEBUG_OUT("shader moved! shader_id is " << shader_id);
					CHECK_GL_ERROR;
				}

				Shader& operator=(const Shader<Shader_type, Allocator> &obj)
				{
					a.destruct(shader_id);
					shader_id = obj.shader_id;
					a.copy(obj.a);
					CHECK_GL_ERROR;
					DEBUG_OUT("shader copied! shader_id is " << shader_id);
					return *this;
				}

				Shader& operator=(Shader<Shader_type, Allocator>&& obj)
				{
					a.destruct(shader_id);
					shader_id = obj.shader_id;
					a.move(std::move(obj.a));
					CHECK_GL_ERROR;
					DEBUG_OUT("shader moved! shader_id is " << shader_id);
					return *this;
				}

				Shader& operator<<(const std::string& str)
					//read shader source and compile
				{
					const char *c_str = str.c_str();
					const int length = str.length();
					glShaderSource(shader_id, 1, (const GLchar**)&c_str, &length);
					CHECK_GL_ERROR;

					GLint compiled, size;
					GLsizei len;
					char* buf = nullptr;
					glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
					CHECK_GL_ERROR;

					if(compiled == GL_FALSE)
					{
						//compile failed
						std::cerr << "id " << shader_id << " Compile Failed!: ";
						glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &size);
						CHECK_GL_ERROR;
						if(size > 0)
						{
							buf = new char[size];
							glGetShaderInfoLog(shader_id, size, &len, buf);
							CHECK_GL_ERROR;
							std::cerr << buf;
							delete[] buf;
						}
						std::cerr << std::endl;
					}
					else
					{
						is_compiled = true;
						DEBUG_OUT("shader compiled!");
					}
					return *this;
				}

				inline GLuint getID() const
				{
					return this->shader_id;
				}
		};

	//shaderprog
	template<typename Allocator = GLAllocator<Alloc_ShaderProg>> 
		class ShaderProg
		{
			private:
				GLuint shaderprog_id;
				Allocator a;
				bool isLinked;
			public:
				ShaderProg()
					: isLinked(false)
				{
					shaderprog_id = a.construct();
					CHECK_GL_ERROR;
					DEBUG_OUT("shaderprog created! shaderprog id is " << shaderprog_id);
				}

				~ShaderProg()
				{
					a.destruct(shaderprog_id);
					CHECK_GL_ERROR;
					DEBUG_OUT("shaderprog destructed!");
				}

				ShaderProg(const ShaderProg<Allocator> &obj)
					: isLinked(false)
				{
					shaderprog_id = obj.shaderprog_id;
					a.copy(obj.a);
					CHECK_GL_ERROR;
					DEBUG_OUT("shaderprog copied! shaderprog id is " << shaderprog_id);
				}

				ShaderProg(ShaderProg<Allocator>&& obj)
					: isLinked(false)
				{
					shaderprog_id = obj.shaderprog_id;
					a.move(std::move(obj.a));
					CHECK_GL_ERROR;
					DEBUG_OUT("shaderprog moved! shaderprog id is " << shaderprog_id);
				}

				ShaderProg& operator=(const ShaderProg<Allocator> &obj)
				{
					a.destruct(shaderprog_id);
					shaderprog_id = obj.shaderprog_id;
					a.copy(obj.a);
					CHECK_GL_ERROR;
					DEBUG_OUT("shaderprog copied! shaderprog id is " << shaderprog_id);
				}

				ShaderProg& operator=(ShaderProg<Allocator>&& obj)
				{
					a.destruct(shaderprog_id);
					shaderprog_id = obj.shaderprog_id;
					a.move(std::move(obj.a));
					CHECK_GL_ERROR;
					DEBUG_OUT("shaderprog moved! shaderprog id is " << shaderprog_id);
				}

				inline GLuint getID() const
				{
					return shaderprog_id;
				}

				template<typename Shader_type, typename Shader_Allocator>
					ShaderProg& operator<<(const Shader<Shader_type, Shader_Allocator> &shader)
					//attach shader
					{
						glAttachShader(shaderprog_id, shader.getID());
						CHECK_GL_ERROR;
						DEBUG_OUT("attach shader! shader id is "
								<< shader.getID() 
								<< ". shaderprog id is" 
								<< shaderprog_id);
						return *this;
					}

				ShaderProg& operator<<(link_these&&)
					//link shader
				{
					glLinkProgram(shaderprog_id);
					CHECK_GL_ERROR;

					GLint linked;
					int size=0, len=0;

					glGetProgramiv(shaderprog_id, GL_LINK_STATUS, &linked);
					CHECK_GL_ERROR;
					if(linked == GL_FALSE)
					{
						std::cerr << "id "<< shaderprog_id <<" Link Failed!: ";
						glGetProgramiv(shaderprog_id, GL_INFO_LOG_LENGTH, &size);
						CHECK_GL_ERROR;
						if(size > 0)
						{
							char *buf = new char[size];
							glGetProgramInfoLog(shaderprog_id, size, &len, buf);
							CHECK_GL_ERROR;
							std::cerr << buf;
							delete[] buf;
						}
						std::cerr << std::endl;
					}
					else
					{
						isLinked = true;
						DEBUG_OUT("shader linked!");
					}
					return *this;
				}
		};

	template<typename TargetType, typename UsageType, typename Allocator = GLAllocator<Alloc_VertexBuffer>>
		class VertexBuffer
		{
			private:
				GLuint buffer_id;
				Allocator a;



			public:

				inline void bind()
				{
					glBindBuffer(TargetType::BUFFER_TARGET, buffer_id);
					CHECK_GL_ERROR;
				}

				VertexBuffer()
				{
					buffer_id = a.construct();
					bind();
					glBufferData(TargetType::BUFFER_TARGET, 0, NULL, UsageType::BUFFER_USAGE);
					CHECK_GL_ERROR;
					DEBUG_OUT("vbuffer created! id is " << buffer_id);
				}

				~VertexBuffer()
				{
					a.destruct(buffer_id);
					CHECK_GL_ERROR;
					DEBUG_OUT("vbuffer destructed!");
				}

				VertexBuffer(const VertexBuffer<TargetType, UsageType> &obj)
				{
					buffer_id = obj.buffer_id;
					a.copy(obj.a);
					CHECK_GL_ERROR;
					DEBUG_OUT("vbuffer copied! id is " << buffer_id);
				}

				VertexBuffer(VertexBuffer<TargetType, UsageType>&& obj)
				{
					buffer_id = obj.buffer_id;
					a.move(std::move(obj.a));
					CHECK_GL_ERROR;
					DEBUG_OUT("vbuffer moved! id is " << buffer_id);
				}

				VertexBuffer& operator=(const VertexBuffer<TargetType, UsageType> &obj)
				{
					a.destruct(buffer_id);
					buffer_id = obj.buffer_id;
					a.copy(obj.a);
					CHECK_GL_ERROR;
					DEBUG_OUT("vbuffer copied! id is " << buffer_id);
					return *this;
				}

				VertexBuffer& operator=(VertexBuffer<TargetType, UsageType>&& obj)
				{
					a.destruct(buffer_id);
					buffer_id = obj.buffer_id;
					a.move(std::move(obj.a));
					CHECK_GL_ERROR;
					DEBUG_OUT("vbuffer moved! id is " << buffer_id);
					return *this;
				}

				inline GLuint getID() const
				{
					return buffer_id;
				}

				template<typename T, std::size_t Size_Elem, std::size_t dim>
					VertexBuffer& setArray(const multi_array<T, Size_Elem, dim> &array)
				{
					static_assert(dim <= 4, "array's dim must be four or less.");
					bind();
					glBufferData(TargetType::BUFFER_TARGET, Size_Elem*dim*sizeof(T), array.data(), UsageType::BUFFER_USAGE);
					CHECK_GL_ERROR;
					DEBUG_OUT("allocate success! buffer id is " << buffer_id);
					return *this;
				}

		};
}

