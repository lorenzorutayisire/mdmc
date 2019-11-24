#version 450

in vec3 position;
in vec2 tex_coord;
uint tex_id;

uniform mat4 u_transform;
uniform mat4 u_camera;

out vec2 v_tex_coord;
flat out uint v_tex_id;

void main()
{
	gl_Position = u_camera * u_transform * vec4(position, 1.0);
	v_tex_coord = tex_coord;
	v_tex_id = tex_id;
}
