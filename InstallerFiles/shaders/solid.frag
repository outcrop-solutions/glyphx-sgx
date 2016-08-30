 
in vec3 frag_normal;
out vec4 outputF;
 
layout(std140) uniform material
{
	vec4 tint_color;
};

void main()
{
	vec3 lit = tint_color.rgb * get_lighting( frag_normal );
    outputF = vec4( lit, tint_color.a );
}
