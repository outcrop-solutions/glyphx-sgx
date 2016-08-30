 
out vec4 outputF;
 
layout(std140) uniform material
{
	vec4 tint_color;
};

in vec2 uv;
uniform sampler2D base_texture;

void main()
{
	vec4 color = tint_color * texture( base_texture, uv );
    if ( color.a == 0 ) discard;
    outputF = color * color.a;
}
