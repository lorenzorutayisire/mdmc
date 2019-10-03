#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 v_position[];
in vec2 v_tex_coord[];

out vec3 g_position;
out vec2 g_tex_coord;

flat out int g_axis;

uniform mat4 u_x_ortho_projection;
uniform mat4 u_y_ortho_projection;
uniform mat4 u_z_ortho_projection;

layout(location = 2) uniform ivec3 u_voxel_size;

void main()
{	
	// Chooses the projection that gives the wider triangle area:
	// takes the normal vector of the face and checks which is its longer axis.

	vec3 face_normal = abs(normalize(cross(v_position[1] - v_position[0], v_position[2] - v_position[0])));
	mat4 projection;

	if (face_normal.x > face_normal.y && face_normal.x > face_normal.z)
    {
	    projection = u_x_ortho_projection;
		g_axis = 0;
	}
	else if (face_normal.y > face_normal.x && face_normal.y > face_normal.z)
    {
	    projection = u_y_ortho_projection;
		g_axis = 1;
    }
	else
    {
	    projection = u_z_ortho_projection;
		g_axis = 2;
	}

	// Emits the projected triangle.

	for (int i = 0; i < 3; i++)
	{
		gl_Position = projection * vec4(v_position[i], 1.0);
		g_position = v_position[i];
		g_tex_coord = v_tex_coord[i];
		EmitVertex();
	}

	EndPrimitive();
}