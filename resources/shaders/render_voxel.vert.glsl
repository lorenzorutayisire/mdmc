#version 430

layout(binding = 0, rgba32f) uniform image3D u_voxel;

uniform mat4 u_camera;
uniform mat4 u_transform;

out vec4 v_voxel_coords;

void main()
{
	ivec3 voxel_size = imageSize(u_voxel);
	v_voxel_coords = vec4(
		(gl_VertexID % voxel_size.x),
		(gl_VertexID / voxel_size.x) % voxel_size.y,
		(gl_VertexID / (voxel_size.x * voxel_size.z)),
		1.0
	);
	gl_Position = u_camera * u_transform * v_voxel_coords;
}
