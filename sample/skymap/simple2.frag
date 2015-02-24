R"(
#version 120

varying vec3 Texcrd;

uniform samplerCube textureobj;

void main()
{
	gl_FragColor = textureCube(textureobj, Texcrd);
}
)"
