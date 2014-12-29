#pragma once

#include <GL/glew.h>
#include <IL/il.h>
#include <IL/ilu.h>

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <tuple>
#include <functional>
#include "gl_helper.h"

namespace jikoLib{
	namespace GLLib {

		//getID
		template<typename T>
			inline GLuint getID(const T &obj)
			{
				return obj.getID();
			}

		// class forward declaration

		template<typename Shader_type, typename Allocator>
			class Shader;
		template<typename Allocator> 
			class ShaderProg;
		template<typename TargetType, typename UsageType, typename Allocator>
			class VertexBuffer;
		template<typename Allocator> 
			class VertexArray;



		/*
		 *
		 *    gl_base
		 *
		 *
		 *
		 *
		 */



		//shader
		template<typename Shader_type, typename Allocator = GLAllocator<Alloc_Shader>>
			class Shader
			{
				private:
					GLuint shader_id;
					bool is_compiled = false;
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
						CHECK_GL_ERROR;
						DEBUG_OUT("shader id " << shader_id << " destructed!");
					}

					Shader(const Shader<Shader_type, Allocator> &obj)
					{
						this->shader_id = obj.shader_id;
						this->is_compiled = obj.is_compiled;

						a.copy(obj.a);
						DEBUG_OUT("shader copied! shader_id is " << shader_id);
						CHECK_GL_ERROR;
					}

					Shader(Shader<Shader_type, Allocator>&& obj)
					{
						this->shader_id = obj.shader_id;
						this->is_compiled = obj.is_compiled;

						a.move(std::move(obj.a));
						DEBUG_OUT("shader moved! shader_id is " << shader_id);
						CHECK_GL_ERROR;
					}

					Shader& operator=(const Shader<Shader_type, Allocator> &obj)
					{
						a.destruct(shader_id);
						this->shader_id = obj.shader_id;
						this->is_compiled = obj.is_compiled;

						a.copy(obj.a);
						CHECK_GL_ERROR;
						DEBUG_OUT("shader copied! shader_id is " << shader_id);
						return *this;
					}

					Shader& operator=(Shader<Shader_type, Allocator>&& obj)
					{
						a.destruct(shader_id);
						this->shader_id = obj.shader_id;
						this->is_compiled = obj.is_compiled;

						a.move(std::move(obj.a));
						CHECK_GL_ERROR;
						DEBUG_OUT("shader moved! shader_id is " << shader_id);
						return *this;
					}

					Shader& operator<<(const std::string& str)
						//read shader source and compile
					{
						const char *source = str.c_str();
						const int length = str.length();
						glShaderSource(shader_id, 1, (const GLchar**)&source, &length);
						CHECK_GL_ERROR;
						glCompileShader(shader_id);
						CHECK_GL_ERROR;

						GLint compiled, size;
						GLsizei len;
						char* buf = nullptr;
						glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
						CHECK_GL_ERROR;

						if(compiled == GL_FALSE)
						{
							//compile failed
							std::cerr << "id " << shader_id << " Compile Failed!: " << std::endl;
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
					bool isLinked = false;
					Allocator a;
				public:
					inline void bind() const
					{
						glUseProgram(shaderprog_id);
						CHECK_GL_ERROR;
					}

					inline void unbind() const
					{
						glUseProgram(0);
						CHECK_GL_ERROR;
					}

					ShaderProg()
					{
						shaderprog_id = a.construct();
						CHECK_GL_ERROR;
						DEBUG_OUT("shaderprog created! shaderprog id is " << shaderprog_id);
					}

					~ShaderProg()
					{
						a.destruct(shaderprog_id);
						CHECK_GL_ERROR;
						DEBUG_OUT("shaderprog id " << shaderprog_id << " destructed!");
					}

					ShaderProg(const ShaderProg<Allocator> &obj)
					{
						this->shaderprog_id = obj.shaderprog_id;
						this->isLinked = obj.isLinked;

						a.copy(obj.a);
						CHECK_GL_ERROR;
						DEBUG_OUT("shaderprog copied! shaderprog id is " << shaderprog_id);
					}

					ShaderProg(ShaderProg<Allocator>&& obj)
					{
						this->shaderprog_id = obj.shaderprog_id;
						this->isLinked = obj.isLinked;

						a.move(std::move(obj.a));
						CHECK_GL_ERROR;
						DEBUG_OUT("shaderprog moved! shaderprog id is " << shaderprog_id);
					}

					ShaderProg& operator=(const ShaderProg<Allocator> &obj)
					{
						a.destruct(shaderprog_id);
						this->shaderprog_id = obj.shaderprog_id;
						this->isLinked = obj.isLinked;

						a.copy(obj.a);
						CHECK_GL_ERROR;
						DEBUG_OUT("shaderprog copied! shaderprog id is " << shaderprog_id);
					}

					ShaderProg& operator=(ShaderProg<Allocator>&& obj)
					{
						a.destruct(shaderprog_id);
						this->shaderprog_id = obj.shaderprog_id;
						this->isLinked = obj.isLinked;

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
							std::cerr << "id "<< shaderprog_id <<" Link Failed!: " << std::endl;
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


					//select appropriate glUniform function
					template <typename... ArgTypes>
						void setUniformXt(const std::string &str, ArgTypes... args)
						{
							//for glUniformXi, glUniformXf
							using first_type = typename std::tuple_element<0, std::tuple<ArgTypes...>>::type;

							static_assert(is_all_same<first_type, ArgTypes...>::value, "ArgTypes must be all same");
							static_assert(is_exist<first_type, GLint, GLfloat>::value, "ArgType must be GLint or GLfloat");
							static_assert((1 <= sizeof...(ArgTypes))&&(sizeof...(ArgTypes) <= 4), "invalid ArgTypes Num");
							bind();

							auto loc = glGetUniformLocation(shaderprog_id, str.c_str());
							CHECK_GL_ERROR;
							if(loc == -1)
							{
								std::cerr << "uniform variable " << str << " cannot be found" << std::endl;
								return;
							}

							glUniformXt<sizeof...(ArgTypes), first_type>::func(loc, args...);
							CHECK_GL_ERROR;
							unbind();
						}


					template <std::size_t Size_Elem, std::size_t Dim, typename T>
						void setUniformXtv(const std::string &str, const T (&array)[Size_Elem][Dim])
						{
							//for glUniformXiv, glUniformXfv
							static_assert((1 <= Dim)&&(Dim <= 4), "invalid Dim");
							static_assert(is_exist<T,GLint, GLfloat>::value,"array type must be GLint for GLfloat");
							bind();

							auto loc = glGetUniformLocation(shaderprog_id, str.c_str());
							CHECK_GL_ERROR;
							if(loc == -1)
							{
								std::cerr << "uniform variable " << str << " cannot be found" << std::endl;
								return;
							}

							glUniformXtv<Dim, T>::func(loc, Size_Elem, array);
							CHECK_GL_ERROR;
							unbind();
						}

					template <std::size_t Dim, typename T>
						void setUniformXtv(const std::string &str, const T (&array)[Dim])
						{
							//for glUniformXiv, glUniformXfv
							static_assert((1 <= Dim)&&(Dim <= 4), "invalid Dim");
							static_assert(is_exist<T,GLint, GLfloat>::value,"array type must be GLint for GLfloat");
							bind();

							auto loc = glGetUniformLocation(shaderprog_id, str.c_str());
							CHECK_GL_ERROR;
							if(loc == -1)
							{
								std::cerr << "uniform variable " << str << " cannot be found" << std::endl;
								return;
							}

							glUniformXtv<Dim, T>::func(loc, 1, array);
							CHECK_GL_ERROR;
							unbind();
						}


					template<typename T>
						void setUniformXtv(const std::string &str, const T *array, std::size_t Size_Elem, std::size_t Dim = 1)
						{
							static_assert(is_exist<T,GLint, GLfloat>::value,"array type must be GLint for GLfloat");
							bind();
							auto loc = glGetUniformLocation(shaderprog_id, str.c_str());
							CHECK_GL_ERROR;
							if(loc == -1)
							{
								std::cerr << "uniform variable " << str << " cannot be found" << std::endl;
								return;
							}
							switch (Dim) {
								case 1:
									glUniformXtv<1, T>::func(loc, Size_Elem, array);
									break;
								case 2:
									glUniformXtv<2, T>::func(loc, Size_Elem, array);
									break;
								case 3:
									glUniformXtv<3, T>::func(loc, Size_Elem, array);
									break;
								case 4:
									glUniformXtv<4, T>::func(loc, Size_Elem, array);
									break;
								default:
									std::cerr << "invalid Dim Number --did nothing." << std::endl;
									break;
							}
							unbind();

						}

					template<std::size_t Size_Elem, std::size_t Dim, typename T>
						void setUniformMatrixXtv(const std::string &str, const T (&array)[Size_Elem][Dim][Dim])
						{
							static_assert(is_exist<T, GLfloat>::value, "array must be GLfloat.");
							static_assert((2 <= Dim)&&(Dim <= 4), "invalid Dim");

							bind();
							auto loc = glGetUniformLocation(shaderprog_id, str.c_str());
							CHECK_GL_ERROR;
							if(loc == -1)
							{
								std::cerr << "uniform variable " << str << " cannot be found" << std::endl;
								return;
							}
							glUniformMatrixXtv<Dim, T>::func(loc, Size_Elem, GL_FALSE, array);
							unbind();
						}

					template<std::size_t Dim, typename T>
						void setUniformMatrixXtv(const std::string &str, const T (&array)[Dim][Dim])
						{
							static_assert(is_exist<T, GLfloat>::value, "array must be GLfloat.");
							static_assert((2 <= Dim)&&(Dim <= 4), "invalid Dim");

							bind();
							auto loc = glGetUniformLocation(shaderprog_id, str.c_str());
							CHECK_GL_ERROR;
							if(loc == -1)
							{
								std::cerr << "uniform variable " << str << " cannot be found" << std::endl;
								return;
							}
							glUniformMatrixXtv<Dim, T>::func(loc, 1, GL_FALSE, array);
							unbind();
						}

					template<typename T>
						void setUniformMatrixXtv(const std::string &str, const T *array, std::size_t Size_Elem, std::size_t Dim)
						{
							static_assert(is_exist<T, GLfloat>::value, "array must be GLfloat.");
							bind();
							auto loc = glGetUniformLocation(shaderprog_id, str.c_str());
							CHECK_GL_ERROR;
							if(loc == -1)
							{
								std::cerr << "uniform variable " << str << " cannot be found" << std::endl;
								return;
							}
							switch(Dim)
							{
								case 2:
									glUniformMatrixXtv<2, T>::func(loc, Size_Elem, GL_FALSE, array);
									break;
								case 3:
									glUniformMatrixXtv<3, T>::func(loc, Size_Elem, GL_FALSE, array);
									break;
								case 4:
									glUniformMatrixXtv<4, T>::func(loc, Size_Elem, GL_FALSE, array);
									break;
								default:
									std::cerr << "invalid Dim Number --did nothing." << std::endl;
									break;
							}
							unbind();
						}

			};


		//vertexbuffer
		template<typename TargetType, typename UsageType, typename Allocator = GLAllocator<Alloc_VertexBuffer>>
			class VertexBuffer
			{
				private:
					GLuint buffer_id;

					bool isSetArray = false; 
					GLenum ArrayEnum;
					std::size_t Size_Elem;
					std::size_t Dim;

					Allocator a;

					template<typename Type> 
						inline void setSizeElem_Dim_Type(std::size_t Size_Elem, std::size_t Dim)
						{
							this->Size_Elem = Size_Elem;
							this->Dim = Dim;
							this->ArrayEnum = getEnum<Type>::value;
							isSetArray = true;
						}

				public:

					inline bool getisSetArray() const
					{
						return isSetArray;
					}
					inline GLenum getArrayEnum() const
					{
						return ArrayEnum;
					}
					inline std::size_t getSizeElem() const
					{
						return Size_Elem;
					}
					inline std::size_t getDim() const
					{
						return Dim;
					}

					inline void bind() const
					{
						glBindBuffer(TargetType::BUFFER_TARGET, buffer_id);
						CHECK_GL_ERROR;
					}

					inline void unbind() const
					{
						glBindBuffer(TargetType::BUFFER_TARGET, 0);
						CHECK_GL_ERROR;
					}

					VertexBuffer()
					{
						buffer_id = a.construct();
						CHECK_GL_ERROR;
						bind();
						DEBUG_OUT("vbuffer created! id is " << buffer_id);
						unbind();
					}

					~VertexBuffer()
					{
						a.destruct(buffer_id);
						CHECK_GL_ERROR;
						DEBUG_OUT("vbuffer id " << buffer_id << " destructed!");
					}

					VertexBuffer(const VertexBuffer<TargetType, UsageType, Allocator> &obj)
					{
						this->buffer_id = obj.buffer_id;
						this->isSetArray = obj.isSetArray;
						this->ArrayEnum = obj.ArrayEnum;
						this->Size_Elem = obj.Size_Elem;
						this->Dim = obj.Dim;

						a.copy(obj.a);
						CHECK_GL_ERROR;
						DEBUG_OUT("vbuffer copied! id is " << buffer_id);
					}

					VertexBuffer(VertexBuffer<TargetType, UsageType, Allocator>&& obj)
					{
						this->buffer_id = obj.buffer_id;
						this->isSetArray = obj.isSetArray;
						this->ArrayEnum = obj.ArrayEnum;
						this->Size_Elem = obj.Size_Elem;
						this->Dim = obj.Dim;

						a.move(std::move(obj.a));
						CHECK_GL_ERROR;
						DEBUG_OUT("vbuffer moved! id is " << buffer_id);
					}

					VertexBuffer& operator=(const VertexBuffer<TargetType, UsageType, Allocator> &obj)
					{
						a.destruct(buffer_id);
						this->buffer_id = obj.buffer_id;
						this->isSetArray = obj.isSetArray;
						this->ArrayEnum = obj.ArrayEnum;
						this->Size_Elem = obj.Size_Elem;
						this->Dim = obj.Dim;

						a.copy(obj.a);
						CHECK_GL_ERROR;
						DEBUG_OUT("vbuffer copied! id is " << buffer_id);
						return *this;
					}

					VertexBuffer& operator=(VertexBuffer<TargetType, UsageType, Allocator>&& obj)
					{
						a.destruct(buffer_id);
						this->buffer_id = obj.buffer_id;
						this->isSetArray = obj.isSetArray;
						this->ArrayEnum = obj.ArrayEnum;
						this->Size_Elem = obj.Size_Elem;
						this->Dim = obj.Dim;

						a.move(std::move(obj.a));
						CHECK_GL_ERROR;
						DEBUG_OUT("vbuffer moved! id is " << buffer_id);
						return *this;
					}

					inline GLuint getID() const
					{
						return buffer_id;
					}


					/*
					 * invalid code
					 *
					 template<typename T,std::size_t Size_Elem, std::size_t Dim>
					 VertexBuffer& operator<<(const std::array<std::array<T, Dim>, Size_Elem> &array)
					// Set Array for std::array 
					{
					static_assert( is_exist<T, GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat, GLdouble>::value, "Invalid type" );
					static_assert( Dim <= 4, "Invalid dimension" );
					static_assert( (Size_Elem != 0)&&(Dim != 0), "Zero Elem" );
					static_assert((!std::is_same<TargetType,ElementArrayBuffer>::value)||((std::is_same<TargetType,ElementArrayBuffer>::value)&&(is_exist<T,GLubyte,GLushort,GLuint>::value)),
					"IBO array type must be GLushort or GLuint or GLubyte");
					bind();
					glBufferData(TargetType::BUFFER_TARGET, Size_Elem*Dim*sizeof(T), array.data(), UsageType::BUFFER_USAGE);
					CHECK_GL_ERROR;
					DEBUG_OUT("allocate "<< Size_Elem*Dim*sizeof(T) <<" B success! buffer id is " << buffer_id);
					setSizeElem_Dim_Type<T>(Size_Elem, Dim);
					return *this;
					}

*/

					template<typename T,std::size_t Size_Elem, std::size_t Dim>
						VertexBuffer& operator<<(const T (&array)[Size_Elem][Dim])
						// Set Array for raw array
						{
							static_assert( is_exist<T, GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat, GLdouble>::value, "Invalid type" );
							static_assert( Dim <= 4, "Invalid dimension" );
							static_assert( (Size_Elem != 0)&&(Dim != 0), "Zero Elem" );
							static_assert((!std::is_same<TargetType,ElementArrayBuffer>::value)||((std::is_same<TargetType,ElementArrayBuffer>::value)&&(is_exist<T,GLubyte,GLushort,GLuint>::value)),
									"IBO array type must be GLushort or GLuint or GLubyte");
							bind();
							glBufferData(TargetType::BUFFER_TARGET, Size_Elem*Dim*sizeof(T), array, UsageType::BUFFER_USAGE);
							CHECK_GL_ERROR;
							DEBUG_OUT("allocate "<< Size_Elem*Dim*sizeof(T) <<" B success! buffer id is " << buffer_id);
							setSizeElem_Dim_Type<T>(Size_Elem, Dim);
							unbind();
							return *this;
						}

					template<typename T>
						void copyData(const T* array, std::size_t Size_Elem, std::size_t Dim = 1)
						{
							static_assert( is_exist<T, GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat, GLdouble>::value, "Invalid type" );
							static_assert((!std::is_same<TargetType,ElementArrayBuffer>::value)||((std::is_same<TargetType,ElementArrayBuffer>::value)&&(is_exist<T,GLubyte,GLushort,GLuint>::value)),
									"IBO array type must be GLushort or GLuint or GLubyte");
							bind();
							glBufferData(TargetType::BUFFER_TARGET, Size_Elem*Dim*sizeof(T), array, UsageType::BUFFER_USAGE);
							CHECK_GL_ERROR;
							DEBUG_OUT("allocate "<< Size_Elem*Dim*sizeof(T) <<" B success! buffer id is " << buffer_id);
							setSizeElem_Dim_Type<T>(Size_Elem, Dim);
							unbind();
						}

					template<typename T,std::size_t Size_Elem, std::size_t Dim>
						inline void copyData(const T (&array)[Size_Elem][Dim])
						{
							this << array;
						}

					
					template<typename T, std::size_t Size_Elem>
						VertexBuffer& operator<<(const T (&array)[Size_Elem])
						//Set Array for raw array
						{
							static_assert( is_exist<T, GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat, GLdouble>::value, "Invalid type" );
							static_assert( (Size_Elem != 0), "Zero Elem" );
							static_assert((!std::is_same<TargetType,ElementArrayBuffer>::value)||((std::is_same<TargetType,ElementArrayBuffer>::value)&&(is_exist<T,GLubyte,GLushort,GLuint>::value)),
									"IBO array type must be GLushort or GLuint or GLubyte");
							bind();
							glBufferData(TargetType::BUFFER_TARGET, Size_Elem*sizeof(T), array, UsageType::BUFFER_USAGE);
							CHECK_GL_ERROR;
							DEBUG_OUT("allocate "<< Size_Elem*sizeof(T) <<" B success! buffer id is " << buffer_id);
							setSizeElem_Dim_Type<T>(Size_Elem, 1);
							unbind();
							return *this;
						}

					template<typename T, std::size_t Size_Elem>
						inline void copyData(const T (&array)[Size_Elem])
						{
							this << array;
						}

					/*
					template<typename T>
						void copyData(const T* array, std::size_t Size_Elem)
						{
							static_assert( is_exist<T, GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat, GLdouble>::value, "Invalid type" );
							static_assert((!std::is_same<TargetType,ElementArrayBuffer>::value)||((std::is_same<TargetType,ElementArrayBuffer>::value)&&(is_exist<T,GLubyte,GLushort,GLuint>::value)),
									"IBO array type must be GLushort or GLuint or GLubyte");
							bind();
							glBufferData(TargetType::BUFFER_TARGET, Size_Elem*sizeof(T), array, UsageType::BUFFER_USAGE);
							CHECK_GL_ERROR;
							DEBUG_OUT("allocate "<< Size_Elem*sizeof(T) <<" B success! buffer id is " << buffer_id);
							setSizeElem_Dim_Type<T>(Size_Elem, 1);
						}
						*/


					/*
					 *
					 * invalid code
					 *
					 template<typename T>
					 VertexBuffer& operator<<(const std::vector<std::vector<T>> &array)
					// Set Array for std::vector
					{
					static_assert( is_exist<T, GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat, GLdouble>::value, "Invalid type" );
					static_assert((!std::is_same<TargetType,ElementArrayBuffer>::value)||((std::is_same<TargetType,ElementArrayBuffer>::value)&&(is_exist<T,GLubyte,GLushort,GLuint>::value)),
					"IBO array type must be GLushort or GLuint or GLubyte");
					std::size_t Size_Elem = 0;
					std::size_t Dim = 0;
					bool first = true;
					if((Size_Elem = array.size()) == 0)
					{
					std::cerr << "Zero Elem! --did nothing." << std::endl;
					return *this;
					}
					for (auto&& sub_array : array) {
					if(first)
					{
					first = false;
					Dim = sub_array.size();
					}
					else
					{
					if(Dim != sub_array.size())
					{
					std::cerr << "diffelent length array! --did nothing." << std::endl;
					return *this;
					}
					}
					if(sub_array.size() > 4)
					{
					std::cerr << "Invalid dimension! --did nothing" << std::endl;
					return *this;
					}
					}
					bind();
					glBufferData(TargetType::BUFFER_TARGET, Size_Elem*Dim*sizeof(T), array.data(), UsageType::BUFFER_USAGE);
					CHECK_GL_ERROR;
					DEBUG_OUT("allocate "<< Size_Elem*Dim*sizeof(T) <<" B success! buffer id is " << buffer_id);
					setSizeElem_Dim_Type<T>(Size_Elem, Dim);
					return *this;
					}

*/

					VertexBuffer operator+(const VertexBuffer<TargetType, UsageType, Allocator> &obj)
						//merge buffer data
					{
						if(!(this->isSetArray && obj.isSetArray))
						{
							std::cerr << "Array is not set. --did nothing" << std::endl;
							return *this;
						}
						if(this->ArrayEnum != obj.ArrayEnum)
						{
							std::cerr << "two types is not same --did nothing" << std::endl;
							return *this;
						}
						if(this->Dim != obj.Dim)
						{
							std::cerr << "two arrays' dimension is no same --did nothing" << std::endl;
							return *this;
						}

						//TODO: this code looks ugly.

						if(this->ArrayEnum == GL_BYTE)
						{
							std::vector<GLbyte> t_array(this->Dim*this->Size_Elem);
							std::vector<GLbyte> o_array(obj.Dim*obj.Size_Elem);
							std::vector<GLbyte> array(this->Dim*this->Size_Elem+obj.Dim*obj.Size_Elem);

							this->bind();
							GLbyte *temp_t_array = static_cast<GLbyte*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_t_array, temp_t_array+this->Dim*this->Size_Elem, t_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							this->unbind();

							obj.bind();
							GLbyte *temp_o_array = static_cast<GLbyte*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_o_array, temp_o_array+obj.Dim*obj.Size_Elem, o_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							obj.unbind();

							array.insert(array.begin(), t_array.begin(), t_array.end());
							array.insert(array.begin()+t_array.size(), o_array.begin(), o_array.end());

							VertexBuffer<TargetType, UsageType, Allocator> buffer;
							buffer.copyData(array.data(), this->Size_Elem+obj.Size_Elem, this->Dim);

							return buffer;
						}
						if(this->ArrayEnum == GL_UNSIGNED_BYTE)
						{
							std::vector<GLubyte> t_array(this->Dim*this->Size_Elem);
							std::vector<GLubyte> o_array(obj.Dim*obj.Size_Elem);
							std::vector<GLubyte> array(this->Dim*this->Size_Elem+obj.Dim*obj.Size_Elem);

							this->bind();
							GLubyte *temp_t_array = static_cast<GLubyte*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_t_array, temp_t_array+this->Dim*this->Size_Elem, t_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							this->unbind();

							obj.bind();
							GLubyte *temp_o_array = static_cast<GLubyte*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_o_array, temp_o_array+obj.Dim*obj.Size_Elem, o_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							obj.unbind();

							array.insert(array.begin(), t_array.begin(), t_array.end());
							array.insert(array.begin()+t_array.size(), o_array.begin(), o_array.end());

							VertexBuffer<TargetType, UsageType, Allocator> buffer;
							buffer.copyData(array.data(), this->Size_Elem+obj.Size_Elem, this->Dim);
							return buffer;
						}
						if(this->ArrayEnum == GL_SHORT)
						{
							std::vector<GLshort> t_array(this->Dim*this->Size_Elem);
							std::vector<GLshort> o_array(obj.Dim*obj.Size_Elem);
							std::vector<GLshort> array(this->Dim*this->Size_Elem+obj.Dim*obj.Size_Elem);

							this->bind();
							GLshort *temp_t_array = static_cast<GLshort*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_t_array, temp_t_array+this->Dim*this->Size_Elem, t_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							this->unbind();

							obj.bind();
							GLshort *temp_o_array = static_cast<GLshort*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_o_array, temp_o_array+obj.Dim*obj.Size_Elem, o_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							obj.unbind();

							array.insert(array.begin(), t_array.begin(), t_array.end());
							array.insert(array.begin()+t_array.size(), o_array.begin(), o_array.end());

							VertexBuffer<TargetType, UsageType, Allocator> buffer;
							buffer.copyData(array.data(), this->Size_Elem+obj.Size_Elem, this->Dim);
							return buffer;
						}
						if(this->ArrayEnum == GL_UNSIGNED_SHORT)
						{
							std::vector<GLushort> t_array(this->Dim*this->Size_Elem);
							std::vector<GLushort> o_array(obj.Dim*obj.Size_Elem);
							std::vector<GLushort> array(this->Dim*this->Size_Elem+obj.Dim*obj.Size_Elem);

							this->bind();
							GLushort *temp_t_array = static_cast<GLushort*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_t_array, temp_t_array+this->Dim*this->Size_Elem, t_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							this->unbind();

							obj.bind();
							GLushort *temp_o_array = static_cast<GLushort*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_o_array, temp_o_array+obj.Dim*obj.Size_Elem, o_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							obj.unbind();

							array.insert(array.begin(), t_array.begin(), t_array.end());
							array.insert(array.begin()+t_array.size(), o_array.begin(), o_array.end());

							VertexBuffer<TargetType, UsageType, Allocator> buffer;
							buffer.copyData(array.data(), this->Size_Elem+obj.Size_Elem, this->Dim);
							return buffer;
						}
						if(this->ArrayEnum == GL_INT)
						{
							std::vector<GLint> t_array(this->Dim*this->Size_Elem);
							std::vector<GLint> o_array(obj.Dim*obj.Size_Elem);
							std::vector<GLint> array(this->Dim*this->Size_Elem+obj.Dim*obj.Size_Elem);

							this->bind();
							GLint *temp_t_array = static_cast<GLint*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_t_array, temp_t_array+this->Dim*this->Size_Elem, t_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							this->unbind();

							obj.bind();
							GLint *temp_o_array = static_cast<GLint*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_o_array, temp_o_array+obj.Dim*obj.Size_Elem, o_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							obj.unbind();

							array.insert(array.begin(), t_array.begin(), t_array.end());
							array.insert(array.begin()+t_array.size(), o_array.begin(), o_array.end());

							VertexBuffer<TargetType, UsageType, Allocator> buffer;
							buffer.copyData(array.data(), this->Size_Elem+obj.Size_Elem, this->Dim);
							return buffer;
						}
						if(this->ArrayEnum == GL_UNSIGNED_INT)
						{
							std::vector<GLuint> t_array(this->Dim*this->Size_Elem);
							std::vector<GLuint> o_array(obj.Dim*obj.Size_Elem);
							std::vector<GLuint> array(this->Dim*this->Size_Elem+obj.Dim*obj.Size_Elem);

							this->bind();
							GLuint *temp_t_array = static_cast<GLuint*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_t_array, temp_t_array+this->Dim*this->Size_Elem, t_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							this->unbind();

							obj.bind();
							GLuint *temp_o_array = static_cast<GLuint*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_o_array, temp_o_array+obj.Dim*obj.Size_Elem, o_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							obj.unbind();

							array.insert(array.begin(), t_array.begin(), t_array.end());
							array.insert(array.begin()+t_array.size(), o_array.begin(), o_array.end());

							VertexBuffer<TargetType, UsageType, Allocator> buffer;
							buffer.copyData(array.data(), this->Size_Elem+obj.Size_Elem, this->Dim);
							return buffer;
						}
						if(this->ArrayEnum == GL_FLOAT)
						{
							std::vector<GLfloat> t_array(this->Dim*this->Size_Elem);
							std::vector<GLfloat> o_array(obj.Dim*obj.Size_Elem);
							std::vector<GLfloat> array(this->Dim*this->Size_Elem+obj.Dim*obj.Size_Elem);

							this->bind();
							GLfloat *temp_t_array = static_cast<GLfloat*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_t_array, temp_t_array+this->Dim*this->Size_Elem, t_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							this->unbind();

							obj.bind();
							GLfloat *temp_o_array = static_cast<GLfloat*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_o_array, temp_o_array+obj.Dim*obj.Size_Elem, o_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							obj.unbind();

							array.insert(array.begin(), t_array.begin(), t_array.end());
							array.insert(array.begin()+t_array.size(), o_array.begin(), o_array.end());

							VertexBuffer<TargetType, UsageType, Allocator> buffer;
							buffer.copyData(array.data(), this->Size_Elem+obj.Size_Elem, this->Dim);
							return buffer;
						}
						if(this->ArrayEnum == GL_DOUBLE)
						{
							std::vector<GLdouble> t_array(this->Dim*this->Size_Elem);
							std::vector<GLdouble> o_array(obj.Dim*obj.Size_Elem);
							std::vector<GLdouble> array(this->Dim*this->Size_Elem+obj.Dim*obj.Size_Elem);

							this->bind();
							GLdouble *temp_t_array = static_cast<GLdouble*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_t_array, temp_t_array+this->Dim*this->Size_Elem, t_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							this->unbind();

							obj.bind();
							GLdouble *temp_o_array = static_cast<GLdouble*>(glMapBuffer(TargetType::BUFFER_TARGET, GL_READ_ONLY));
							CHECK_GL_ERROR;
							std::copy(temp_o_array, temp_o_array+obj.Dim*obj.Size_Elem, o_array.begin());
							glUnmapBuffer(TargetType::BUFFER_TARGET);
							CHECK_GL_ERROR;
							obj.unbind();

							array.insert(array.begin(), t_array.begin(), t_array.end());
							array.insert(array.begin()+t_array.size(), o_array.begin(), o_array.end());

							VertexBuffer<TargetType, UsageType, Allocator> buffer;
							buffer.copyData(array.data(), this->Size_Elem+obj.Size_Elem, this->Dim);
							return buffer;
						}

						std::cerr << "invalid enum --did nothing" << std::endl;
						return *this;

					}
			};

		//vertexarray
		template<typename Allocator=GLAllocator<Alloc_VertexArray>> 
			class VertexArray
			{
				private:
					GLuint varray_id;
					Allocator a;

				public:

					inline void bind() const
					{
						glBindVertexArray(varray_id);
						CHECK_GL_ERROR;
					}

					inline void unbind() const
					{
						glBindVertexArray(0);
						CHECK_GL_ERROR;
					}

					template<typename IBOAlloc>
						inline void bindIBO(const VertexBuffer<ElementArrayBuffer, IBOAlloc> &ibo) const
						{
							bind();
							ibo.bind();
							unbind();
						}
					template<typename IBOAlloc>
						inline void unbindIBO(const VertexBuffer<ElementArrayBuffer, IBOAlloc> &ibo) const
						{
							bind();
							ibo.unbind();
							unbind();
						}


					VertexArray()
					{
						varray_id = a.construct();
						CHECK_GL_ERROR;
						bind();
						DEBUG_OUT("varray created! id is " << varray_id);
						unbind();
					}

					~VertexArray()
					{
						a.destruct(varray_id);
						CHECK_GL_ERROR;
						DEBUG_OUT("varray id " << varray_id << " destructed!");
					}

					VertexArray(const VertexArray<Allocator> &obj)
					{
						this->varray_id = obj.varray_id;

						a.copy(obj.a);
						CHECK_GL_ERROR;
						DEBUG_OUT("varray copied! id is " << varray_id);
					}

					VertexArray(VertexArray<Allocator>&& obj)
					{
						this->varray_id = obj.varray_id;

						a.move(std::move(obj.a));
						CHECK_GL_ERROR;
						DEBUG_OUT("varray moved! id is " << varray_id);
					}

					VertexArray& operator=(const VertexArray<Allocator> &obj)
					{
						a.destruct(varray_id);
						this->varray_id = obj.varray_id;

						a.copy(obj.a);
						CHECK_GL_ERROR;
						DEBUG_OUT("varray copied! id is " << varray_id);
						return *this;
					}

					VertexArray& operator=(VertexArray<Allocator>&& obj)
					{
						a.destruct(varray_id);
						this->varray_id = obj.varray_id;

						a.move(std::move(obj.a));
						CHECK_GL_ERROR;
						DEBUG_OUT("varray moved! id is " << varray_id);
						return *this;
					}

					inline GLuint getID() const
					{
						return varray_id;
					}
			};


		template<typename TargetType, typename Allocator = GLAllocator<Alloc_Texture>> 
			class Texture
			{
				private:

					GLuint texture_id;
					Allocator a;

				public:
					inline void bind(std::size_t TexUnitNum = 0) const
					{
						if(32 <= TexUnitNum)
						{
							std::cerr << "TextureUnit must be between 0 and 32. --set TextureUnit 0" << std::endl;
							TexUnitNum = 0;
						}
						glActiveTexture(GL_TEXTURE0 + TexUnitNum);
						glBindTexture(TargetType::TEXTURE_TARGET, texture_id);
						CHECK_GL_ERROR;
					}

					inline void unbind() const
					{
						glBindTexture(TargetType::TEXTURE_TARGET, 0);
						CHECK_GL_ERROR;
					}

					Texture()
					{
						texture_id = a.construct();
						CHECK_GL_ERROR;
						bind();
						DEBUG_OUT("texture created! id is " << texture_id);
						unbind();
					}

					~Texture()
					{
						a.destruct(texture_id);
						CHECK_GL_ERROR;
						DEBUG_OUT("texture id " << texture_id << " destructed!");
					}

					Texture(const Texture<TargetType, Allocator> &obj)
					{
						this->texture_id = obj.texture_id;

						a.copy(obj.a);
						CHECK_GL_ERROR;
						DEBUG_OUT("texture copied! id is " << texture_id);
					}

					Texture(Texture<TargetType, Allocator>&& obj)
					{
						this->texture_id = obj.texture_id;

						a.move(std::move(obj.a));
						CHECK_GL_ERROR;
						DEBUG_OUT("texture moved! id is " << texture_id);
					}

					Texture& operator=(const Texture<TargetType, Allocator> &obj)
					{
						a.destruct(texture_id);
						this->texture_id = obj.texture_id;

						a.copy(obj.a);
						CHECK_GL_ERROR;
						DEBUG_OUT("texture copied! id is " << texture_id);
						return *this;
					}

					Texture& operator=(Texture<TargetType, Allocator>&& obj)
					{
						a.destruct(texture_id);
						this->texture_id = obj.texture_id;

						a.move(std::move(obj.a));
						CHECK_GL_ERROR;
						DEBUG_OUT("texture moved! id is " << texture_id);
						return *this;
					}

					inline GLuint getID() const
					{
						return texture_id;
					}


					template<GLint level = 0, typename int_format = RGBA, typename format = RGBA, typename... Args>
						inline void texImage2D(Args&&... args)
						{
							bind();
							TextureTraits<TargetType, level, int_format, format>::texImage2D(std::forward<Args>(args)...);
							unbind();
						}
			};


		//some tips
		using VShader = Shader<VertexShader>;
		using FShader = Shader<FragmentShader>;
		using ShaderProgram = ShaderProg<>;
		using VBO = VertexBuffer<ArrayBuffer, StaticDraw>;
		using IBO = VertexBuffer<ElementArrayBuffer, StaticDraw>;
		using VAO = VertexArray<>;
	}
}
