R"(
#version 120
// showdepth.frag

varying vec2 Texcrd;

uniform sampler2D depth;

void main (void)
{
  gl_FragColor = texture2D(depth, Texcrd);
}
)"
