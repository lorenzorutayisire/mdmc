#version 450

#define DEBUG

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(location = 0) uniform uint resolution; // <= 9
layout(location = 1) uniform uint blocks_count;

layout(binding = 0, rgba8) uniform image3D model;
layout(binding = 1, rgba8) uniform image3D blocks;

layout(binding = 2, rgba8) uniform image3D mapped_model;
layout(binding = 3, r32ui) uniform uimage3D mc_schematic;

vec4 get_model_color_at(uvec3 offset)
{
	return imageLoad(model, ivec3(gl_WorkGroupID * resolution + offset));
}

vec4 get_block_color_at(uint block_id, uvec3 offset)
{
	uvec3 position = uvec3(
		(block_id % 16),
		(block_id / (16 * 16)),
		(block_id / 16) % 16
	);

	position *= 2;

	return imageLoad(blocks, ivec3(position * resolution + offset));
}

float get_distance_from(uint block_id)
{
	float _distance = 0;

	for (uint x = 0; x < resolution; x++)
	{
		for (uint y = 0; y < resolution; y++)
		{
			for (uint z = 0; z < resolution; z++)
			{
				uvec3 offset = uvec3(x, y, z);

				vec4 block_color = get_block_color_at(block_id, offset);
				_distance += distance(get_model_color_at(offset), block_color);
			}
		}
	}

	return _distance;
}

uint get_nearest_block()
{
	uint nearest_block_id;
	float min_distance = -1;

	for (uint block_id = 0; block_id < blocks_count; block_id++)
	{
		float _distance = get_distance_from(block_id);

		if (min_distance < 0 || min_distance > _distance)
		{
			min_distance = _distance;
			nearest_block_id = block_id;
		}
	}

	return nearest_block_id;
}

void main()
{
	uint nearest_block_id = get_nearest_block();

	imageStore(mc_schematic, ivec3(gl_WorkGroupID), uvec4(nearest_block_id));

#ifdef DEBUG
	for (uint x = 0; x < resolution; x++)
	{
		for (uint y = 0; y < resolution; y++)
		{
			for (uint z = 0; z < resolution; z++)
			{
				uvec3 offset = uvec3(x, y, z);
				imageStore(mapped_model, ivec3(gl_WorkGroupID * resolution + offset), get_block_color_at(nearest_block_id, offset));
			}
		}
	}
#endif
}

