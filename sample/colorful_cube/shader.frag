R"(
#version 120
varying vec3 Normal;
varying vec3 Vertex;

void main()
{
	gl_FragColor = vec4(Normal, 1.0);
}
)"
