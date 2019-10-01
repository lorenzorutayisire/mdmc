// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

//Geometry shader for scene voxelization
//Using the method described in Ch. 22, OpenGL Insights

# version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 v_vertex[];
in vec3 v_normal[];
in vec2 v_tex_coord[];

out vec3 g_position;
out vec3 g_normal;
out vec2 g_tex_coord;

flat out int g_axis;   //indicate which axis the projection uses
flat out vec4 g_AABB;

uniform mat4 u_MVPx;
uniform mat4 u_MVPy;
uniform mat4 u_MVPz;

layout(location = 2) uniform ivec3 u_voxel_size;

void main()
{
	vec3 faceNormal = normalize( cross( v_vertex[1]-v_vertex[0], v_vertex[2]-v_vertex[0] ) );
	float NdotXAxis = abs( faceNormal.x );
	float NdotYAxis = abs( faceNormal.y );
	float NdotZAxis = abs( faceNormal.z );
	mat4 proj;

	//Find the axis the maximize the projected area of this triangle
	if( NdotXAxis > NdotYAxis && NdotXAxis > NdotZAxis)
    {
	    proj = u_MVPx;
		g_axis = 1;
	}
	else if( NdotYAxis > NdotXAxis && NdotYAxis > NdotZAxis)
    {
	    proj = u_MVPy;
		g_axis = 2;
    }
	else
    {
	    proj = u_MVPz;
		g_axis = 3;
	}

	vec4 pos[3];

	// Transforms the vertices to clip space, in order to get the voxels.
	pos[0] = proj * gl_in[0].gl_Position;
	pos[1] = proj * gl_in[1].gl_Position;
	pos[2] = proj * gl_in[2].gl_Position;

	// Enlarge the triangle for conservative rasterization.
	vec4 AABB;
	vec2 hPixel = vec2( 1.0/u_voxel_size.x, 1.0/u_voxel_size.y ); // ?
	float pl = 1.4142135637309 / u_voxel_size.x ;
	
	// Calculates the AABB of this triangle.
	AABB.xy = pos[0].xy;
	AABB.zw = pos[0].xy;

	AABB.xy = min( pos[1].xy, AABB.xy );
	AABB.zw = max( pos[1].xy, AABB.zw );
	
	AABB.xy = min( pos[2].xy, AABB.xy );
	AABB.zw = max( pos[2].xy, AABB.zw );

	// enlarge the aabb of half pixel
	AABB.xy -= hPixel;
	AABB.zw += hPixel;

	g_AABB = AABB;

	//find 3 triangle edge plane
    vec3 e0 = vec3( pos[1].xy - pos[0].xy, 0 );
	vec3 e1 = vec3( pos[2].xy - pos[1].xy, 0 );
	vec3 e2 = vec3( pos[0].xy - pos[2].xy, 0 );
	vec3 n0 = cross( e0, vec3(0,0,1) );
	vec3 n1 = cross( e1, vec3(0,0,1) );
	vec3 n2 = cross( e2, vec3(0,0,1) );

	// dilates the triangle's vertices
	pos[0].xy = pos[0].xy + pl*( (e2.xy/dot(e2.xy,n0.xy)) + (e0.xy/dot(e0.xy,n2.xy)) );
	pos[1].xy = pos[1].xy + pl*( (e0.xy/dot(e0.xy,n1.xy)) + (e1.xy/dot(e1.xy,n0.xy)) );
	pos[2].xy = pos[2].xy + pl*( (e1.xy/dot(e1.xy,n2.xy)) + (e2.xy/dot(e2.xy,n1.xy)) );

	// emits the enlarged triangle
	gl_Position = pos[0];
	g_position = pos[0].xyz;
	g_normal = v_normal[0];
	g_tex_coord = v_tex_coord[0];
	EmitVertex();

	gl_Position = pos[1];
	g_position = pos[1].xyz;
	g_normal = v_normal[1];
	g_tex_coord = v_tex_coord[1];
	EmitVertex();

	gl_Position = pos[2];
	g_position = pos[2].xyz;
	g_normal = v_normal[2];
	g_tex_coord = v_tex_coord[2];
	EmitVertex();

	EndPrimitive();
}