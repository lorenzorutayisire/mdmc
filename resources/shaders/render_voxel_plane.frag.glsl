#version 430

in vec3 v_position;

uniform vec3 u_voxel_size;
layout(binding = 0) uniform sampler3D u_voxel;

void main()
{
	// v_position is interpolated and now represents a point within the voxel.
	// Since uvw texture coordinates goes from 0.0 to 1.0, we need to normalize it.

	vec4 color = texture(u_voxel, v_position / u_voxel_size);

	// If the pixel indexed is empty, discards the fragment.
	// Otherwise colors it.

	if (color.a == 0)
	{
		discard;
	}
	gl_FragColor = color;
}
