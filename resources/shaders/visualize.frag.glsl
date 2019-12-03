#version 430

in vec2 v_tex_coord;
in vec4 v_color;

layout(location = 3) uniform vec4 u_color;
uniform sampler2D u_texture2d;

out vec4 frag_color;

void main()
{
	frag_color = u_color * v_color * texture(u_texture2d, v_tex_coord);
	if (frag_color.a == 0)
		discard;
}
