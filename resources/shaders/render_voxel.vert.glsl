#version 430

in vec3 position;
in vec3 normal;

uniform mat4 u_transform;
uniform mat4 u_camera;

out vec3 v_tex_coord;
flat out vec3 v_normal;

out float flogz;
out float Fcoef;

const float far_plane = 100000.0;

void main()
{
	gl_Position = u_camera * u_transform * vec4(position, 1.0);
	
	Fcoef = 2.0 / log2(far_plane + 1.0);
	gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * Fcoef - 1.0;
	
    flogz = 1.0 + gl_Position.w;
	
	v_tex_coord = position;
	v_normal = normal;
}
