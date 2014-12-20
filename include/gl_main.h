#pragma once

#include "gl_base.h"
#include "gl_debug.h"
#include "gl_mesh.h"
#include <functional>

namespace jikoLib{
	namespace GLLib {


		class GLObject
		{
			private:
				//member variable
				bool _is_initialized;

			public:
				GLObject() :_is_initialized(false)
			{
			}
				bool initialize()
				{
					bool success = true;
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
						//OpenGL initialize complete
						DEBUG_OUT("--- OpenGL initialize complete ---");
						DEBUG_OUT("OpenGL Vendor: " << glGetString(GL_VENDOR));
						DEBUG_OUT("OpenGL Renderer: " << glGetString(GL_RENDERER));
						DEBUG_OUT("OpenGL Version: " << glGetString(GL_VERSION));
						DEBUG_OUT(" ");
						_is_initialized = true;
					}
					ilInit();
					iluInit();
					ILenum ilerror = ilGetError();
					if(ilerror != IL_NO_ERROR)
					{
						std::cerr << "cannot initialize devIL!: " << iluErrorString(ilerror) << std::endl; 
					}
					else
					{
						DEBUG_OUT("--- devIL initialize complete ---");
						ilClearColour(255, 255, 255, 0);
					}
					return success;
				}

				/*
				void finalize(const std::function<void()> &fin_func)
				{
					if(_is_initialized)
					{
						fin_func();
						DEBUG_OUT("OpenGL Context is closed.");
						_is_initialized = false;
					}
				}
				*/

				/*
				void makeCurrent(const std::function<void()> &makec_func)
				{
					makec_func();
					DEBUG_OUT("Make Current");
				}
				*/


				
				GLObject& operator<<(Begin&&)
					//initializer
				{
					initialize();
					return *this;
				}
				/*

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
				*/

				template<typename UsageType, typename Allocator_sh, typename Allocator_vb, typename Allocator_va>
					void connectAttrib(const ShaderProg<Allocator_sh> &prog, const VertexBuffer<ArrayBuffer, UsageType, Allocator_vb> &buffer, const VertexArray<Allocator_va> &varray, const std::string &name)
					{
						if(!buffer.getisSetArray())
						{
							std::cerr << "Array is not set! --did nothing" << std::endl;
							return;
						}
						if(getSizeof(buffer.getArrayEnum()) == 0)
						{
							std::cerr << "buffer ArrayEnum is invalid! --did nothing" << std::endl;
							return;
						}
						varray.bind();
						buffer.bind();
						GLint attribloc = glGetAttribLocation(prog.getID(), name.c_str());
						CHECK_GL_ERROR;
						if(attribloc == -1)
						{
							std::cerr << "attribute variable " << name << " cannot be found" << std::endl;
							return;
						}
						glVertexAttribPointer(attribloc, buffer.getDim(), buffer.getArrayEnum(), GL_FALSE, buffer.getDim()*getSizeof(buffer.getArrayEnum()), 0);
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
					void draw(const VertexArray<varrAlloc> &varray, const ShaderProg<Sp_Alloc> &program, const VertexBuffer<ElementArrayBuffer, vbUsage, vbAlloc> &ibo)
					{
						varray.bind();
						ibo.bind();
						program.bind();

						if(!ibo.getisSetArray())
						{
							std::cerr << "IBO array isn't set. cannot draw" << std::endl;
						}
						//else
						glDrawElements(RenderMode::RENDER_MODE, ibo.getSizeElem(), ibo.getArrayEnum(), NULL);
						CHECK_GL_ERROR;
					}

				template<typename RenderMode = rm_Triangles, typename varrAlloc, typename Sp_Alloc, typename vbUsage, typename vbAlloc>
					void draw(const VertexArray<varrAlloc> &varray, const ShaderProg<Sp_Alloc> &program, const VertexBuffer<ArrayBuffer, vbUsage, vbAlloc> &vbo)
					{
						varray.bind();
						program.bind();
						if(!vbo.getisSetArray())
						{
							std::cerr << "VBO array isn't set. cannot draw" << std::endl;
						}
						//else
						glDrawArrays(RenderMode::RENDER_MODE, 0, vbo.getSizeElem());
						CHECK_GL_ERROR;
					}

				template<typename RenderMode = rm_Triangles, typename varrAlloc, typename Sp_Alloc, typename TexTarget, typename Tex_Unit, typename TexAlloc, typename vbUsage, typename vbAlloc>
					void draw(const VertexArray<varrAlloc> &varray, const ShaderProg<Sp_Alloc> &program, const VertexBuffer<ElementArrayBuffer, vbUsage, vbAlloc> &ibo, std::initializer_list<Texture<TexTarget, Tex_Unit, TexAlloc> > tex_list)
					{
						for (auto&& var : tex_list) {
							var.bind();
						}
						draw<RenderMode>(varray, program, ibo);
					}

				template<typename RenderMode = rm_Triangles, typename varrAlloc, typename Sp_Alloc, typename TexTarget, typename Tex_Unit, typename TexAlloc, typename vbUsage, typename vbAlloc>
					void draw(const VertexArray<varrAlloc> &varray, const ShaderProg<Sp_Alloc> &program, const VertexBuffer<ArrayBuffer, vbUsage, vbAlloc> &vbo, std::initializer_list<Texture<TexTarget, Tex_Unit, TexAlloc> > tex_list)
					{
						for (auto&& var : tex_list) {
							var.bind();
						}
						draw<RenderMode>(varray, program, vbo);
					}

		};
	} 
}
