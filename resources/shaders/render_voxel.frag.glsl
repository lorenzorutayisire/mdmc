#version 430

in vec3 v_tex_coord;
in vec3 v_normal;

uniform uint u_color_mode;

uniform float u_voxel_size;

layout(binding = 0) uniform sampler3D u_voxel;
layout(binding = 1, r32ui) uniform uimage3D u_minecraft_blocks;

layout(binding = 2) uniform sampler2DArray u_minecraft_avg;
layout(binding = 3) uniform sampler2DArray u_minecraft_textures;

vec4 get_texture3d_color(sampler3D texture3d)
{
	vec3 before = v_tex_coord - v_normal * (u_voxel_size / 2);
	vec3 after = v_tex_coord + v_normal * (u_voxel_size / 2);

	vec4 before_color = texture(texture3d, before);
	vec4 after_color = texture(texture3d, after);

	if (before_color.a == 0 && after_color.a == 0)
		discard;

	if (before_color.a == 0)
		return after_color;
	
	if (after_color.a == 0)
		return before_color;
}

vec4 get_scene3d_color()
{
	return get_texture3d_color(u_voxel);
}

uint get_minecraft_block()
{
	ivec3 before = ivec3(v_tex_coord - v_normal / 2);
	ivec3 after = ivec3(v_tex_coord + v_normal / 2);

	uint prev_block = imageLoad(u_minecraft_blocks, before).r;
	uint next_block = imageLoad(u_minecraft_blocks, after).r;

	if (prev_block == 0 && next_block == 0)
		discard;

	if (prev_block == 0)
		return next_block;

	if (next_block == 0)
		return prev_block;
}

vec4 get_minecraft_avg_color()
{
	return texture(u_minecraft_avg, vec3(0, 0, get_minecraft_block()));
}

vec4 get_minecraft_texture_color()
{
	return texture(u_minecraft_textures, v_tex_coord);
}

void main()
{
	switch (u_color_mode)
	{
	case 1:
		gl_FragColor = get_minecraft_avg_color();
		break;
	case 2:
		gl_FragColor = get_minecraft_texture_color();
		break;
	default: // 0
		gl_FragColor = get_scene3d_color();
		break;
	}
}
