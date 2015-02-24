R"(
#version 120

attribute vec3 vertex;
attribute vec3 normal;
attribute vec2 texcrd;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 Vertex;
varying vec3 Normal;
varying mat4 View;

void main()
{
	Vertex = (view * model * vec4(vertex, 1.0)).xyz;
	Normal = normalize(mat3(view * model) * normal);
	View = view;
	gl_Position = projection * view * model * vec4(vertex, 1.0);
}
)"
