#version 430

// Reference:
// https://github.com/otaku690/SparseVoxelOctree/blob/master/WIN/SVO/shader/voxelize.geom.glsl

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 v_position[];
in vec2 v_tex_coord[];

out vec3 g_position;
out vec2 g_tex_coord;

flat out int g_axis;
flat out vec4 g_aabb;

uniform mat4 u_x_ortho_projection;
uniform mat4 u_y_ortho_projection;
uniform mat4 u_z_ortho_projection;

layout(location = 2) uniform float u_voxel_size;

void main()
{
	/* Project */

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

	vec3 p0 = (projection * vec4(v_position[0], 1)).xyz;
	vec3 p1 = (projection * vec4(v_position[1], 1)).xyz;
	vec3 p2 = (projection * vec4(v_position[2], 1)).xyz;

	/* Enlarge */

	float half_pixel = 1.0 / u_voxel_size;
	float pl = 1.4142135637309 / u_voxel_size;
	vec4 aabb;
	
	aabb.xy = p0.xy;
	aabb.zw = p1.xy;

	aabb.xy = min(p1.xy, aabb.xy );
	aabb.zw = max(p1.xy, aabb.zw );
	
	aabb.xy = min(p2.xy, aabb.xy );
	aabb.zw = max(p2.xy, aabb.zw );

	aabb.xy -= half_pixel;
	aabb.zw += half_pixel;

	g_aabb = aabb;

	vec3 e0 = vec3(p1.xy - p0.xy, 0);
	vec3 e1 = vec3(p2.xy - p1.xy, 0);
	vec3 e2 = vec3(p0.xy - p2.xy, 0);

	vec3 n0 = cross(e0, vec3(0, 0, 1));
	vec3 n1 = cross(e1, vec3(0, 0, 1));
	vec3 n2 = cross(e2, vec3(0, 0, 1));

	p0.xy = p0.xy + pl * ((e2.xy / dot(e2.xy, n0.xy)) + (e0.xy / dot(e0.xy, n2.xy)));
	p1.xy = p1.xy + pl * ((e0.xy / dot(e0.xy, n1.xy)) + (e1.xy / dot(e1.xy, n0.xy)));
	p2.xy = p2.xy + pl * ((e1.xy / dot(e1.xy, n2.xy)) + (e2.xy / dot(e2.xy, n1.xy)));

	/* Emit */

	gl_Position = vec4(p0, 1);
	g_position = p0;
	g_tex_coord = v_tex_coord[0];
	EmitVertex();
	
	gl_Position = vec4(p1, 1);
	g_position = p1;
	g_tex_coord = v_tex_coord[1];
	EmitVertex();
	
	gl_Position = vec4(p2, 1);
	g_position = p2;
	g_tex_coord = v_tex_coord[2];
	EmitVertex();

	EndPrimitive();
}