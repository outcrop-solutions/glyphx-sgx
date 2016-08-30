 
out vec4 outputF;

in vec3 nml;
in vec3 eye;
in vec4 frag_color;
uniform sampler2D base_texture;

void main()
{
	vec4 result = frag_color;
	result.a *= clamp( 0.5f + 2.f * ( abs( dot( nml, eye ) ) ), 0, 1 );
    outputF = result;
}
