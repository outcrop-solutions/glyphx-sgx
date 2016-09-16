
#version 330 core
 
layout(std140) uniform instance_data
{
	mat4 transform;
	vec4 color;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
out vec2 _uv;
out vec4 _color;

void main()
{
	_uv = texcoord;
	_color = color;
    gl_Position = transform * vec4( position, 1 );
}
