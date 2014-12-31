#pragma once

#include <cmath>
#define M_PI 3.14159265358979323846
#include <vector>
#include "gl_helper.h"
#include "gl_base.h"
#include "gl_debug.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla

namespace jikoLib{
	namespace GLLib{
		
		class Mesh3D{
			private:
				VBO vertex;
				VBO normal;
				VBO texcrd;
				IBO index;
				VAO v_array;

				glm::vec3 pos;
				glm::quat rot;
				glm::vec3 scale;

			public:

				Mesh3D()
					:pos(0.0f, 0.0f, 0.0f),
					 rot(1.0f, 0.0f, 0.0f, 0.0f),
					 scale(1.0f, 1.0f, 1.0f)  {}

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
				inline void copyIndex(const T (&ind)[Size_Elem])
				{
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
					inline void copyIndex(const T *ind, std::size_t Size_Elem)
					{
						index.copyData(ind, Size_Elem);
					}

				inline void setPos(const glm::vec3 &vec)
				{
					this->pos = vec;
				}

				inline const glm::vec3& getPos()
				{
					return this->pos;
				}

				inline void setScale(const glm::vec3 &vec)
				{
					this->scale = vec;
				}

				inline const glm::vec3& getScale()
				{
					return this->scale;
				}

				inline void rotate(const glm::vec3 &axis, GLfloat angle)
				{
					this->rot = glm::rotate(this->rot, angle, glm::normalize(axis));
				}

				inline void resetRot()
				{
					this->rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
				}

				inline const glm::quat& getRot()
				{
					return this->rot;
				}

				inline glm::mat4 getModelMatrix()
				{
					glm::mat4 mat = glm::mat4_cast(rot);
					glm::mat4 poyo = mat*glm::scale(glm::mat4(), scale);
					return glm::translate(poyo, glm::inverse(glm::mat3(poyo))*pos);
				}
		};

		class Camera{
			private:
				glm::vec3 pos; //position
				glm::vec3 drct;  //direction
				glm::vec3 up; //up

				GLfloat fovy;
				GLfloat aspect;
				GLfloat _near;
				GLfloat _far;

			public:

				Camera()
					:pos(0.0f, 0.0f, 0.0f),
					drct(1.0f, 0.0f, 0.0f),
					up(0.0f, 1.0f, 0.0f),
					fovy(M_PI/4.0),
					aspect(1.0f),
					_near(0.1f),
					_far(10.0f) {}

				inline void setPos(const glm::vec3 &pos)
				{
					this->pos = pos;
				}
				inline void setDrct(const glm::vec3 &drct)
				{
					this->drct = drct;
				}
				inline void setUp(const glm::vec3 &up)
				{
					this->up = up;
				}
				inline void setFovy(GLfloat fovy)
				{
					this->fovy = fovy;
				}
				inline void setAspect(GLfloat aspect)
				{
					this->aspect = aspect;
				}
				inline void setAspect(GLfloat width, GLfloat height)
				{
					this->aspect = width / height;
				}
				inline void setNear(GLfloat _near)
				{
					this->_near = _near;
				}
				inline void setFar(GLfloat _far)
				{
					this->_far = _far;
				}
				inline const glm::vec3& getPos() const
				{
					return this->pos;
				}
				inline const glm::vec3& getDrct() const
				{
					return this->drct;
				}

				inline glm::mat4 getViewMatrix()
				{
					return glm::lookAt(pos, drct, up);
				}

				inline glm::mat4 getProjectionMatrix()
				{
					return glm::perspective(fovy, aspect, _near, _far);
				}
		};

		class AssimpLoader{
			private:
				AssimpLoader(const AssimpLoader&) = delete;
				AssimpLoader(AssimpLoader&&) = delete;
				AssimpLoader& operator=(const AssimpLoader&);
				AssimpLoader& operator=(AssimpLoader&&);

				Assimp::Importer importer;
				const aiScene *scene; //nullptr if not loaded

			public:

				AssimpLoader(const std::string &path) :scene(importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs))
				{
					if(scene == nullptr)
					{
						std::cerr << "Assimp ReadFileError: " << importer.GetErrorString() << std::endl;
					}
				};

				inline const aiScene* getScene() const
				{
					return this->scene; 
				}
		};
	}
}
