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
				inline const IBO& getIndex() const
				{
					return index;
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

				inline bool getIsIndexSet() const
				{
					return index.getisSetArray();
				}

				template<typename T, std::size_t Size_Elem>
				inline void copyData(const T (&vert)[Size_Elem][3], const T (&norm)[Size_Elem][3], const T (&tex)[Size_Elem][2])
				{
					vertex.copyData(vert);
					normal.copyData(norm);
					texcrd.copyData(tex );
				}

				template<typename T, std::size_t Size_Elem>
				inline void copyData(const T (&vert)[Size_Elem][3], const T (&norm)[Size_Elem][3], const T (&tex)[Size_Elem][2], const T (&ind)[Size_Elem])
				{
					vertex.copyData(vert);
					normal.copyData(norm);
					texcrd.copyData(tex );
					index.copyData(ind);
				}

				template<typename T>
					inline void copyData(const T *vert, const T *norm, const T *tex, std::size_t Size_Elem)
					{
						vertex.copyData(vert, Size_Elem, 3);
						normal.copyData(norm, Size_Elem, 3);
						texcrd.copyData( tex, Size_Elem, 2);
					}

				template<typename T>
					inline void copyData(const T *vert, const T *norm, const T *tex, const T *ind, std::size_t Size_Elem)
					{
						vertex.copyData(vert, Size_Elem, 3);
						normal.copyData(norm, Size_Elem, 3);
						texcrd.copyData( tex, Size_Elem, 2);
						index.copyData(ind, Size_Elem);
					}


		};
	}
}
