# version 430

// flat = do not perform interpolation for this value.
flat in vec4 f_aabb;

in vec3 f_position;
in vec2 f_texcoords;

uniform vec4 u_color;
uniform sampler2D u_texture2d;

uniform ivec3 u_voxel_size;
uniform layout(binding = 0, rgba8) imageBuffer u_voxel;

void main()
{
	// If the position is outside the AABB retrieved previously, discards the fragment.
    if (f_position.x < f_aabb.x || f_position.y < f_aabb.y || f_position.x > f_aabb.z || f_position.y > f_aabb.w )
	   discard;

	// Gets the 3D position of the fragment and flattens it to 1D.
	ivec3 voxel_position = ivec3(gl_FragCoord.xy, gl_FragCoord.z * u_voxel_size.z);
	int index = voxel_position.x + u_voxel_size.x * (voxel_position.y + u_voxel_size.z * voxel_position.z);
	
	// Gets the color of the fragment and stores it in the voxel.
	vec4 color = u_color * texture(u_texture2d, f_texcoords);
	imageStore(u_voxel, index, color);

	gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
