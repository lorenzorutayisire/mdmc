#version 430

in vec3 position;
in vec3 normal;

uniform mat4 u_transform;
uniform mat4 u_camera;

out vec3 v_tex_coord;
flat out vec3 v_normal;

void main()
{
	gl_Position = u_camera * u_transform * vec4(position, 1.0);
	v_tex_coord = position;
	v_normal = normal;
}
