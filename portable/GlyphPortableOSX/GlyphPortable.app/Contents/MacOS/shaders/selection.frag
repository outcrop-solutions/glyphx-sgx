 
out vec4 outputF;

in vec3 nml;
in vec3 eye;
in vec4 frag_color;
uniform sampler2D base_texture;

void main()
{
	vec4 result = frag_color;
	result.a *= rim( normalize( nml ), normalize( eye ), 1, 1, 0.5 );
    outputF = result;
}
