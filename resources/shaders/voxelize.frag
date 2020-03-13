# version 460

in vec3 g_position;
in vec2 g_tex_coord;

flat in int g_axis;
flat in vec4 g_aabb;

layout (pixel_center_integer) in vec4 gl_FragCoord;

layout(location = 6) uniform vec4 u_color;
layout(location = 7, binding = 0) uniform sampler2D u_texture2d;

uniform uint u_viewport;
uniform uvec3 u_grid;

layout(binding = 1, rgba8) uniform imageBuffer u_volume;

void main()
{
	uvec3 position = uvec3(gl_FragCoord.xy, gl_FragCoord.z * u_viewport);

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
		position.z = u_viewport - position.z;
		break;
	}

	// If the target Voxel position is outside the Volume size, discard it.
	// The position XYZ shouldn't be negative, so check only positive out-bounds.

	if (position.x > u_grid.x || position.y > u_grid.y || position.z > u_grid.z)
		discard;

	vec4 color = u_color * texture2D(u_texture2d, g_tex_coord);

	uint position_i = x + WIDTH * (y + DEPTH * z)

	imageStore(u_volume, ivec3(position), color);
}
