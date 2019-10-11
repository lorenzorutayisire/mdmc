#version 450

layout(binding = 0, rgba32f) uniform image3D u_voxel;

layout(binding = 1, rgba32f) uniform image2DArray u_minecraft_avg;
layout(binding = 2, r32ui)   uniform uimage3D u_minecraft_blocks;

uint find_nearest_minecraft_block(vec4 color)
{
	float nearest_distance = -1;
	uint nearest_block;
	
	for (uint i = 0; i < imageSize(u_minecraft_avg).z; i++)
	{
		vec4 avg_color = imageLoad(u_minecraft_avg, ivec3(0, 0, i));
		float d = distance(color, avg_color);
		if (nearest_distance < 0 || d < nearest_distance)
		{
			nearest_distance = d;
			nearest_block = i;
		}
	}

	return nearest_block;
}

void main()
{
	vec4 voxel_color = imageLoad(u_voxel, ivec3(gl_WorkGroupID));
	if (voxel_color.a > 0)
	{
		uint nearest_block = find_nearest_minecraft_block(voxel_color);
		imageStore(u_minecraft_blocks, ivec3(gl_WorkGroupID), uvec4(nearest_block));
	}
}
