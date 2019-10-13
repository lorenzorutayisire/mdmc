#version 450

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0, rgba32f) uniform image3D u_voxel;
layout(binding = 1, rgba8) uniform image2DArray u_minecraft_avg;
layout(binding = 2, r32ui) uniform uimage3D u_minecraft_blocks;

uint find_nearest_minecraft_block_id(vec4 color)
{
	float nearest_distance = -1;
	uint nearest_block_id;
	
	for (uint block_id = 0; block_id < imageSize(u_minecraft_avg).z; block_id++)
	{
		vec4 avg_color = imageLoad(u_minecraft_avg, ivec3(0, 0, block_id));
		float cmp_distance = distance(color, avg_color);
		if (nearest_distance < 0 || cmp_distance < nearest_distance)
		{
			nearest_distance = cmp_distance;
			nearest_block_id = block_id;
		}
	}

	return nearest_block_id;
}

void main()
{
	ivec3 voxel_coord = ivec3(gl_WorkGroupID);
	vec4 voxel_color = imageLoad(u_voxel, voxel_coord);
	if (voxel_color.a > 0)
	{
		uint nearest_block_id = find_nearest_minecraft_block_id(voxel_color);
		imageStore(u_minecraft_blocks, voxel_coord, uvec4(nearest_block_id));
	}
}
