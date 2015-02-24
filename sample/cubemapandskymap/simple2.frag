R"(
#version 120

varying vec3 Vertex;
varying vec3 Normal;
varying mat4 View;

uniform samplerCube textureobj;

uniform bool drawsphere;

void main()
{
	vec4 Color;
	if(drawsphere)
	{
		Color = textureCube(textureobj, mat3(transpose(View))*reflect(normalize(Vertex), Normal));
	}
	else
	{
		Color = textureCube(textureobj, mat3(transpose(View))*Vertex);
	}
	gl_FragColor = Color;
}
)"
