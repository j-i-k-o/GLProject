#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include "gl_debug.h"
#include <functional>
#include <array>
#include <type_traits>
#include <utility>

namespace GLLib
{

	/**
	 * Multi static array object
	 *
	 */

	template<typename T, std::size_t First, std::size_t... Rests>
		struct multi_array_traits
		{
			using type = std::array<typename multi_array_traits<T, Rests...>::type, First>; 
		};

	template<typename T, std::size_t Size>
		struct multi_array_traits<T, Size>
		{
			using type = std::array<T, Size>;
		};

	template<typename T, std::size_t... Sizes>
	using multi_array = typename multi_array_traits<T, Sizes...>::type;

	/**
	 * make_common_array function
	 *
	 */

	template<typename... Args>
		struct common_array_type
		{
			using type = std::array<typename std::decay<typename std::common_type<Args...>::type>::type, sizeof... (Args)>;
		};

	template<typename... Args>
		constexpr typename common_array_type<Args...>::type make_common_array(Args&&... args)
		{
			return typename common_array_type<Args...>::type{{std::forward<Args>(args)...}};
		}

	/**
	 * find nth number from non-type variadic template
	 *
	 */

	template<typename T, int n, T First, T... Rests> 
		struct find_elem
		{
			constexpr static T value = find_elem<T, n-1, Rests...>::value;
		};
	template<typename T, T First, T... Rests>
		struct find_elem<T, 0, First, Rests...>
		{
			constexpr static T value = First;
		};

	/**
	 * std::is_same for variadic template
	 *
	 */

	template<typename T, typename Type_First, typename... Type_Rests>
		struct is_exist
		{
			constexpr static bool value = std::is_same<T,Type_First>::value?true:is_exist<T, Type_Rests...>::value;
		};

	template<typename T, typename Type_Last>
		struct is_exist<T,Type_Last>
		{
			constexpr static bool value = std::is_same<T,Type_Last>::value;
		};

	/*
	 * connect type and OpenGL Enum
	 *
	 */

	template<typename T>
		struct getEnum{
			static_assert( is_exist<T,GLbyte,GLubyte,GLshort,GLushort,GLint,GLuint,GLfloat,GLdouble>::value, "Invalid type" );
			constexpr static GLenum value =
				std::is_same<T, GLbyte>::value	?		GL_BYTE				:
				std::is_same<T, GLubyte>::value	?		GL_UNSIGNED_BYTE	:
				std::is_same<T, GLshort>::value	?		GL_SHORT				:
				std::is_same<T, GLushort>::value	?		GL_UNSIGNED_SHORT	:
				std::is_same<T, GLint>::value		?		GL_INT				:
				std::is_same<T, GLuint>::value	?		GL_UNSIGNED_INT	:
				std::is_same<T, GLfloat>::value	?		GL_FLOAT				:
				std::is_same<T, GLdouble>::value	?		GL_DOUBLE			: static_cast<GLenum>(NULL);
		};


	/**
	 * GLObject Initialize and Destroy struct
	 *
	 */

	struct Begin
	{
		SDL_Window* m_window;
		Begin(SDL_Window* window):m_window(window){};
	};
	struct MakeCurrent
	{
		SDL_Window* m_window;
		MakeCurrent(SDL_Window* window):m_window(window){};
	};
	struct End{};



	/**
	 * SetAttribute helper
	 *
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

	/**
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
	struct Alloc_VertexBuffer {};
	struct Alloc_VertexArray {};



	//alloc traits
	template<typename T>
		struct GLAllocTraits{};

	// note:
	// these traits must have two function object named "allocfunc" and "deallocfunc".
	// the return type of allocfunc must be GLuint.

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

	template<>
		struct GLAllocTraits<Alloc_VertexBuffer>
		{
			using alloc_func_t = GLuint(*)();
			using dealloc_func_t = void(*)(GLuint);

			static GLuint my_glGenBuffers()
			{
				GLuint id;
				glGenBuffers(1, &id);
				return id;
			}

			static void my_glDeleteBuffers(GLuint id)
			{
				glDeleteBuffers(1, &id);
			}

			constexpr static alloc_func_t allocfunc = my_glGenBuffers; 
			constexpr static dealloc_func_t deallocfunc = my_glDeleteBuffers; 
		};

	template<>
		struct GLAllocTraits<Alloc_VertexArray>
		{
			using alloc_func_t = GLuint(*)();
			using dealloc_func_t = void(*)(GLuint);

			static GLuint my_glGenVertexArrays()
			{
				GLuint id;
				glGenVertexArrays(1, &id);
				return id;
			}

			static void my_glDeleteVertexArrays(GLuint id)
			{
				glDeleteVertexArrays(1, &id);
			}

			constexpr static alloc_func_t allocfunc = my_glGenVertexArrays; 
			constexpr static dealloc_func_t deallocfunc = my_glDeleteVertexArrays; 
		};


	/**
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
				GLAllocator():ref_c(nullptr){};
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
				void copy(const GLAllocator<Arg_type> &obj)
				{
					static_assert( std::is_same<T,Arg_type>::value, "copy with different type!" );
					ref_c = obj.ref_c;
					(*ref_c)++;
				}
				
				template<typename Arg_type>
				void move(GLAllocator<Arg_type>&& obj)
				{
					static_assert( std::is_same<T,Arg_type>::value, "move with different type!" );
					ref_c = obj.ref_c;
					obj.ref_c = nullptr;
				}
		};

	/**
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

	/**
	 * "link-these" struct for ShaderProg
	 */

	struct link_these{};

	/**
	 * Target_Type for VertexBuffer
	 */
	
	struct ArrayBuffer
	{
		constexpr static GLenum BUFFER_TARGET = GL_ARRAY_BUFFER;
	};

	struct ElementArrayBuffer //maybe for IBO
	{
		constexpr static GLenum BUFFER_TARGET = GL_ELEMENT_ARRAY_BUFFER;
	};

	/**
	 * Usage_Type for VertexBuffer
	 */

	struct StaticDraw
	{
		constexpr static GLenum BUFFER_USAGE = GL_STATIC_DRAW;
	};
}
