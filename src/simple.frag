R"(
#version 120

varying vec3 Vertex;

uniform sampler2D textureobj;

void main()
{
	//gl_FragColor = texture2D(textureobj, Texcrd);
	gl_FragColor = vec4(Vertex.z/200.0, Vertex.z/200.0, Vertex.z/200.0, 1.0);
}
)"
