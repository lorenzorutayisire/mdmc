#pragma once

#include <unordered_set>
#include <memory>

#include <GL/glew.h>

#include <glm/glm.hpp>

#ifdef IGNORED

// std::hash<glm::ivec3>
namespace std
{
	template<>
	struct hash<glm::ivec3>
	{
		size_t operator()(glm::ivec3 const& v) const
		{
			return std::hash<int>()(v.x) ^ std::hash<int>()(v.y) ^ std::hash<int>()(v.z);
		}

		bool operator()(glm::ivec3 const& a, glm::ivec3 const& b) const
		{
			return a.x == b.x && a.y == b.y && a.z == b.z;
		}
	};
}

struct MinecraftWorld
{
public:
	enum VertexAttribute
	{
		POSITION,
		UV,
		TINT_INDEX
	};

private:
	std::unordered_map<glm::ivec3, MinecraftBlockStateVariant const*> block_by_position;

	GLuint vao, vbo;
	size_t vertices_count = 0;

public:
	std::shared_ptr<MinecraftContext const> const context;

	MinecraftWorld(std::shared_ptr<MinecraftContext const> const& context);
	MinecraftWorld(MinecraftWorld const&) = delete;
	MinecraftWorld(MinecraftWorld const&&) = delete;

	~MinecraftWorld();

	MinecraftBlockStateVariant const* get_block(glm::ivec3 const& position) const;
	void set_block(const glm::ivec3& position, const MinecraftBlockStateVariant& block_state_variant, bool rebuild = false);

	void build();
	void draw();
};

#endif
