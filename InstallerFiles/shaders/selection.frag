 
out vec4 outputF;
 
layout(std140) uniform material
{
	vec4 tint_color;
};

in vec3 nml;
in vec2 uv;
in vec3 eye;
uniform sampler2D base_texture;

void main()
{
	vec4 result = tint_color;
	result.a *= clamp( 0.5f + 2.f * ( abs( dot( nml, eye ) ) ), 0, 1 );
    outputF = result;
}
