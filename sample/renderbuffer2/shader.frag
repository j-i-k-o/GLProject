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

struct Material{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

struct Attenuation{
	float constant;
	float linear;
	float quadratic;
};


uniform Light light;
uniform Material material;
uniform Attenuation attenuation;

uniform sampler2D textureobj;

uniform sampler2DShadow shadowTexture;
uniform float shadowTextureWidth;
uniform float shadowTextureHeight;

void main()
{
	//ambient
	vec4 ambient = light.ambient*texture2D(textureobj, Texcrd);
	//diffuse
	vec3 N = normalize(mat3(View*Model)*Normal);
	vec3 P = (View*Model*vec4(Vertex, 1.0)).xyz;
	vec3 L = (View*vec4(light.position, 1.0)).xyz;
	float diffuseLighting = max(dot(N, normalize(L-P)), 0);
	vec4 diffuse = light.diffuse*diffuseLighting*texture2D(textureobj, Texcrd);
	//specular
	vec3 H = normalize(normalize(L-P)+normalize(-P));
	float specularLighting = pow(max(dot(H, N),0), material.shininess);
	if(diffuseLighting <= 0.0)
	{
		specularLighting = 0.0;
	}
	vec4 specular = specularLighting*light.specular*material.specular;
	vec4 texcolor = texture2D(textureobj, Texcrd);
	gl_FragColor = (ambient + diffuse + specular)*(1.0/(attenuation.constant+attenuation.linear*length(L-P)+attenuation.quadratic*length(L-P)*length(L-P)));
}
)"
