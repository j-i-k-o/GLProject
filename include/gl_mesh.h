#pragma once

#include "gl_helper.h"
#include "gl_base.h"
#include "gl_debug.h"
#include <vector>

namespace jikoLib{
	namespace GLLib{

		class Mesh
		{
			private:
				VertexBuffer<ArrayBuffer, StaticDraw, GLAllocator<Alloc_VertexBuffer>> vertex;
				VertexBuffer<ArrayBuffer, StaticDraw, GLAllocator<Alloc_VertexBuffer>> normal;
			public:
		};

	}
}
