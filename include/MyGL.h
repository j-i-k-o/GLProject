#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <tuple>
#include <functional>
#include "HelperClass.h"

namespace GLLib {

	/*
	 * Check OpenGL Error
	 */

#ifdef DEBUG
	extern GLenum _err_;
#endif
#ifdef DEBUG
#define CHECK_GL_ERROR \
	_err_ = glGetError(); \
	if(_err_ == GL_INVALID_ENUM) std::cerr << "GL_INVALID_ENUM" << " at " << __FILE__ << ": " << __LINE__ << std::endl; \
	if(_err_ == GL_INVALID_VALUE) std::cerr << "GL_INVALID_VALUE" << " at " << __FILE__ << ": " << __LINE__ << std::endl; \
	if(_err_ == GL_INVALID_OPERATION) std::cerr << "GL_INVALID_OPERATION" << " at " << __FILE__ << ": " << __LINE__ << std::endl; \
	if(_err_ == GL_STACK_OVERFLOW) std::cerr << "GL_STACK_OVERFLOW" << " at " << __FILE__ << ": " << __LINE__ << std::endl; \
	if(_err_ == GL_STACK_UNDERFLOW) std::cerr << "GL_STACK_UNDERFLOW" << " at " << __FILE__ << ": " << __LINE__ << std::endl; \
	if(_err_ == GL_OUT_OF_MEMORY) std::cerr << "GL_OUT_OF_MEMORY" << " at " << __FILE__ << ": " << __LINE__ << std::endl; \
	if(_err_ == GL_TABLE_TOO_LARGE) std::cerr << "GL_TABLE_TOO_LARGE" << " at " << __FILE__ << ": " << __LINE__ << std::endl;
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

	// class forward declaration

	template<typename Shader_type, typename Allocator>
		class Shader;
	template<typename Allocator> 
		class ShaderProg;
	template<typename TargetType, typename UsageType, typename Allocator>
		class VertexBuffer;
	template<typename Allocator> 
		class VertexArray;


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

			void finalize()
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

			GLObject& operator<<(Begin&& obj)
				//initializer
			{
				initialize(obj.m_window);
				return *this;
			}

			GLObject& operator<<(MakeCurrent&& obj)
				//make current
			{
				makeCurrent(obj.m_window);
				return *this;
			}

			GLObject& operator<<(End&&)
				//finalizer
			{
				finalize();
				return *this;
			}

			template<typename UsageType, typename Allocator_sh, typename Allocator_vb, typename Allocator_va>
				void connectAttrib(const ShaderProg<Allocator_sh> &prog, const VertexBuffer<ArrayBuffer, UsageType, Allocator_vb> &buffer, const VertexArray<Allocator_va> &varray, const std::string &name)
				// TODO: make parser for searching name.c_str() string in prog source
				{
					if(!buffer.isSetArray)
					{
						std::cerr << "Array is not set! --did nothing" << std::endl;
						return;
					}
					if(getSizeof(buffer.ArrayEnum) == 0)
					{
						std::cerr << "buffer ArrayEnum is invalid! --did nothing" << std::endl;
						return;
					}
					varray.bind();
					buffer.bind();
					GLint attribloc = glGetAttribLocation(prog.getID(), name.c_str());
					CHECK_GL_ERROR;
					glVertexAttribPointer(attribloc, buffer.Dim, buffer.ArrayEnum, GL_FALSE, buffer.Dim*getSizeof(buffer.ArrayEnum), 0);
					CHECK_GL_ERROR;
					glEnableVertexAttribArray(attribloc);
					CHECK_GL_ERROR;
					varray.unbind();
				}

			template<typename Allocator_sh>
				void disconnectAttrib(const ShaderProg<Allocator_sh> &prog, const std::string &name)
				{
					glDisableVertexAttribArray(glGetAttribLocation(prog.getID(), name.c_str()));
					CHECK_GL_ERROR;
				}

			//draw
			
			template<typename RenderMode = rm_Triangles, typename varrAlloc, typename Sp_Alloc, typename vbUsage, typename vbAlloc>
			void drawElements(const VertexArray<varrAlloc> &varray, const ShaderProg<Sp_Alloc> &program, const VertexBuffer<ElementArrayBuffer, vbUsage, vbAlloc> &ibo)
			{
				varray.bind();
				ibo.bind();
				program.bind();

				if(!ibo.isSetArray)
				{
					std::cerr << "IBO array isn't set. cannot draw" << std::endl;
				}
				//else
				glDrawElements(RenderMode::RENDER_MODE, ibo.Size_Elem, ibo.ArrayEnum, NULL);
				CHECK_GL_ERROR;
			}

	};

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
					a.copy(obj.a);
					DEBUG_OUT("shader copied! shader_id is " << shader_id);
					CHECK_GL_ERROR;
				}

				Shader(Shader<Shader_type, Allocator>&& obj)
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
				Allocator a;
				bool isLinked = false;
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
					shaderprog_id = obj.shaderprog_id;
					a.copy(obj.a);
					CHECK_GL_ERROR;
					DEBUG_OUT("shaderprog copied! shaderprog id is " << shaderprog_id);
				}

				ShaderProg(ShaderProg<Allocator>&& obj)
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

						static_assert(is_all_same<ArgTypes...>::value, "ArgTypes must be all same");
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
					}

				template <std::size_t Size_Elem, std::size_t Dim, typename T>
					void setUniformXtv(const std::string &str, const std::array<std::array<T,Dim>,Size_Elem>& array)
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

						glUniformXtv<Dim, T>::func(loc, Size_Elem, array.data());
						CHECK_GL_ERROR;
					}

				//TODO: setUniformMatrixXtv
				

				//for glm
				inline void setUniformMatrix4fv_glm(const std::string &str, const glm::mat4 &matrix)
				{
					bind();
					auto loc = glGetUniformLocation(shaderprog_id, str.c_str());
					CHECK_GL_ERROR;
					if(loc == -1)
					{
						std::cerr << "uniform variable " << str << " cannot be found" << std::endl;
						return;
					}

					glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
				}

		};

	//vertexbuffer
	template<typename TargetType, typename UsageType, typename Allocator = GLAllocator<Alloc_VertexBuffer>>
		class VertexBuffer
		{
			private:
				GLuint buffer_id;
				Allocator a;

				bool isSetArray = false; 
				GLenum ArrayEnum;
				std::size_t Size_Elem;
				std::size_t Dim;

				template<typename Type> 
					inline void setSizeElem_Dim_Type(std::size_t Size_Elem, std::size_t Dim)
					{
						this->Size_Elem = Size_Elem;
						this->Dim = Dim;
						this->ArrayEnum = getEnum<Type>::value;
						isSetArray = true;
					}

			public:

				//friend function
				template<typename UsageType_v, typename Allocator_sh, typename Allocator_vb, typename Allocator_va>
					friend void 
					GLObject::connectAttrib(const ShaderProg<Allocator_sh> &prog, const VertexBuffer<ArrayBuffer, UsageType_v, Allocator_vb> &buffer, const VertexArray<Allocator_va> &varray, const std::string &name);

				template<typename RenderMode, typename varrAlloc, typename Sp_Alloc, typename vbUsage, typename vbAlloc>
					friend void
					GLObject::drawElements(const VertexArray<varrAlloc> &varray, const ShaderProg<Sp_Alloc> &program, const VertexBuffer<ElementArrayBuffer, vbUsage, vbAlloc> &ibo);


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

				inline bool getisSetArray() const
				{
					return isSetArray;
				}

				VertexBuffer()
				{
					buffer_id = a.construct();
					CHECK_GL_ERROR;
					bind();
					DEBUG_OUT("vbuffer created! id is " << buffer_id);
				}

				~VertexBuffer()
				{
					a.destruct(buffer_id);
					CHECK_GL_ERROR;
					DEBUG_OUT("vbuffer id " << buffer_id << " destructed!");
				}

				VertexBuffer(const VertexBuffer<TargetType, UsageType, Allocator> &obj)
				{
					buffer_id = obj.buffer_id;
					a.copy(obj.a);
					CHECK_GL_ERROR;
					DEBUG_OUT("vbuffer copied! id is " << buffer_id);
				}

				VertexBuffer(VertexBuffer<TargetType, UsageType, Allocator>&& obj)
				{
					buffer_id = obj.buffer_id;
					a.move(std::move(obj.a));
					CHECK_GL_ERROR;
					DEBUG_OUT("vbuffer moved! id is " << buffer_id);
				}

				VertexBuffer& operator=(const VertexBuffer<TargetType, UsageType, Allocator> &obj)
				{
					a.destruct(buffer_id);
					buffer_id = obj.buffer_id;
					a.copy(obj.a);
					CHECK_GL_ERROR;
					DEBUG_OUT("vbuffer copied! id is " << buffer_id);
					return *this;
				}

				VertexBuffer& operator=(VertexBuffer<TargetType, UsageType, Allocator>&& obj)
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

				template<typename T,std::size_t Size_Elem, std::size_t Dim>
					VertexBuffer& operator<<(const std::array<std::array<T, Dim>, Size_Elem> &array)
					// Set Array for std::array 
					{
						static_assert( is_exist<T, GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat, GLdouble>::value, "Invalid type" );
						static_assert( Dim <= 4, "Invalid dimension" );
						static_assert( (Size_Elem != 0)&&(Dim != 0), "Zero Elem" );
						static_assert((!std::is_same<TargetType,ElementArrayBuffer>::value)||((std::is_same<TargetType,ElementArrayBuffer>::value)&&(is_exist<T,GLubyte,GLushort,GLuint>::value)),"IBO array type must be GLushort or GLuint or GLubyte");
						bind();
						glBufferData(TargetType::BUFFER_TARGET, Size_Elem*Dim*sizeof(T), array.data(), UsageType::BUFFER_USAGE);
						CHECK_GL_ERROR;
						DEBUG_OUT("allocate "<< Size_Elem*Dim*sizeof(T) <<" B success! buffer id is " << buffer_id);
						setSizeElem_Dim_Type<T>(Size_Elem, Dim);
						return *this;
					}

				template<typename T,std::size_t Size_Elem, std::size_t Dim>
					VertexBuffer& operator<<(const T (&array)[Size_Elem][Dim])
					// Set Array for raw array
					{
						static_assert( is_exist<T, GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat, GLdouble>::value, "Invalid type" );
						static_assert( Dim <= 4, "Invalid dimension" );
						static_assert( (Size_Elem != 0)&&(Dim != 0), "Zero Elem" );
						static_assert((!std::is_same<TargetType,ElementArrayBuffer>::value)||((std::is_same<TargetType,ElementArrayBuffer>::value)&&(is_exist<T,GLubyte,GLushort,GLuint>::value)),"IBO array type must be GLushort or GLuint or GLubyte");
						bind();
						glBufferData(TargetType::BUFFER_TARGET, Size_Elem*Dim*sizeof(T), array, UsageType::BUFFER_USAGE);
						CHECK_GL_ERROR;
						DEBUG_OUT("allocate "<< Size_Elem*Dim*sizeof(T) <<" B success! buffer id is " << buffer_id);
						setSizeElem_Dim_Type<T>(Size_Elem, Dim);
						return *this;
					}

				template<typename T, std::size_t Size_Elem>
					VertexBuffer& operator<<(const T (&array)[Size_Elem])
					//Set Array for raw array
					{
						static_assert( is_exist<T, GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat, GLdouble>::value, "Invalid type" );
						static_assert( (Size_Elem != 0), "Zero Elem" );
						static_assert((!std::is_same<TargetType,ElementArrayBuffer>::value)||((std::is_same<TargetType,ElementArrayBuffer>::value)&&(is_exist<T,GLubyte,GLushort,GLuint>::value)),"IBO array type must be GLushort or GLuint or GLubyte");
						bind();
						glBufferData(TargetType::BUFFER_TARGET, Size_Elem*sizeof(T), array, UsageType::BUFFER_USAGE);
						CHECK_GL_ERROR;
						DEBUG_OUT("allocate "<< Size_Elem*sizeof(T) <<" B success! buffer id is " << buffer_id);
						setSizeElem_Dim_Type<T>(Size_Elem, 1);
						return *this;
					}

				template<typename T>
					VertexBuffer& operator<<(const std::vector<std::vector<T>> &array)
					// Set Array for std::vector
					{
						static_assert( is_exist<T, GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat, GLdouble>::value, "Invalid type" );
						static_assert((!std::is_same<TargetType,ElementArrayBuffer>::value)||((std::is_same<TargetType,ElementArrayBuffer>::value)&&(is_exist<T,GLubyte,GLushort,GLuint>::value)),"IBO array type must be GLushort or GLuint or GLubyte");
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

				VertexArray()
				{
					varray_id = a.construct();
					CHECK_GL_ERROR;
					bind();
					DEBUG_OUT("varray created! id is " << varray_id);
				}

				~VertexArray()
				{
					a.destruct(varray_id);
					CHECK_GL_ERROR;
					DEBUG_OUT("varray id " << varray_id << " destructed!");
				}

				VertexArray(const VertexArray<Allocator> &obj)
				{
					varray_id = obj.varray_id;
					a.copy(obj.a);
					CHECK_GL_ERROR;
					DEBUG_OUT("varray copied! id is " << varray_id);
				}

				VertexArray(VertexArray<Allocator>&& obj)
				{
					varray_id = obj.varray_id;
					a.move(std::move(obj.a));
					CHECK_GL_ERROR;
					DEBUG_OUT("varray moved! id is " << varray_id);
				}

				VertexArray& operator=(const VertexArray<Allocator> &obj)
				{
					a.destruct(varray_id);
					varray_id = obj.varray_id;
					a.copy(obj.a);
					CHECK_GL_ERROR;
					DEBUG_OUT("varray copied! id is " << varray_id);
					return *this;
				}

				VertexArray& operator=(VertexArray<Allocator>&& obj)
				{
					a.destruct(varray_id);
					varray_id = obj.varray_id;
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

	//some tips
	using VBO = VertexBuffer<ArrayBuffer, StaticDraw>;
	using IBO = VertexBuffer<ElementArrayBuffer, StaticDraw>;
	using VAO = VertexArray<>;

}
