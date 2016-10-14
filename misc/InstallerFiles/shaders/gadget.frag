 
out vec4 outputF;
 
layout(std140) uniform material
{
	vec4 tint_color;
};

in vec2 uv;
uniform sampler2D base_texture;

void main()
{
    outputF = tint_color * texture( base_texture, uv );
}
