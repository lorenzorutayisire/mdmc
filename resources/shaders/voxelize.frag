# version 460

in vec3 g_position;
in vec2 g_tex_coord;

flat in int g_axis;
flat in vec4 g_aabb;

layout (pixel_center_integer) in vec4 gl_FragCoord;

layout(location = 6) uniform vec4 u_color;
layout(location = 7, binding = 0) uniform sampler2D u_texture2d;

uniform float u_viewport_side; // = largest_side * resolution
uniform vec3 u_volume_size;

uniform int fragment_table_capacity;

uniform uint can_store = 0;

layout(binding = 1) uniform atomic_uint fragment_count;

struct Voxel
{
	uvec4 position;
	vec4 color;
};

layout(std430, binding = 2) buffer volume_buffer
{
	Voxel volume[];
};

void main()
{
	ivec4 position = ivec4(gl_FragCoord.xy, gl_FragCoord.z * u_viewport_side, 1);

	switch (g_axis)
	{
	case 0: // X
		position.xyz = position.zyx;
		break;
	case 1: // Y
		position.xyz = position.xzy;
		break;
	case 2: // Z
		position.xy = position.xy;
		position.z = int(u_viewport_side) - position.z;
		break;
	}

	// If the target Voxel position is outside the Volume size, discard it.
	// The position XYZ shouldn't be negative, so check only positive out-bounds.

	if (position.x > u_volume_size.x || position.y > u_volume_size.y || position.z > u_volume_size.z)
		discard;
		
	uint index = atomicCounterIncrement(fragment_count);

	vec4 color = u_color * texture(u_texture2d, g_tex_coord);
	
	if (can_store == 1)
	{
		volume[index].position = position;
		volume[index].color = color;
	}
}
