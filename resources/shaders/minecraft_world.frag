#version 330

in vec2 v_uv;
flat in float v_tint_index;

uniform sampler2D u_texture2d;
uniform vec4 u_tint_color;

out vec4 f_color;

void main()
{
	vec2 uv = v_uv / vec2(textureSize(u_texture2d, 0));
	f_color = (v_tint_index == 1.0 ? u_tint_color : vec4(1)) * texture(u_texture2d, uv);
}
