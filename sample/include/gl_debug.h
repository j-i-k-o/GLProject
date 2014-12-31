#pragma once

#include <iostream>

namespace jikoLib{
	namespace GLLib{

#ifdef DEBUG
#define DEBUG_OUT(x) std::cout << x << std::endl;
#else
#define DEBUG_OUT(x)
#endif

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
#else
#define CHECK_GL_ERROR
#endif
	}
}
