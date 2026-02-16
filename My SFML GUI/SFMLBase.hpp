#pragma once
#include <unordered_map>
#include "SFML/Graphics.hpp"
#include "BinaryFileStreamSFML.hpp"

extern sf::Texture nullTexture;

template <typename T>
inline sf::Rect<T> operator+(sf::Rect<T> rect, sf::Vector2<T> vec) {
	rect.position += vec;
	return rect;
}
template <typename T>
inline sf::Rect<T> operator-(sf::Rect<T> rect, sf::Vector2<T> vec) {
	rect.position -= vec;
	return rect;
}


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
extern FontManager fontManager;

//存储图片Texture
class ImageManager {
private:
	std::map<std::string, sf::Texture>image;
	//noncopyable
	ImageManager(const ImageManager& other) = delete;
	ImageManager& operator=(const ImageManager& _f) = delete;
public:
	ImageManager() {};
	bool loadImage(const std::filesystem::path& filename) {
		return image[filename.stem().string()].loadFromFile(filename);
	}
	sf::Texture& operator[](const std::string& name) {
		return image[name];
	}
	friend inline BinaryFStream& operator>>(BinaryFStream& bf, ImageManager& x) {
		size_t size;
		bf >> size;
		std::string t{};
		sf::Texture u{};
		for (size_t i = 0; i < size; ++i) {
			bf >> t >> u;
			x.image.emplace(std::move(t), std::move(u));
			t = std::string{};
			u = sf::Texture{};
		}
		return bf;
	}
	friend inline BinaryFStream& operator<<(BinaryFStream& bf, const ImageManager& x) {
		bf << x.image;
		return bf;
	}
};
extern ImageManager imageManager;

class Pixelated {
	sf::RenderTexture pixelated;
	unsigned int pixelSize;
public:
	Pixelated(sf::Vector2u size, unsigned int pixelSize) :pixelSize(pixelSize) {
		if (!pixelated.resize((size + sf::Vector2u(pixelSize-1, pixelSize - 1)).componentWiseDiv(sf::Vector2u(pixelSize,pixelSize)))) {
			std::cerr << "[Pixelated::Pixelated] Failed to create sf::RenderTexture."<<std::endl;
			throw std::runtime_error("[Pixelated::Pixelated] Failed to create sf::RenderTexture.\n");
		}
		pixelated.clear(sf::Color(0, 0, 0, 0));
	}
	void reset() {
		pixelated.clear(sf::Color(0, 0, 0, 0));
	}
	void draw(sf::Sprite sprite) {
		sprite.setScale(sprite.getScale().componentWiseDiv(sf::Vector2f(static_cast<float>(pixelSize), static_cast<float>(pixelSize))));
		sprite.setPosition(sprite.getPosition().componentWiseDiv(sf::Vector2f(static_cast<float>(pixelSize), static_cast<float>(pixelSize))));
		pixelated.draw(sprite);
	}
	void display(sf::RenderTarget& target) {
		pixelated.display();
		sf::Sprite pixelatedSprite(pixelated.getTexture());
		pixelatedSprite.setScale(sf::Vector2f(static_cast<float>(pixelSize), static_cast<float>(pixelSize)));
		target.draw(pixelatedSprite);
	}
};

//快速绘制简单图形
namespace Draw {
	static void Rect(sf::RenderTarget& r, const sf::Vector2f& point1, const sf::Vector2f& point2, sf::Color fillcolor, sf::Color linecolor = sf::Color::Transparent, float thickness = 0) {
		sf::RectangleShape rect;
		rect.setOrigin((point2 - point1) / 2.0f);
		rect.setPosition((point1 + point2) / 2.f);
		rect.setSize(point2 - point1);
		rect.setFillColor(fillcolor);
		rect.setOutlineColor(linecolor);
		rect.setOutlineThickness(thickness);
		r.draw(rect);
	}
	static void Line(sf::RenderTarget& r, const sf::Vector2f& point1, const sf::Vector2f& point2, sf::Color color, float thickness) {
		sf::RectangleShape rect;
		sf::CircleShape circle;
		float len = (point2 - point1).length();
		rect.setPosition((point1 + point2) / 2.f);
		rect.setOrigin({ len / 2.0f, thickness / 2.0f });
		rect.setSize({ len, thickness });
		rect.setFillColor(color);
		rect.setRotation((point2 - point1).angle());
		r.draw(rect);
		circle.setOrigin({ thickness / 2.0f, thickness / 2.0f });
		circle.setRadius(thickness / 2.0f);
		circle.setFillColor(color);
		circle.setPosition(point1);
		r.draw(circle);
		circle.setPosition(point2);
		r.draw(circle);
	}
}
