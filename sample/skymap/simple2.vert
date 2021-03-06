R"(
#version 120

attribute vec3 vertex;
attribute vec3 normal;
attribute vec2 texcrd;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 Texcrd;

void main()
{
	Texcrd = (model * vec4(vertex, 1.0)).xyz;
	gl_Position = projection * view * model * vec4(vertex, 1.0);
}
)"
