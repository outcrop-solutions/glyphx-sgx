 
layout(std140) uniform camera_data
{
	mat4 view, proj;
	vec3 camera_pos, cam_up, cam_right;
};

layout(std140) uniform instance_data
{
	mat4 world;
};

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texcoord;
out vec2 uv;

void main()
{
	uv = texcoord;
	vec3 u_axis = cam_up;//vec3( 0, 0, 1 );
	mat4 vt = transpose( view );
	vec3 pos = ( position.x * vt[0] + position.y * vec4( normalize( u_axis ), 0.0 ) ).xyz;
    gl_Position = proj * view * world * vec4(pos, 1.0);
}
