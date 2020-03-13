#include "volume.hpp"

Volume::Volume(glm::uvec3 size, GLenum format) :
	size(size),
	format(format),

	texture3d([]() {
		GLuint name;
		glGenTextures(1, &name);
		return name;
	}())
{
	glBindTexture(GL_TEXTURE_3D, this->texture3d);

	glTexStorage3D(GL_TEXTURE_3D, 1, format, size.x, size.y, size.z);
	
	glClearTexImage(GL_TEXTURE_3D, 1, format, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_3D, 0);
}

Volume::~Volume()
{
	glDeleteTextures(1, &this->texture3d);
}
