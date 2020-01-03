# version 430

in vec3 g_position;
in vec2 g_tex_coord;

flat in int g_axis;
flat in vec4 g_aabb;

layout(location = 6) uniform vec4 u_color;
layout(location = 7, binding = 0) uniform sampler2D u_texture2d;

layout(location = 5, binding = 5, rgba8) uniform image3D u_voxel;

uniform float u_viewport_side; // = largest_side * resolution
uniform vec3 u_volume_size;

void main()
{
    if (g_position.x < g_aabb.x || g_position.y < g_aabb.y || g_position.x > g_aabb.z || g_position.y > g_aabb.w)
	   discard;

	ivec3 position = ivec3(gl_FragCoord.xy, gl_FragCoord.z * u_viewport_side);

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

	// Write the Voxel in the Volume.

	vec4 color = u_color * texture(u_texture2d, g_tex_coord);
	imageStore(u_voxel, position, color);
}
