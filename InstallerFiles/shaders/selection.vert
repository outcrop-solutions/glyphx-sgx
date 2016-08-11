
layout(std140) uniform camera_data
{
	mat4 view, proj;
	vec3 eyevec;
};

layout(std140) uniform global_data
{
	float scale;
	float elapsed_seconds;
};

layout(std140) uniform instance_data
{
	mat4 world;
	vec4 rotation_animation;
};

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
out vec3 nml;
out vec2 uv;
out vec3 eye;

void main()
{
	uv = texcoord;
	nml = vec3( view * world * normalize( vec4( normal, 0 ) ) );
	eye = eyevec;

	vec3 rotation_axis = rotation_animation.xyz;
	float rotation_rate = rotation_animation.w;
	mat3 rotation = rotation_matrix( rotation_axis, elapsed_seconds * rotation_rate );

	vec3 pos = rotation * position.xyz;

    gl_Position = proj * view * world * vec4( pos * scale, 1 );
}
