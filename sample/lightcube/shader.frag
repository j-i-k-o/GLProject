R"(
#version 120
varying vec3 Normal;
varying vec3 Vertex;
varying vec2 Texcrd;

varying mat4 Model;
varying mat4 View;
varying mat4 Projection;

struct Light{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};

uniform Light light;

struct Material{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform Material material;

uniform sampler2D textureobj;

void main()
{
	//ambient
	vec4 ambient = light.ambient*material.ambient;
	//diffuse
	vec3 N = normalize(mat3(View)*mat3(Model)*Normal);
	vec3 P = (View*Model*vec4(Vertex, 1.0)).xyz;
	vec3 L = mat3(View)*light.position;
	float diffuseLighting = max(dot(N, normalize(L-P)), 0);
	vec4 diffuse = light.diffuse*diffuseLighting*material.diffuse;
	//specular
	vec3 H = normalize(normalize(L-P)+normalize(-P));
	float specularLighting = pow(max(dot(H, N),0), material.shininess);
	/*
	if(diffuseLighting <= 0.0)
	{
		specularLighting = 0.0;
	}
	*/
	vec4 specular = specularLighting*light.specular*material.specular;
	vec4 texcolor = texture2D(textureobj, Texcrd);
	gl_FragColor = ambient + diffuse + specular;
}
)"
