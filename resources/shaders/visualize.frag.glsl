#version 430

in vec2 v_tex_coord;

layout(location = 3) uniform vec4 u_color;
uniform sampler2D u_texture2d;

void main()
{
	gl_FragColor = u_color * texture(u_texture2d, v_tex_coord);
}
