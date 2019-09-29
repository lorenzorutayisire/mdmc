#version 430

in vec3 position;

uniform mat4 u_transform;
uniform mat4 u_camera;

out vec3 v_position;

void main()
{
	v_position = position;
	gl_Position = u_camera * u_transform * vec4(position, 1.0);
}
