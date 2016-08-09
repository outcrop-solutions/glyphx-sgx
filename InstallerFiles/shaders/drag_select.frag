
#version 330 core
 
out vec4 outputF;
 
layout(std140) uniform material
{
	vec4 tint_color;
};

void main()
{
    outputF = tint_color;
}
