 
layout(std140) uniform camera_data
{
	mat4 view, proj;
	vec3 eye;
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
    gl_Position = proj * view * world * position;
}
