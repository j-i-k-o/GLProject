R"(
#version 120
attribute vec2 position;
attribute vec2 texcoord;
varying vec2 v_tex;
void main()
{
	v_tex = texcoord;
	gl_Position = vec4(position, 0.0, 1.0);
}
)"
