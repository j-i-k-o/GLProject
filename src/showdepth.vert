R"(
#version 120
// showdepth.vert

attribute vec3 vertex;
attribute vec2 texcrd;

varying vec2 Texcrd;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	Texcrd = texcrd;
	gl_Position = projection * view * model * vec4(vertex, 1.0);
}
)"
