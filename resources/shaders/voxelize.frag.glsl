# version 430

in vec3 v_position;
in vec2 v_tex_coords;

uniform vec4 u_color;
layout(binding = 0) uniform sampler2D u_texture2d;

layout(location = 5, binding = 5, rgba32f) uniform image3D u_voxel;
layout(location = 2) uniform ivec3 u_voxel_size;

void main()
{
	ivec3 position = ivec3(gl_FragCoord.xy, gl_FragCoord.z * u_voxel_size.z);
	vec4 color = u_color * texture(u_texture2d, v_tex_coords);

	imageStore(u_voxel, position, color);
	//gl_FragColor = color;
}
