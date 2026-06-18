#pragma once
#include <unordered_map>
#include <map>
#include <vector>
#include <filesystem>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "engine/serialization/BinaryFileStream.hpp"

// 安全遍历文件夹中的所有文件，返回相对于 dir 的相对路径
inline std::vector<std::filesystem::path> listFiles(const std::filesystem::path& dir, int depth = 1){
	std::vector<std::filesystem::path> result;
	if (depth <= 0 || !std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
		return result;
	}

	std::error_code ec;
	for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
		if (ec) {
			ec.clear();
			continue;
		}
		if (entry.is_regular_file(ec)) {
			result.push_back(std::filesystem::relative(entry.path(), dir));
		}
		else if (entry.is_directory(ec)) {
			auto subFiles = listFiles(entry.path(), depth - 1);
			for (auto& f : subFiles) {
				result.push_back(entry.path().filename() / f);
			}
		}
	}
	return result;
}

extern sf::Texture nullTexture;

const std::filesystem::path defaultFontDir = "resources/fonts";

class FontManager {
private:
	std::string defaultFontName = "default";
	std::map<std::string, sf::Font>font;
	//noncopyable
	FontManager(const FontManager& other) = delete;
	FontManager& operator=(const FontManager& _f) = delete;
public:
	FontManager() {};
	FontManager(std::string defaultFontName) : defaultFontName(defaultFontName) {};
	std::string getDefaultFontName() {
		return defaultFontName;
	}
	void setDefaultFontName(std::string defaultFontName) {
		this->defaultFontName = defaultFontName;
	}
	void loadAuto(){
		auto files = listFiles(defaultFontDir);
		for (const auto& file : files) {
			loadFont(file);
		}
	}
	bool loadFont(const std::filesystem::path& relativePath) {
		auto fullPath = defaultFontDir / relativePath;
		std::string name = relativePath.filename().stem().string();
		if (font.count(name)) {
			return false;
		}
		return font[name].openFromFile(fullPath.string());
	}
	sf::Font& operator[](const std::string& name) {
		if (font.count(name)) {
			return font[name];
		}
		return font.at(defaultFontName);
	}
};
inline FontManager fontManager("default");

//存储图片Texture
class ImageManager {
private:
	std::map<std::string, sf::Texture>image;
	//noncopyable
	ImageManager(const ImageManager& other) = delete;
	ImageManager& operator=(const ImageManager& _f) = delete;
public:
	ImageManager() {};
	bool loadImage(const std::string& name,const std::filesystem::path& filename) {
		if (image.count(name)){
			throw std::runtime_error("[ImageManager::loadImage] Image already exists");
		}
		return image[name].loadFromFile(filename);
	}
	sf::Texture& operator[](const std::string& name) {
		return image.at(name);
	}
	friend inline void read(BinaryFileStream& bf, ImageManager& x) {
		bf.read(x.image);
	}
	friend inline void write(BinaryFileStream& bf, const ImageManager& x) {
		bf.write(x.image);
	}
};
inline ImageManager globalImageManager;
