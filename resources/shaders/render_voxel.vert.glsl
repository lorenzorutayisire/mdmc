#version 430

layout(binding = 0, rgba32f) uniform image3D u_voxel;

out vec4 v_position;

void main()
{
	ivec3 voxel_size = imageSize(u_voxel);
	v_position = vec4(
		(gl_VertexID % voxel_size.x),
		(gl_VertexID / voxel_size.x) % voxel_size.y,
		(gl_VertexID / (voxel_size.x * voxel_size.z)),
		1.0
	);
	gl_Position = v_position;
}
