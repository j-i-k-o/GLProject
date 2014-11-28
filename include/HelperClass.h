#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include "gl_debug.h"
#include <functional>

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


	//available alloc type
	struct Alloc_Shader {};
	struct Alloc_ShaderProg {};



	//alloc traits
	template<typename T>
		struct GLAllocTraits{};

	// note:
	// these traits must have two functors named "allocfunc" and "deallocfunc".
	// the return value of allocfunc must be GLuint!

	template<>
		struct GLAllocTraits<Alloc_Shader>
		{
			using alloc_func_t = GLuint(*)(GLenum);
			using dealloc_func_t = void(*)(GLuint);

			constexpr static alloc_func_t& allocfunc = glCreateShader; 
			constexpr static dealloc_func_t& deallocfunc = glDeleteShader; 
		};

	template<>
		struct GLAllocTraits<Alloc_ShaderProg>
		{
			using alloc_func_t = GLuint(*)();
			using dealloc_func_t = void(*)(GLuint);

			constexpr static alloc_func_t& allocfunc = glCreateProgram; 
			constexpr static dealloc_func_t& deallocfunc = glDeleteProgram; 
		};


	/*
	 * GLAllocator
	 *
	 */

	template<typename T>
		class GLAllocator
		{
			private:
				int *ref_c;
				GLAllocator<T>& operator=(const GLAllocator<T> &);
			public:
				GLAllocator(){};
				GLAllocator(const GLAllocator<T> &) = delete;

				template<typename... Args>
					GLuint construct(Args... args)
					{
						ref_c = new int(1);
						return GLAllocTraits<T>::allocfunc(args...);
					}

				template<typename... Args>
					void destruct(Args... args)
					{
						if((ref_c != nullptr)&&(*ref_c > 0))
						{
							(*ref_c)--;
							if(*ref_c == 0)
							{
								GLAllocTraits<T>::deallocfunc(args...);
								delete ref_c;
								ref_c = nullptr;
							}
						}
					}

				template<typename Arg_type>
				void overwrite(const GLAllocator<Arg_type> &obj)
				{
					static_assert( std::is_same<T,Arg_type>::value, "overwrite with differet type!" );
					ref_c = obj.ref_c;
					(*ref_c)++;
				}
		};

	/*
	 * Shader_type
	 */
	struct VertexShader
	{
		constexpr static auto SHADER_TYPE = GL_VERTEX_SHADER;
	};
	struct FragmentShader
	{
		constexpr static auto SHADER_TYPE = GL_FRAGMENT_SHADER;
	};
}
