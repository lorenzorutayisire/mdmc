#version 450

in vec2 v_tex_coord;
flat in uint v_tex_id;

uniform vec4 u_color;
uniform sampler2DArray u_texture2d;

out vec4 frag_color;

void main()
{
	frag_color = u_color * texture(u_texture2d, vec3(v_tex_coord, v_tex_id));
}
