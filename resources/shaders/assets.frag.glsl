#version 450

in vec2 v_uv;
flat in float v_texture_id;
flat in vec4 v_tint;

uniform sampler2DArray u_textures;

out vec4 frag_color;

void main()
{
	frag_color = v_tint * texture(u_textures, vec3(v_uv, v_texture_id));
}
