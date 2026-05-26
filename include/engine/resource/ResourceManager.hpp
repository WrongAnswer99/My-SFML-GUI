#pragma once
#include <unordered_map>
#include <map>
#include <filesystem>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "engine/serialization/BinaryFileStream.hpp"

extern sf::Texture nullTexture;

class FontManager {
private:
	std::map<std::string, sf::Font>font;
	//noncopyable
	FontManager(const FontManager& other) = delete;
	FontManager& operator=(const FontManager& _f) = delete;
public:
	FontManager() {};
	bool loadFont(const std::string& name, const std::filesystem::path& filename) {
		return font[name].openFromFile(filename);
	}
	sf::Font& operator[](const std::string& name) {
		return font[name];
	}
};
inline FontManager fontManager;

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
