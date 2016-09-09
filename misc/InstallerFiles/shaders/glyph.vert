
layout(std140) uniform scene_data
{
	mat4 view, proj;
	float elapsed_seconds;
	vec3 camera_pos;
};

layout(std140) uniform instance_data
{
	mat4 world[512];
};

layout(std140) uniform material
{
	vec4 color[512];
};

struct rotation_t
{
	vec4 axis_rate;		// xyz = axis, w = rate
	vec4 center;	// xyz = rotate around
};

layout(std140) uniform animation
{
	rotation_t rotation[512];
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 frag_normal;
out vec4 frag_color;
out vec3 eye;

void main()
{
	vec3 rotation_center = rotation[gl_InstanceID].center.xyz;
	vec3 rotation_axis = rotation[gl_InstanceID].axis_rate.xyz;
	float rotation_rate = rotation[gl_InstanceID].axis_rate.w;
	mat3 rotation_mat = rotation_matrix( rotation_axis, elapsed_seconds * rotation_rate );

	frag_normal = normalize( rotation_mat * vec3( inverse( transpose( world[gl_InstanceID] ) ) * vec4( normal, 0 ) ) );

	frag_color = color[gl_InstanceID];

	vec4 world_pos = world[gl_InstanceID] * vec4( position, 1 );
	world_pos = vec4( offset_rotation( world_pos.xyz, rotation_mat, rotation_center ), 1 );

	eye = world_pos.xyz - camera_pos;	// fragment shader will normalize (has to be done there since this will be interpolated)

    gl_Position = proj * ( view * world_pos );
}
 