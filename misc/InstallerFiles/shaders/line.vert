 
layout(std140) uniform camera_data
{
	mat4 view, proj;
	vec3 camera_pos;
};

layout(std140) uniform instance_data
{
	mat4 world;
};

layout (location = 0) in vec4 position;

void main()
{
    gl_Position = proj * view * world * position;
}
