#pragma once
#include <nlohmann/json.hpp>
#include "SFML/Graphics.hpp"
#include "engine/data/VarianTmap.hpp"

// SFML type JSON serialization using adl_serializer specialization
// This avoids injecting into the sf namespace
namespace nlohmann {
	template<>
	struct adl_serializer<sf::Color> {
		static void to_json(json& j, const sf::Color& x) {
			j = json{ {"r", x.r}, {"g", x.g}, {"b", x.b}, {"a", x.a} };
		}
		static void from_json(const json& j, sf::Color& x) {
			j.at("r").get_to(x.r);
			j.at("g").get_to(x.g);
			j.at("b").get_to(x.b);
			j.at("a").get_to(x.a);
		}
	};

	template<typename T>
	struct adl_serializer<sf::Vector2<T>> {
		static void to_json(json& j, const sf::Vector2<T>& x) {
			j = json{ {"x", x.x}, {"y", x.y} };
		}
		static void from_json(const json& j, sf::Vector2<T>& x) {
			j.at("x").get_to(x.x);
			j.at("y").get_to(x.y);
		}
	};

	template<typename T>
	struct adl_serializer<sf::Rect<T>> {
		static void to_json(json& j, const sf::Rect<T>& x) {
			j = json{ {"position", x.position}, {"size", x.size} };
		}
		static void from_json(const json& j, sf::Rect<T>& x) {
			j.at("position").get_to(x.position);
			j.at("size").get_to(x.size);
		}
	};

	template<>
	struct adl_serializer<sf::String> {
		static void to_json(json& j, const sf::String& x) {
			auto utf8 = x.toUtf8();
			j = std::string(utf8.begin(), utf8.end());
		}
		static void from_json(const json& j, sf::String& x) {
			std::string s = j.get<std::string>();
			x = sf::String::fromUtf8(s.begin(), s.end());
		}
	};

	template<>
	struct adl_serializer<sf::Image> {
		static void to_json(json& j, const sf::Image& x) {
			auto memory = x.saveToMemory("png");
			if (!memory) {
				throw std::runtime_error("[JsonExtensions] Image saveToMemory failed");
			}
			j = json::binary(*memory);
		}
		static void from_json(const json& j, sf::Image& x) {
			auto data = j.get_binary();
			if (!x.loadFromMemory(data.data(), data.size())) {
				throw std::runtime_error("[JsonExtensions] Image from_json failed");
			}
		}
	};

	template<>
	struct adl_serializer<sf::Texture> {
		static void to_json(json& j, const sf::Texture& x) {
			auto memory = x.copyToImage().saveToMemory("png");
			if (!memory) {
				throw std::runtime_error("[JsonExtensions] Texture saveToMemory failed");
			}
			j = json::binary(*memory);
		}
		static void from_json(const json& j, sf::Texture& x) {
			auto data = j.get_binary();
			sf::Image img;
			if (!img.loadFromMemory(data.data(), data.size())) {
				throw std::runtime_error("[JsonExtensions] Texture from_json failed (image load)");
			}
			if (!x.loadFromImage(img)) {
				throw std::runtime_error("[JsonExtensions] Texture from_json failed (texture load)");
			}
		}
	};
}

// VarianTmap JSON serialization
// 参照 BinaryFileStream 的 VarianTmapSerializerWrapper read/write 逻辑
// 使用 TypeOrderIndex（类型序号）而非 type_index，保证跨平台一致性
template<typename Base, typename ...T>
class VarianTmapJsonSerializerWrapper {
	VarianTmap<Base>* data;
public:
	VarianTmapJsonSerializerWrapper(VarianTmap<Base>& data) : data(&data) {}
	VarianTmapJsonSerializerWrapper(const VarianTmap<Base>& data) : data(const_cast<VarianTmap<Base>*>(&data)) {}

	friend void to_json(nlohmann::json& j, const VarianTmapJsonSerializerWrapper<Base, T...>& x) {
		if constexpr (sizeof...(T) == 0) { j = nlohmann::json::array(); return; }
		size_t TypeOrderIndex = 0;
		std::unordered_map<std::type_index, size_t> TypeOrderIndexMap;
		((TypeOrderIndexMap.emplace(std::type_index(typeid(T)), TypeOrderIndex++)), ...);
		std::unordered_map<std::type_index, std::function<void(nlohmann::json&, Base*)>> writeHelper;
		((writeHelper[std::type_index(typeid(T))] = [&](nlohmann::json& j, Base* BasePointer) {
			j.push_back(nlohmann::json{
				{"type_index", TypeOrderIndexMap[std::type_index(typeid(T))]},
				{"!type_name", typeid(T).name()},
				{"key", x.data->find_key(BasePointer)},
				{"value", *static_cast<T*>(BasePointer)}
			});
		}), ...);
		j = nlohmann::json::array();
		for (auto& elem : x.data->order()) {
			if (writeHelper.find(x.data->find_type_index(elem)) != writeHelper.end())
				writeHelper[x.data->find_type_index(elem)](j, elem);
		}
	}

	friend void from_json(const nlohmann::json& j, VarianTmapJsonSerializerWrapper<Base, T...> x) {
		if constexpr (sizeof...(T) == 0) return;
		x.data->clear();
		size_t TypeOrderIndex = 0;
		std::unordered_map<size_t, std::type_index> TypeOrderIndexMap;
		(TypeOrderIndexMap.emplace(TypeOrderIndex++, std::type_index(typeid(T))), ...);
		std::unordered_map<std::type_index, std::function<void(const nlohmann::json&)>> readHelper;
		((readHelper[std::type_index(typeid(T))] = [&](const nlohmann::json& elem) {
			std::string key;
			elem.at("key").get_to(key);
			T* pointer = x.data->template push_back_named<T>(key, T{});
			elem.at("value").get_to(*pointer);
		}), ...);
		for (const auto& elem : j) {
			size_t TypeIndex;
			elem.at("!type_index").get_to(TypeIndex);
			readHelper[TypeOrderIndexMap.at(TypeIndex)](elem);
		}
	}
};
