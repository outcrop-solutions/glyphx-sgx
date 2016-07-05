
#version 330 core
 
in vec3 frag_normal;
flat in int instance_id;
out vec4 outputF;
 
layout(std140) uniform material
{
	vec4 color[512];
};

void main()
{
	const vec4 ambient = vec4( 0.2 );
	const float light1_str = 1.0;
	const float light2_str = 0.25;
	const vec4 color1 = vec4( vec3( light1_str ), 1 );
	const vec4 color2 = vec4( vec3( light2_str ), 1 );
	vec3 light_dir = normalize( vec3( 1, 1, -1 ) );
    vec4 light1 = clamp( color1 * dot( frag_normal, light_dir ), 0, 1 );
    vec4 light2 = clamp( color2 * dot( frag_normal, -light_dir ), 0, 1 );
    vec4 light = clamp( ambient + ( light1 + light2 ), 0, 1 );
    outputF = color[instance_id] * light;
}
