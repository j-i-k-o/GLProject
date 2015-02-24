R"(
#version 120

varying vec2 Texcrd;

uniform sampler2D textureobj;

void main()
{
	gl_FragColor = texture2D(textureobj, Texcrd);
}
)"
