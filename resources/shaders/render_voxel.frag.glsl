#version 430

in vec4 g_position;

layout(binding = 0, rgba32f) uniform image3D u_voxel;

void main()
{
	vec4 color = imageLoad(u_voxel, ivec3(g_position.xyz));
	if (color.a == 0)
	{
		discard;
	}
	gl_FragColor = color;
}
