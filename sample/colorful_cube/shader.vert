R"(
#version 120

attribute vec3 norm;
attribute vec3 vertex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 Normal;
varying vec3 Vertex;

void main()
{
	Normal = normalize(mat3(model)*norm);
	Vertex = mat3(view*model)*vertex;
	gl_Position = projection*view*model*vec4(vertex, 1.0);
}
)"
