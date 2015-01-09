#pragma once

#include <GL/glew.h>
#include "gl_debug.h"
#include <functional>
#include <vector>
#include <array>
#include <type_traits>
#include <utility>
#include <IL/il.h>
#include <IL/ilu.h>
#include <cmath>


namespace jikoLib
{
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
		 * Multi vector array object
		 *
		 */

		template<typename T, std::size_t Num>
			struct multi_vector_traits
			{
				static_assert(Num > 0, "invalid template argument");
				using type = std::vector<typename multi_vector_traits<T, Num-1>::type>;
			};

		template<typename T>
			struct multi_vector_traits<T, 1>
			{
				using type = std::vector<T>;
			};

		template<typename T, std::size_t Size>
			using multi_vector = typename multi_vector_traits<T, Size>::type;

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
		 * std::is_same for variadic template (exist)
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

		/**
		 * std::is_same for variadic template (all)
		 *
		 */

		template<typename T, typename Type_First, typename... Type_Rests>
			struct is_all_same
			{
				constexpr static bool value = std::is_same<T,Type_First>::value?is_all_same<T, Type_Rests...>::value:false;
			};

		template<typename T, typename Type_Last>
			struct is_all_same<T,Type_Last>
			{
				constexpr static bool value = std::is_same<T,Type_Last>::value;
			};

		/**
		 * select type by constant expression
		 *
		 */

		template<bool b, typename T, typename F>
			struct type_if
			{
				using type = F;
			};

		template<typename T, typename F>
			struct type_if<true, T, F>
			{
				using type = T;
			};


		/**
		 * connect type and OpenGL Enum
		 *
		 */

		template<typename T>
			struct getEnum{
				constexpr static GLenum value =
					std::is_same<T, GLbyte>::value?GL_BYTE	:
					std::is_same<T, GLubyte>::value?	GL_UNSIGNED_BYTE:
					std::is_same<T, GLshort>::value?	GL_SHORT	:
					std::is_same<T, GLushort>::value?GL_UNSIGNED_SHORT:
					std::is_same<T, GLint>::value?GL_INT:
					std::is_same<T, GLuint>::value?GL_UNSIGNED_INT:
					std::is_same<T, GLfloat>::value?GL_FLOAT:
					std::is_same<T, GLdouble>::value?GL_DOUBLE: static_cast<GLenum>(NULL);
				static_assert(value!=static_cast<GLenum>(NULL),"Invalid type");
			};

		template<GLenum TypeEnum>
			struct getType
			{
				using type =
					typename type_if<TypeEnum == GL_BYTE, GLbyte,
								typename type_if<TypeEnum == GL_UNSIGNED_BYTE, GLubyte,
								typename type_if<TypeEnum == GL_SHORT, GLshort,
								typename type_if<TypeEnum == GL_UNSIGNED_SHORT, GLushort,
								typename type_if<TypeEnum == GL_INT, GLint,
								typename type_if<TypeEnum == GL_UNSIGNED_INT, GLuint,
								typename type_if<TypeEnum == GL_FLOAT, GLfloat,
								typename type_if<TypeEnum == GL_DOUBLE, GLdouble,std::nullptr_t>::type>::type>::type>::type>::type>::type>::type>::type;
				static_assert(!std::is_same<type,std::nullptr_t>::value, "Invalid Enum");
			};

		constexpr std::size_t getSizeof(GLenum TypeEnum)
		{
			return
				(TypeEnum == GL_BYTE) ? sizeof(GLbyte) :
				(TypeEnum == GL_UNSIGNED_BYTE) ? sizeof(GLubyte) :
				(TypeEnum == GL_SHORT) ? sizeof(GLshort) :
				(TypeEnum == GL_UNSIGNED_SHORT) ? sizeof(GLushort) :
				(TypeEnum == GL_INT) ? sizeof(GLint) :
				(TypeEnum == GL_UNSIGNED_INT) ? sizeof(GLuint) :
				(TypeEnum == GL_FLOAT) ? sizeof(GLfloat) :
				(TypeEnum == GL_DOUBLE) ? sizeof(GLdouble) : 0;
		}



		/**
		 * GLObject Initialize and Finalize destruct
		 *
		 */


		struct Begin
		{
		};
		/*
			struct MakeCurrent
			{
			SDL_Window* m_window;
			MakeCurrent(SDL_Window* window):m_window(window){};
			};
			struct End{};

*/



		//available alloc type
		struct Alloc_Shader {};
		struct Alloc_ShaderProg {};
		struct Alloc_VertexBuffer {};
		struct Alloc_VertexArray {};
		struct Alloc_Texture {};



		//alloc traits
		template<typename T>
			struct GLAllocTraits{};

		// note:
		// these traits must have two function object named "allocfunc" and "deallocfunc".
		// the return type of allocfunc must be GLuint.

		template<>
			struct GLAllocTraits<Alloc_Shader>
			{
				constexpr static auto& allocfunc = glCreateShader; 
				constexpr static auto& deallocfunc = glDeleteShader; 
			};

		template<>
			struct GLAllocTraits<Alloc_ShaderProg>
			{
				constexpr static auto& allocfunc = glCreateProgram; 
				constexpr static auto& deallocfunc = glDeleteProgram; 
			};

		template<>
			struct GLAllocTraits<Alloc_VertexBuffer>
			{
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

				using allocfunc_t = GLuint(*)(void);
				using deallocfunc_t = void(*)(GLuint);

				constexpr static allocfunc_t allocfunc = &my_glGenBuffers; 
				constexpr static deallocfunc_t deallocfunc = &my_glDeleteBuffers; 
			};

		template<>
			struct GLAllocTraits<Alloc_VertexArray>
			{

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

				using allocfunc_t = GLuint(*)(void);
				using deallocfunc_t = void(*)(GLuint);

				constexpr static allocfunc_t allocfunc = &my_glGenVertexArrays; 
				constexpr static deallocfunc_t deallocfunc = &my_glDeleteVertexArrays; 
			};

		template<>
			struct GLAllocTraits<Alloc_Texture>
			{

				static GLuint my_glGenTextures()
				{
					GLuint id;
					glGenTextures(1, &id);
					return id;
				}

				static void my_glDeleteTextures(GLuint id)
				{
					glDeleteTextures(1, &id);
				}

				using allocfunc_t = GLuint(*)(void);
				using deallocfunc_t = void(*)(GLuint);

				constexpr static allocfunc_t allocfunc = &my_glGenTextures; 
				constexpr static deallocfunc_t deallocfunc = &my_glDeleteTextures; 
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
			constexpr static GLenum SHADER_TYPE = GL_VERTEX_SHADER;
		};
		struct FragmentShader
		{
			constexpr static GLenum SHADER_TYPE = GL_FRAGMENT_SHADER;
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

		/**
		 * setUniform
		 *
		 */

		template<std::size_t size, typename Type>
			struct glUniformXt{};
		template<>
			struct glUniformXt<1,GLint>
			{
				constexpr static auto& func = glUniform1i;
			};
		template<>
			struct glUniformXt<2,GLint>
			{
				constexpr static auto& func = glUniform2i;
			};
		template<>
			struct glUniformXt<3,GLint>
			{
				constexpr static auto& func = glUniform3i;
			};
		template<>
			struct glUniformXt<4,GLint>
			{
				constexpr static auto& func = glUniform4i;
			};
		template<>
			struct glUniformXt<1,GLfloat>
			{
				constexpr static auto& func = glUniform1f;
			};
		template<>
			struct glUniformXt<2,GLfloat>
			{
				constexpr static auto& func = glUniform2f;
			};
		template<>
			struct glUniformXt<3,GLfloat>
			{
				constexpr static auto& func = glUniform3f;
			};
		template<>
			struct glUniformXt<4,GLfloat>
			{
				constexpr static auto& func = glUniform4f;
			};


		template<std::size_t size, typename Type>
			struct glUniformXtv{};
		template<>
			struct glUniformXtv<1,GLint>
			{
				constexpr static auto& func = glUniform1iv;
			};
		template<>
			struct glUniformXtv<2,GLint>
			{
				constexpr static auto& func = glUniform2iv;
			};
		template<>
			struct glUniformXtv<3,GLint>
			{
				constexpr static auto& func = glUniform3iv;
			};
		template<>
			struct glUniformXtv<4,GLint>
			{
				constexpr static auto& func = glUniform4iv;
			};
		template<>
			struct glUniformXtv<1,GLfloat>
			{
				constexpr static auto& func = glUniform1fv;
			};
		template<>
			struct glUniformXtv<2,GLfloat>
			{
				constexpr static auto& func = glUniform2fv;
			};
		template<>
			struct glUniformXtv<3,GLfloat>
			{
				constexpr static auto& func = glUniform3fv;
			};
		template<>
			struct glUniformXtv<4,GLfloat>
			{
				constexpr static auto& func = glUniform4fv;
			};


		template<std::size_t size, typename Type>
			struct glUniformMatrixXtv{};
		template<>
			struct glUniformMatrixXtv<2,GLfloat>
			{
				constexpr static auto& func = glUniformMatrix2fv;
			};
		template<>
			struct glUniformMatrixXtv<3,GLfloat>
			{
				constexpr static auto& func = glUniformMatrix3fv;
			};
		template<>
			struct glUniformMatrixXtv<4,GLfloat>
			{
				constexpr static auto& func = glUniformMatrix4fv;
			};

		/**
		 * render mode
		 *
		 */

		struct rm_Triangles
		{
			constexpr static GLenum RENDER_MODE = GL_TRIANGLES; 
		};

		struct rm_LineLoop
		{
			constexpr static GLenum RENDER_MODE = GL_LINE_LOOP;
		};


		/**
		 * texture target type
		 *
		 */

		struct Texture2D
		{
			constexpr static GLenum TEXTURE_TARGET = GL_TEXTURE_2D;
		};

		struct Texture3D
		{
			constexpr static GLenum TEXTURE_TARGET = GL_TEXTURE_3D;
		};

		struct TextureCubeMap
		{
			constexpr static GLenum TEXTURE_TARGET = GL_TEXTURE_CUBE_MAP;
			constexpr static GLenum TEXTURE_NEGX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			constexpr static GLenum TEXTURE_POSX = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			constexpr static GLenum TEXTURE_NEGY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			constexpr static GLenum TEXTURE_POSY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			constexpr static GLenum TEXTURE_NEGZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			constexpr static GLenum TEXTURE_POSZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		};

		/**
		 * texture unit
		 *
		 */

		template<std::size_t i>
			struct TextureUnit
			{
				static_assert(0<=i && i<32, "invalid i");
				constexpr static GLenum TEXTURE_UNIT = GL_TEXTURE0 + i;
				constexpr static GLuint TEXTURE_UNIT_NUM = i;
			};
		

		/**
		 * GL_RGB GL_RGBA
		 *
		 */

		struct RGB
		{
			constexpr static GLenum TEXTURE_COLOR = GL_RGB;
			constexpr static std::size_t ALIGN = 1;
			constexpr static ILenum IL_COLOR = IL_RGB;
		};

		struct RGBA
		{
			constexpr static GLenum TEXTURE_COLOR = GL_RGBA;
			constexpr static std::size_t ALIGN = 4;
			constexpr static ILenum IL_COLOR = IL_RGBA;
		};

		/**
		 * TexImage
		 *
		 */

		template<std::size_t Dim>
			struct TexImage_D {};
		template<>
			struct TexImage_D<1>
			{
				constexpr static auto& func = glTexImage1D;
			};
		template<>
			struct TexImage_D<2>
			{
				constexpr static auto& func = glTexImage2D;
			};
		template<>
			struct TexImage_D<3>
			{
				constexpr static auto& func = glTexImage3D;
			};

		/**
		 * TextureTraits
		 *
		 */

		template<typename TargetType, GLint level, typename int_format, typename format>
			struct TextureTraits
			{
					static void texImage2D(const std::string &path)
					{
						ILuint imgID;
						ilGenImages(1, &imgID);
						ilBindImage(imgID);
						ILboolean success = ilLoadImage(path.c_str());
						if(success == IL_TRUE)
						{
							success = ilConvertImage(format::IL_COLOR, IL_UNSIGNED_BYTE);
							if(success == IL_TRUE)
							{
								//texture load
								glPixelStorei(GL_UNPACK_ALIGNMENT, format::ALIGN);
								CHECK_GL_ERROR;
								TexImage_D<2>::func(TargetType::TEXTURE_TARGET, level, int_format::TEXTURE_COLOR, (GLuint)ilGetInteger(IL_IMAGE_WIDTH),(GLuint)ilGetInteger(IL_IMAGE_HEIGHT), 0, format::TEXTURE_COLOR, GL_UNSIGNED_BYTE, static_cast<GLubyte*>(ilGetData()));
								CHECK_GL_ERROR;
							}
						}
						else
						{
							std::cerr << "cannot load image! --did nothing" << std::endl;
						}
						ilDeleteImages(1, &imgID);
					}
			};

		
		template<GLint level, typename int_format,typename format>
			struct TextureTraits<TextureCubeMap, level, int_format, format>
			{
					static void texImage2D(
							const std::string &neg_x,
							const std::string &pos_x,
							const std::string &neg_y,
							const std::string &pos_y,
							const std::string &neg_z,
							const std::string &pos_z)
					{
						ILuint imgID;
						ilGenImages(1, &imgID);
						ilBindImage(imgID);
						ILboolean success;

						//NEG_X
						success = ilLoadImage(neg_x.c_str());
						if(success == IL_TRUE)
						{
							success = ilConvertImage(format::IL_COLOR, IL_UNSIGNED_BYTE);
							if(success == IL_TRUE)
							{
								//texture load
								glPixelStorei(GL_UNPACK_ALIGNMENT, format::ALIGN);
								CHECK_GL_ERROR;
								TexImage_D<2>::func(TextureCubeMap::TEXTURE_NEGX, level, int_format::TEXTURE_COLOR, (GLuint)ilGetInteger(IL_IMAGE_WIDTH),(GLuint)ilGetInteger(IL_IMAGE_HEIGHT), 0, format::TEXTURE_COLOR, GL_UNSIGNED_BYTE, static_cast<GLubyte*>(ilGetData()));
								CHECK_GL_ERROR;
							}
						}
						else
						{
							std::cerr << "cannot load image! --did nothing" << std::endl;
						}

						//POS_X
						success = ilLoadImage(pos_x.c_str());
						if(success == IL_TRUE)
						{
							success = ilConvertImage(format::IL_COLOR, IL_UNSIGNED_BYTE);
							if(success == IL_TRUE)
							{
								//texture load
								glPixelStorei(GL_UNPACK_ALIGNMENT, format::ALIGN);
								CHECK_GL_ERROR;
								TexImage_D<2>::func(TextureCubeMap::TEXTURE_POSX, level, int_format::TEXTURE_COLOR, (GLuint)ilGetInteger(IL_IMAGE_WIDTH),(GLuint)ilGetInteger(IL_IMAGE_HEIGHT), 0, format::TEXTURE_COLOR, GL_UNSIGNED_BYTE, static_cast<GLubyte*>(ilGetData()));
								CHECK_GL_ERROR;
							}
						}
						else
						{
							std::cerr << "cannot load image! --did nothing" << std::endl;
						}

						//NEG_Y
						success = ilLoadImage(neg_y.c_str());
						if(success == IL_TRUE)
						{
							success = ilConvertImage(format::IL_COLOR, IL_UNSIGNED_BYTE);
							if(success == IL_TRUE)
							{
								//texture load
								glPixelStorei(GL_UNPACK_ALIGNMENT, format::ALIGN);
								CHECK_GL_ERROR;
								TexImage_D<2>::func(TextureCubeMap::TEXTURE_NEGY, level, int_format::TEXTURE_COLOR, (GLuint)ilGetInteger(IL_IMAGE_WIDTH),(GLuint)ilGetInteger(IL_IMAGE_HEIGHT), 0, format::TEXTURE_COLOR, GL_UNSIGNED_BYTE, static_cast<GLubyte*>(ilGetData()));
								CHECK_GL_ERROR;
							}
						}
						else
						{
							std::cerr << "cannot load image! --did nothing" << std::endl;
						}

						//POS_Y
						success = ilLoadImage(pos_y.c_str());
						if(success == IL_TRUE)
						{
							success = ilConvertImage(format::IL_COLOR, IL_UNSIGNED_BYTE);
							if(success == IL_TRUE)
							{
								//texture load
								glPixelStorei(GL_UNPACK_ALIGNMENT, format::ALIGN);
								CHECK_GL_ERROR;
								TexImage_D<2>::func(TextureCubeMap::TEXTURE_POSY, level, int_format::TEXTURE_COLOR, (GLuint)ilGetInteger(IL_IMAGE_WIDTH),(GLuint)ilGetInteger(IL_IMAGE_HEIGHT), 0, format::TEXTURE_COLOR, GL_UNSIGNED_BYTE, static_cast<GLubyte*>(ilGetData()));
								CHECK_GL_ERROR;
							}
						}
						else
						{
							std::cerr << "cannot load image! --did nothing" << std::endl;
						}

						//NEG_Z
						success = ilLoadImage(neg_z.c_str());
						if(success == IL_TRUE)
						{
							success = ilConvertImage(format::IL_COLOR, IL_UNSIGNED_BYTE);
							if(success == IL_TRUE)
							{
								//texture load
								glPixelStorei(GL_UNPACK_ALIGNMENT, format::ALIGN);
								CHECK_GL_ERROR;
								TexImage_D<2>::func(TextureCubeMap::TEXTURE_NEGZ, level, int_format::TEXTURE_COLOR, (GLuint)ilGetInteger(IL_IMAGE_WIDTH),(GLuint)ilGetInteger(IL_IMAGE_HEIGHT), 0, format::TEXTURE_COLOR, GL_UNSIGNED_BYTE, static_cast<GLubyte*>(ilGetData()));
								CHECK_GL_ERROR;
							}
						}
						else
						{
							std::cerr << "cannot load image! --did nothing" << std::endl;
						}

						//POS_Z
						success = ilLoadImage(pos_z.c_str());
						if(success == IL_TRUE)
						{
							success = ilConvertImage(format::IL_COLOR, IL_UNSIGNED_BYTE);
							if(success == IL_TRUE)
							{
								//texture load
								glPixelStorei(GL_UNPACK_ALIGNMENT, format::ALIGN);
								CHECK_GL_ERROR;
								TexImage_D<2>::func(TextureCubeMap::TEXTURE_POSZ, level, int_format::TEXTURE_COLOR, (GLuint)ilGetInteger(IL_IMAGE_WIDTH),(GLuint)ilGetInteger(IL_IMAGE_HEIGHT), 0, format::TEXTURE_COLOR, GL_UNSIGNED_BYTE, static_cast<GLubyte*>(ilGetData()));
								CHECK_GL_ERROR;
							}
						}
						else
						{
							std::cerr << "cannot load image! --did nothing" << std::endl;
						}


						ilDeleteImages(1, &imgID);
					}

			};

		/**
		 * Texture parameter
		 *
		 */
		template<GLenum param>
			struct Wrap_S{
				static_assert((param == GL_CLAMP_TO_EDGE)||( param == GL_REPEAT), "invalid param");
				static void setTextureParameter(GLenum target)
				{
					glTexParameteri(target, GL_TEXTURE_WRAP_S, param);
					CHECK_GL_ERROR;
				}
			};
		template<GLenum param>
			struct Wrap_T{
				static_assert((param == GL_CLAMP_TO_EDGE)||(param == GL_REPEAT), "invalid param");
				static void setTextureParameter(GLenum target)
				{
					glTexParameteri(target, GL_TEXTURE_WRAP_T, param);
					CHECK_GL_ERROR;
				}
			};
		template<GLenum param>
			struct Wrap_R{
				static_assert((param == GL_CLAMP_TO_EDGE)||(param == GL_REPEAT), "invalid param");
				static void setTextureParameter(GLenum target)
				{
					glTexParameteri(target, GL_TEXTURE_WRAP_R, param);
					CHECK_GL_ERROR;
				}
			};
		template<GLenum param>
			struct Mag_Filter{
				static_assert((param == GL_NEAREST)||(param == GL_LINEAR), "invalid param");
				static void setTextureParameter(GLenum target)
				{
					glTexParameteri(target, GL_TEXTURE_MAG_FILTER, param);
					CHECK_GL_ERROR;
				}
			};
		template<GLenum param>
			struct Min_Filter{
				static_assert((param == GL_NEAREST)||(param == GL_LINEAR), "invalid param");
				static void setTextureParameter(GLenum target)
				{
					glTexParameteri(target, GL_TEXTURE_MIN_FILTER, param);
					CHECK_GL_ERROR;
				}
			};


		/**
		 * Texture parameter helper
		 *
		 */

		template<typename First, typename... Args>
			struct SetParamTraits
			{
				inline static void func(GLenum target)
				{
					First::setTextureParameter(target);
					SetParamTraits<Args...>::func(target);
				}
			};

		template<typename Last>
			struct SetParamTraits<Last>
			{
				inline static void func(GLenum target)
				{
					Last::setTextureParameter(target);
				}
			};

		/**
		 * Mesh Sample
		 *
		 */

		// Cube
		//  
		
		/*
		struct unitCube{
			constexpr static GLfloat vertex[][3] =
			{
				{ 1.0/2.0,  1.0/2.0,  1.0/2.0},
				{-1.0/2.0,  1.0/2.0,  1.0/2.0},
				{-1.0/2.0, -1.0/2.0,  1.0/2.0},
				{ 1.0/2.0, -1.0/2.0,  1.0/2.0},
				{ 1.0/2.0,  1.0/2.0, -1.0/2.0},
				{-1.0/2.0,  1.0/2.0, -1.0/2.0},
				{-1.0/2.0, -1.0/2.0, -1.0/2.0},
				{ 1.0/2.0, -1.0/2.0, -1.0/2.0},
			};vertex

			constexpr static GLfloat normal[][3] = 
			{
				{ 0.57735,  0.57735,  0.57735},
				{-0.57735,  0.57735,  0.57735},
				{-0.57735, -0.57735,  0.57735},
				{ 0.57735, -0.57735,  0.57735},
				{ 0.57735,  0.57735, -0.57735},
				{-0.57735,  0.57735, -0.57735},
				{-0.57735, -0.57735, -0.57735},
				{ 0.57735, -0.57735, -0.57735},
			};

			constexpr static GLshort index[] = 
			{
				0,1,2,0,2,3,
				3,2,6,3,6,7,
				0,3,7,0,7,4,
				1,0,4,1,4,5,
				2,1,5,2,5,6,
				7,6,5,7,5,4
			};
		};
		*/

		
	}
}
