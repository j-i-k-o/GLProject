R"(
#version 120

attribute vec3 norm;
attribute vec3 vertex;
attribute vec2 texcrd;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 Normal;
varying vec3 Vertex;
varying vec2 Texcrd;

varying mat4 Model;
varying mat4 View;
varying mat4 Projection;

void main()
{
	Normal = norm;
	Vertex = vertex;
	Texcrd = texcrd;
	Model = model;
	View= view;
	Projection = projection;

	gl_Position = projection*view*model*vec4(vertex, 1.0);
}
)"
