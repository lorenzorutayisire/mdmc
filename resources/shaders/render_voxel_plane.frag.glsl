#version 430

in vec3 v_position;

uniform vec3 u_voxel_size;
layout(binding = 0) uniform sampler3D u_voxel;

void main()
{
	vec4 color = texture(u_voxel, v_position / u_voxel_size);
	if (color.a == 0)
	{
		discard;
	}
	gl_FragColor = color;
}
