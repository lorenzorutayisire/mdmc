# version 430

flat in int g_axis;

in vec3 g_position;
in vec2 g_tex_coord;

uniform vec4 u_color;
layout(binding = 0) uniform sampler2D u_texture2d;

layout(location = 5, binding = 5, rgba32f) uniform image3D u_voxel;
layout(location = 2) uniform ivec3 u_voxel_size;

void main()
{
	ivec3 position = ivec3(gl_FragCoord.xy, gl_FragCoord.z * u_voxel_size.z);

	switch (g_axis)
	{
	case 0: // X
		position.zyx = position.xyz;
		break;
	case 1: // Y
		position.xzy = ivec3(position.x, -position.y, position.z);
		break;
	case 2: // Z
		position.xyz = position.xyz;
		break;
	}

	vec4 color = u_color * texture(u_texture2d, g_tex_coord);
	imageStore(u_voxel, position, color);

	//gl_FragColor = color;
}
