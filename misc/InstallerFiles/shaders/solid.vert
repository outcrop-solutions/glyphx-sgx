 
layout(std140) uniform camera_data
{
	mat4 view, proj;
	vec3 eye;
};

layout(std140) uniform instance_data
{
	mat4 world;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 frag_normal;
out vec3 scene_eye;

void main()
{
	scene_eye = eye;
	frag_normal = normalize( ( world * vec4( normal, 0 ) ).xyz );
    gl_Position = proj * view * world * vec4( position, 1 );
}
