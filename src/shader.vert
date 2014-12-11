R"(
#version 120
attribute vec2 LVertexPos2D;
attribute vec3 Color;
varying vec3 vcolor;
void main()
{
	vcolor = Color;
	gl_Position = vec4(LVertexPos2D, 0.0, 1.0);
}
)"
