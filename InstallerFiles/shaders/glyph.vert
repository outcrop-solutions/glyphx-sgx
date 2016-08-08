
#version 330 core
 
layout(std140) uniform scene_data
{
	mat4 view, proj;
	float elapsed_seconds;
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

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
out vec3 frag_normal;
out vec4 frag_color;
flat out int instance_id;

mat3 rotation_matrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c );
}

void main()
{
	vec3 rotation_axis = rotation[gl_InstanceID].xyz;
	float rotation_rate = rotation[gl_InstanceID].w;
	mat3 rotation = rotation_matrix( rotation_axis, elapsed_seconds * rotation_rate );

	vec3 pos = rotation * position.xyz;

	frag_normal = normalize( ( world[gl_InstanceID] * vec4( normal, 0 ) ).xyz );
	instance_id = gl_InstanceID;
	frag_color = color[gl_InstanceID];
    gl_Position = proj * view * world[gl_InstanceID] * vec4( pos, 1 );
}
