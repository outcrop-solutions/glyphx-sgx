
#version 330 core
 
layout(std140) uniform camera_data
{
	mat4 view, proj;
	vec3 eyevec;
};

layout(std140) uniform global_data
{
	float scale;
};

layout(std140) uniform instance_data
{
	mat4 world;
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

	vec4 pos = vec4( position.xyz * scale, 1 );

    gl_Position = proj * view * world * pos;
}
