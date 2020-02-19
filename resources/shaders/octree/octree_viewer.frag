#version 460

struct Camera
{
	vec3 position;
	vec3 direction;

	float fov;
};

uniform Camera camera;

struct Octree
{
	vec3 position;
	uint size; // 2^n

	uint depth;
};

uniform Octree octree;
layout(binding = 0, r32ui) uniform uimageBuffer octree_buffer;

void ray_march(vec3 origin, vec3 direction)
{
	const float near_plane = 1.0;
	const float far_plane = 100.0;

	float _distance = near_plane;
	uint level = 0;
	do
	{

		float hop = octree.size / pow(2, level);
		_distance += hop;

	} while (_distance < far_plane);

	while (_distance < far_plane)
	{
		vec3 position = origin + direction * _distance;
	}
}

void main()
{
	
}

