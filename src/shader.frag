R"(
#version 120
uniform sampler2D surftexture;
varying vec2 v_tex;
void main()
{
	gl_FragColor = texture2D(surftexture, v_tex);
	//gl_FragColor = vec4(v_tex, 0.0, 1.0);
}
)"
