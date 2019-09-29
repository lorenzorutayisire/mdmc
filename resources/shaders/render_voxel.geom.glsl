#version 430

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

uniform mat4 u_camera;
uniform mat4 u_transform;

in vec4 v_voxel_coords[];

out vec4 g_voxel_coords;

void emit_offset_vertex(float x_offset, float y_offset, float z_offset)
{
	gl_Position = u_camera * u_transform * vec4(v_voxel_coords[0].x + x_offset, v_voxel_coords[0].y + y_offset, v_voxel_coords[0].z + z_offset, 1);
	EmitVertex();
}

void main()
{
	g_voxel_coords = v_voxel_coords[0];

	// Front
	emit_offset_vertex(0, 1, 1); // Front top left
	emit_offset_vertex(1, 1, 1); // Front top right
	emit_offset_vertex(0, 0, 1); // Front bottom left
	EndPrimitive();
	
	emit_offset_vertex(1, 1, 1); // Front top right
	emit_offset_vertex(1, 0, 1); // Front bottom right
	emit_offset_vertex(0, 0, 1); // Front bottom left
	EndPrimitive();

	// Back
	emit_offset_vertex(0, 1, 0); // Back top left
	emit_offset_vertex(1, 1, 0); // Back top right
	emit_offset_vertex(0, 0, 0); // Back bottom left
	EndPrimitive();
	
	emit_offset_vertex(1, 1, 0); // Back top right
	emit_offset_vertex(1, 0, 0); // Back bottom right
	emit_offset_vertex(0, 0, 0); // Back bottom left
	EndPrimitive();

	// Top
	emit_offset_vertex(0, 1, 1); // Front top left
	emit_offset_vertex(0, 1, 0); // Back top left
	emit_offset_vertex(1, 1, 0); // Back top right
	EndPrimitive();
	
	emit_offset_vertex(1, 1, 0); // Back top right
	emit_offset_vertex(1, 1, 1); // Front top right
	emit_offset_vertex(0, 1, 1); // Front top left
	EndPrimitive();

	// Bottom
	emit_offset_vertex(0, 0, 1); // Front bottom left
	emit_offset_vertex(0, 0, 0); // Back bottom left
	emit_offset_vertex(1, 0, 0); // Back bottom right
	EndPrimitive();

	emit_offset_vertex(1, 0, 0); // Back bottom right
	emit_offset_vertex(1, 0, 1); // Front bottom right
	emit_offset_vertex(0, 0, 1); // Front bottom left
	EndPrimitive();

	// Left
	emit_offset_vertex(0, 0, 0); // Back bottom left
	emit_offset_vertex(0, 1, 0); // Back top left
	emit_offset_vertex(0, 1, 1); // Front top left
	EndPrimitive();

	emit_offset_vertex(0, 1, 1); // Front top left
	emit_offset_vertex(0, 0, 1); // Front bottom left
	emit_offset_vertex(0, 0, 0); // Back bottom left
	EndPrimitive();
	
	// Right
	emit_offset_vertex(1, 0, 0); // Back bottom right
	emit_offset_vertex(1, 1, 0); // Back top right
	emit_offset_vertex(1, 1, 1); // Front top right
	EndPrimitive();

	emit_offset_vertex(1, 1, 1); // Front top right
	emit_offset_vertex(1, 0, 1); // Front bottom right
	emit_offset_vertex(1, 0, 0); // Back bottom right
	EndPrimitive();
}
