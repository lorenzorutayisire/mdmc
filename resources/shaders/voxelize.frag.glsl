# version 430

in vec3 g_position;
in vec2 g_tex_coord;

flat in int g_axis;
flat in vec4 g_aabb;

uniform vec4 u_color;
layout(binding = 0) uniform sampler2D u_texture2d;

layout(location = 5, binding = 5, rgba32f) uniform image3D u_voxel;
layout(location = 2) uniform float u_voxel_size;

void main()
{
    if (g_position.x < g_aabb.x || g_position.y < g_aabb.y || g_position.x > g_aabb.z || g_position.y > g_aabb.w)
	   discard;

	ivec3 position = ivec3(gl_FragCoord.xy, gl_FragCoord.z * u_voxel_size);

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
		position.z = int(u_voxel_size) - position.z;
		break;
	}

	vec4 color = u_color * texture(u_texture2d, g_tex_coord);
	imageStore(u_voxel, position, color);

	//gl_FragColor = color;
}
