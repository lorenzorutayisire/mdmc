#include "mc_assets.hpp"

#include <stdexcept>
#include <iostream>
#include <memory>
#include <utility>

#include <stb_image.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

// ------------------------------------------------------------------------------------------------ mc_atlas

void generate_mc_block_textures_atlas(zip* mc_jar, mdmc::mc_atlas& atlas)
{
	zip_stat_t st{};

	std::vector<std::string> texture_names;

	atlas.m_entry_width = 0;
	atlas.m_entry_height = 0;

	std::string const base_dir = "assets/minecraft/textures/block/";
	for (int i = 0; i < zip_get_num_entries(mc_jar, 0); i++)
	{
		zip_stat_init(&st);
		zip_stat_index(mc_jar, i, 0, &st);

		std::filesystem::path filename(st.name);
		if (
			filename.u8string().rfind(base_dir, 0) == 0 &&
			st.name[strlen(st.name) - 1] != '/' &&
			filename.extension() == ".png")
		{
			std::vector<stbi_uc> buffer(st.size);

			zip_file* f = zip_fopen(mc_jar, st.name, 0);
			zip_fread(f, buffer.data(), st.size);
			zip_fclose(f);

			int w, h, ch;
			stbi_uc* image = stbi_load_from_memory(buffer.data(), (int) buffer.size(), &w, &h, &ch, STBI_default);
			if (atlas.m_entry_width < w) atlas.m_entry_width = w;
			if (atlas.m_entry_height < h) atlas.m_entry_height = h;

			stbi_image_free(image);

			texture_names.emplace_back(filename.u8string());

			mdmc::mc_atlas::texture texture{};
			texture.m_x = 0;
			texture.m_y = 0;
			texture.m_width = w;
			texture.m_height = h;
			atlas.m_textures.emplace("block/" + filename.stem().u8string(), texture);
		}
	}

	atlas.m_data.resize(atlas.size() * STBI_rgb_alpha, 0);

	size_t offset = 0;

	for (std::string const& texture_name : texture_names)
	{
		zip_stat_init(&st);
		zip_stat(mc_jar, texture_name.c_str(), 0, &st);

		std::vector<stbi_uc> buffer(st.size);

		zip_file* f = zip_fopen(mc_jar, st.name, 0);
		zip_fread(f, buffer.data(), st.size);
		zip_fclose(f);

		int w, h, ch;
		stbi_uc* image = stbi_load_from_memory(buffer.data(), (int) buffer.size(), &w, &h, &ch, STBI_rgb_alpha);

		std::memcpy(atlas.m_data.data() + offset, image, w * h * STBI_rgb_alpha);

		stbi_image_free(image);

		offset += atlas.entry_size() * STBI_rgb_alpha;
	}
}

// ------------------------------------------------------------------------------------------------ mc_model

template<glm::length_t n, typename T>
glm::vec<n, T> parse_vec_from_json(rapidjson::Value::Array const& json)
{
	glm::vec<n, T> vec{};
	for (int i = 0; i < n; i++) {
		if constexpr(std::is_same_v<T, int>) {
			vec[i] = json[i].GetInt();
		} else if constexpr(std::is_same_v<T, float>) {
			vec[i] = json[i].GetFloat();
		} else {
			__builtin_unreachable();
		}
	}
	return vec;
}

const std::unordered_map<std::string, mdmc::mc_model_element_face_orientation> g_orientation_by_name{
	{"down", mdmc::mc_model_element_face_orientation::DOWN},
	{"up", mdmc::mc_model_element_face_orientation::UP},
	{"north", mdmc::mc_model_element_face_orientation::NORTH},
	{"south", mdmc::mc_model_element_face_orientation::SOUTH},
	{"west", mdmc::mc_model_element_face_orientation::WEST},
	{"east", mdmc::mc_model_element_face_orientation::EAST}
};

mdmc::mc_atlas::texture const& mdmc::mc_model_element_face::get_texture(
	mdmc::mc_assets const& assets,
	std::unordered_map<std::string, std::string> const& texture_by_variable
) const
{
	std::string texture = m_texture;
	while (texture[0] == '#')
	{
		auto variable = texture.substr(1);
		if (texture_by_variable.find(variable) != texture_by_variable.end()) {
			texture = texture_by_variable.at(variable);
		} else {
			throw std::runtime_error("Unresolved texture name");
		}
	}
	return assets.m_atlas.m_textures.at(texture);
}

void mdmc::mc_model_element_face::from_json(mdmc::mc_model_element& element, rapidjson::Value::Object const& json)
{
	if (json.HasMember("uv")) {
		auto uv = json["uv"].GetArray();

		m_from_uv = glm::vec2(uv[0].GetFloat(), uv[1].GetFloat());
		m_to_uv = glm::vec2(uv[2].GetFloat(), uv[3].GetFloat());
	} else {
		m_from_uv = element.m_from;
		m_to_uv = element.m_to;
	}

	m_texture = json["texture"].GetString();
	m_rotation = json.HasMember("rotation") ? json["rotation"].GetInt() : 0;
	m_tint_index = json.HasMember("tintindex");
}

void mdmc::mc_model_element::from_json(rapidjson::Value::Object const& json)
{
	m_from = parse_vec_from_json<3, float>(json["from"].GetArray());
	m_to = parse_vec_from_json<3, float>(json["to"].GetArray());

	auto old_to = m_to;

	// If a dimension is null adds a very small quantity. This way we always have a OBB.
	if (m_to.x - m_from.x == 0) m_to.x = m_from.x + 0.01f;
	if (m_to.y - m_from.y == 0) m_to.y = m_from.y + 0.01f;
	if (m_to.z - m_from.z == 0) m_to.z = m_from.z + 0.01f;

	if (m_to != old_to) {
		std::cerr << "The block was < 3D. Add a non-null value to make it an OBB." << std::endl;
	}

	if (json.HasMember("rotation"))
	{
		auto rotation = json["rotation"].GetObject();

		m_rotation.m_origin = parse_vec_from_json<3, float>(rotation["origin"].GetArray());
		m_rotation.m_axis = rotation["axis"].GetString()[0];
		m_rotation.m_angle = rotation["angle"].GetFloat();
		m_rotation.m_rescale = rotation.HasMember("rescale") && rotation["rescale"].GetBool();
	}
	else
	{
		m_rotation.m_angle = 0;
	}

	for (auto& member : json["faces"].GetObject())
	{
		mdmc::mc_model_element_face face;
		face.m_orientation = g_orientation_by_name.at(member.name.GetString());
		face.from_json(*this, member.value.GetObject());
		m_face_by_position.emplace(face.m_orientation, face);
	}

	// If the model hasn't got all faces, add the missing ones.
	if (!m_face_by_position.empty() && m_face_by_position.size() < 6)
	{
		for (auto& member : g_orientation_by_name)
		{
			if (m_face_by_position.find(member.second) == m_face_by_position.end())
			{
				mdmc::mc_model_element_face face;
				face.m_orientation = member.second;
				face.m_from_uv = glm::vec3(0);
				face.m_to_uv = glm::vec3(0);
				face.m_texture = "block/torch"; // Some dummy texture invisible at (0, 0).
				face.m_rotation = 0;
				face.m_tint_index = false;
				m_face_by_position.insert(std::make_pair(face.m_orientation, face));
			}
		}
	}
}

void mdmc::mc_model::from_json(rapidjson::Value::Object const& json)
{
	if (json.HasMember("parent")) {
		m_parent_model = json["parent"].GetString();
	}

	if (json.HasMember("textures")) {
		for (auto const& member : json["textures"].GetObject()) {
			m_texture_by_variable.insert(std::make_pair(member.name.GetString(), member.value.GetString()));
		}
	}

	if (json.HasMember("elements")) {
		for (auto& object : json["elements"].GetArray()) {
			mdmc::mc_model_element element{};
			element.from_json(object.GetObject());
			m_elements.push_back(element);
		}
	}
}

void load_models(zip* mc_jar, std::map<std::string, mdmc::mc_model>& models)
{
	zip_stat_t st{};

	std::string const base_dir = "assets/minecraft/models/block/";
	for (int i = 0; i < zip_get_num_entries(mc_jar, 0); i++)
	{
		zip_stat_init(&st);
		zip_stat_index(mc_jar, i, 0, &st);

		std::filesystem::path filename(st.name);
		if (
			filename.u8string().rfind(base_dir, 0) == 0 &&
				st.name[strlen(st.name) - 1] != '/' &&
				filename.extension() == ".json")
		{
			std::vector<char> buffer(st.size);

			zip_file* f = zip_fopen(mc_jar, st.name, 0);
			zip_fread(f, buffer.data(), st.size);
			zip_fclose(f);

			rapidjson::Document document;
			document.Parse(buffer.data(), buffer.size());

			if (document.HasParseError()) {
				rapidjson::ParseErrorCode err = document.GetParseError();
				throw std::runtime_error("Couldn't parse model .json file");
			}

			mdmc::mc_model model{};
			model.from_json(document.GetObject());
			models.emplace("block/" + filename.stem().u8string(), model);
		}
	}
}

// ------------------------------------------------------------------------------------------------ mc_block_state

void mdmc::mc_block_state_variant::from_json(rapidjson::Value::Object const& json)
{
	m_model = json["model"].GetString();
	m_x = json.HasMember("x") ? json["x"].GetInt() : 0;
	m_y = json.HasMember("y") ? json["y"].GetInt() : 0;
	//this->uvlock = json["uvlock"].GetBool();
}

void mdmc::mc_block_state::from_json(rapidjson::Value::Object const& json)
{
	if (json.HasMember("variants"))
	{
		for (auto& member : json["variants"].GetObject())
		{
			mdmc::mc_block_state_variant variant{};
			variant.from_json(member.value.IsArray() ? member.value.GetArray()[0].GetObject() : member.value.GetObject());
			m_variant_by_name.emplace(member.name.GetString(), variant);
		}
	}
	// multiparts
}

void load_block_states(zip* mc_jar, std::function<void(std::string const& name, mdmc::mc_block_state& block_state)> const& on_load)
{
	zip_stat_t st{};

	std::string const base_dir = "assets/minecraft/blockstates/";
	for (int i = 0; i < zip_get_num_entries(mc_jar, 0); i++)
	{
		zip_stat_init(&st);
		zip_stat_index(mc_jar, i, 0, &st);

		std::filesystem::path filename(st.name);
		if (
			filename.u8string().rfind(base_dir, 0) == 0 &&
				st.name[strlen(st.name) - 1] != '/' &&
				filename.extension() == ".json")
		{
			std::vector<char> buffer(st.size);

			zip_file* f = zip_fopen(mc_jar, st.name, 0);
			zip_fread(f, buffer.data(), st.size);
			zip_fclose(f);

			rapidjson::Document document;
			document.Parse(buffer.data(), buffer.size());

			mdmc::mc_block_state block_state{};
			block_state.from_json(document.GetObject());
			on_load(filename.stem().u8string(), block_state);
		}
	}
}

// ------------------------------------------------------------------------------------------------ mc_assets

void mdmc::mc_assets::from_jar(zip* mc_jar, std::string const& mc_version)
{
	m_version = mc_version;

	generate_mc_block_textures_atlas(mc_jar, m_atlas);

	load_models(mc_jar, m_model_by_name);

	load_block_states(mc_jar, [this](std::string const& name, mdmc::mc_block_state& block_state) {
		m_block_state_by_name.emplace(name, block_state);

		for (auto const& [variant_name, _] : block_state.m_variant_by_name) {
			std::string full_variant_name = name + (variant_name.empty() ? "" : ("[" + variant_name + "]"));
			m_block_state_variant_by_name.emplace(full_variant_name, std::ref(m_block_state_by_name[name].m_variant_by_name[variant_name]));
		}
	});

	std::cout << "finished" << std::endl;
}
