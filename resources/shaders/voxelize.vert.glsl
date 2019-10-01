#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coord;

uniform mat4 u_transform;
uniform mat4 u_camera;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_tex_coord;

void main()
{
    gl_Position = u_camera * u_transform * vec4(position, 1);
	v_position = gl_Position.xyz;
	v_normal = normal;
	v_tex_coord = tex_coord;
}
