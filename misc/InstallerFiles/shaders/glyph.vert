
layout(std140) uniform scene_data
{
	mat4 view, proj;
	float elapsed_seconds;
	vec3 eye;
};

layout(std140) uniform instance_data
{
	mat4 world[512];
};

layout(std140) uniform material
{
	vec4 color[512];
};

layout(std140) uniform animation
{
	vec4 rotation[512];	// xyz = axis, w = rate
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 frag_normal;
out vec4 frag_color;
out vec3 scene_eye;

void main()
{
	vec3 rotation_axis = rotation[gl_InstanceID].xyz;
	float rotation_rate = rotation[gl_InstanceID].w;
	mat3 rotation_mat = rotation_matrix( rotation_axis, elapsed_seconds * rotation_rate );

	vec3 pos = rotation_mat * position.xyz;

	frag_normal = normalize( ( inverse( transpose( world[gl_InstanceID] ) ) * vec4( rotation_mat * normal, 0 )  ).xyz );
	frag_color = color[gl_InstanceID];
	scene_eye = eye;
    gl_Position = proj * ( view * ( world[gl_InstanceID] * vec4( pos, 1 ) ) );
}
 