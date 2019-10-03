#version 430

in vec3 v_tex_coord;
in vec3 v_normal;

uniform float u_voxel_size; // The size of a single voxel.
layout(binding = 0) uniform sampler3D u_voxel;

void main()
{
	vec3 before = v_tex_coord - v_normal * (u_voxel_size / 2);
	vec3 after = v_tex_coord + v_normal * (u_voxel_size / 2);

	vec4 before_color = texture(u_voxel, before);
	vec4 after_color = texture(u_voxel, after);

	if (before_color.a == 0 && after_color.a == 0)
	{
		discard;
	}

	if (before_color.a == 0)
	{
		gl_FragColor = after_color;
	}
	else
	{
		gl_FragColor = before_color;
	}
}
