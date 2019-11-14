#version 430

in vec3 v_tex_coord;
flat in vec3 v_normal;

uniform uint u_color_mode;

uniform float u_voxel_size;

layout(binding = 0) uniform sampler3D u_voxel;
layout(binding = 1, r32ui) uniform uimage3D u_minecraft_blocks;

layout(binding = 2) uniform sampler2DArray u_minecraft_avg;
layout(binding = 3) uniform sampler2DArray u_minecraft_textures;

in float flogz;
in float Fcoef;

vec4 get_voxel_color()
{
	vec3 before = (v_tex_coord - v_normal / 2) / u_voxel_size;
	vec3 after = (v_tex_coord + v_normal / 2) / u_voxel_size;

	vec4 before_color = texture(u_voxel, before);
	vec4 after_color = texture(u_voxel, after);

	if (before_color.a == 0)
		return after_color;
	if (after_color.a == 0)
		return before_color;
	else
		return vec4(0);
}

//
// Gets minecraft block id based on the coords of the current vertex.
// If the next block is 0 then the current block has to be shown.
//
uint get_minecraft_block_id()
{
	ivec3 prev_block_coord = ivec3(floor(v_tex_coord - v_normal / 2));
	ivec3 next_block_coord = ivec3(floor(v_tex_coord + v_normal / 2));
	
	uint prev_block_id = imageLoad(u_minecraft_blocks, prev_block_coord).r;
	uint next_block_id = imageLoad(u_minecraft_blocks, next_block_coord).r;
	
	if (prev_block_id == 0)
		return next_block_id;
	else if (next_block_id == 0)
		return prev_block_id;
	else
		return 0; // neither of them is 0
}

//
// Using the block id retrieved, gets the avg color of the fragment.
//
vec4 get_minecraft_avg_color()
{
	uint block_id = get_minecraft_block_id();

	if (block_id == 0)
		discard;

	return texture(u_minecraft_avg, vec3(0, 0, block_id));
}

//
// Using the block id retrieved, maps the texture of this fragment.
//
vec4 get_minecraft_texture_color()
{
	uint block_id = get_minecraft_block_id();

	if (block_id == 0)
		discard;

	vec2 tex_coord;

	if (v_normal.x > 0)
		tex_coord = v_tex_coord.yz;
	else if (v_normal.y > 0)
		tex_coord = v_tex_coord.xz;
	else // (v_normal.z > 0)
		tex_coord = v_tex_coord.xy;

	return texture(u_minecraft_textures, vec3(tex_coord - floor(tex_coord), block_id));
}

void main()
{
	vec4 color;
	switch (u_color_mode)
	{
	default: // KEY_1
		color = get_voxel_color();
		break;
	case 1: // KEY_2
		color = get_minecraft_avg_color();
		break;
	case 2: // KEY_3
		color = get_minecraft_texture_color();
		break;
	}
	if (color.a == 0)
	{
		discard;
	}
	
	const float Fcoef_half = Fcoef / 2;
    gl_FragDepth = log2(flogz) * Fcoef_half;
	
	gl_FragColor = color;
}
