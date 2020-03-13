#include "atlas.hpp"

Atlas::Atlas() :
	name([]() {
		GLuint name;
		glGenTextures(1, &name);
		return name;
	}()),
	width(0),
	height(0)
{}

Atlas::~Atlas()
{
	glDeleteTextures(1, &this->name);
}
