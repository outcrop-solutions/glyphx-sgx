 
in vec3 frag_normal;
in vec3 scene_eye;
out vec4 outputF;
 
layout(std140) uniform material
{
	vec4 tint_color;
};

void main()
{
	vec3 lit = tint_color.rgb * get_lighting( frag_normal, normalize( scene_eye ), 0.25, 0.25, 1.1 );
    outputF = vec4( lit, tint_color.a );
}
