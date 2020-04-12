#version 330

in vec2 v_uv;
flat in float v_tint_index;

uniform sampler2D u_texture2d;
uniform vec4 u_tint_color;

out vec4 f_color;

void main()
{
	vec2 texture_size = textureSize(u_texture2d, 0);
	vec2 uv = v_uv / texture_size;
	vec4 color = (v_tint_index != 0 ? u_tint_color : vec4(1)) * texture(u_texture2d, uv);

	if (color.a == 0)
		discard;

	f_color = color;
}
