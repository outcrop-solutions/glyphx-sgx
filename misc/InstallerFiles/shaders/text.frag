
#version 330 core
 
out vec4 outputF;

in vec2 _uv;
in vec4 _color;
uniform sampler2D base_texture;

void main()
{
	float coverage = texture( base_texture, _uv ).r;

	// todo: proper blending computation (see freetype documentation)
	outputF =  _color;
	outputF.a *= coverage;
}
