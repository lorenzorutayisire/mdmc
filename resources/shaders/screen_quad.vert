#version 330

layout(location = 0) in vec2 u_position;

void main()
{
	gl_Position = vec4(u_position, 1, 1);
};
