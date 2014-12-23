#pragma once

#include "gl_helper.h"
#include "gl_base.h"
#include "gl_debug.h"

namespace jikoLib{
	namespace GLLib{
		
		class Mesh3D{
			private:
				VBO vertex;
				VBO normal;
				VBO texcrd;
				IBO index;
				VAO v_array;
			public:
				inline const VBO& getVertex() const
				{
					return vertex;
				}
				inline const VBO& getNormal() const
				{
					return normal;
				}
				inline const VBO& getTexcrd() const
				{
					return texcrd;
				}
				inline const VAO& getVArray() const
				{
					return v_array;
				}

				inline bool getIsSetArray() const
				{
					return 
						vertex.getisSetArray() &&
						normal.getisSetArray() &&
						texcrd.getisSetArray();
				}

				Mesh3D operator+(const Mesh3D &obj)
				{

				}

		};
	}
}
