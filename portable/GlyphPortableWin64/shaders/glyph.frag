 
in vec3 frag_normal;
in vec4 frag_color;
in vec3 eye;
out vec4 outputF;

layout(std140) uniform global_material_data
{
	float base_alpha;
};

void main()
{
	vec3 light = get_lighting( frag_normal, normalize( eye ), 0.75, 0.25, 0.9 );
    outputF = vec4( frag_color.rgb * light, frag_color.a * base_alpha );
}
