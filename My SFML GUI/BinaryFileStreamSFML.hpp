#pragma once
#include "BinaryFileStreamStd.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
inline BinaryFStream& operator>>(BinaryFStream& bf, sf::Color& x) {
	bf >> x.r >> x.g >> x.b >> x.a;
	return bf;
}
inline BinaryFStream& operator<<(BinaryFStream& bf, const sf::Color& x) {
	bf << x.r << x.g << x.b << x.a;
	return bf;
}
template<typename T>
inline BinaryFStream& operator>>(BinaryFStream& bf, sf::Vector2<T>& x) {
	bf >> x.x >> x.y;
	return bf;
}
template<typename T>
inline BinaryFStream& operator<<(BinaryFStream& bf, const sf::Vector2<T>& x) {
	bf << x.x << x.y;
	return bf;
}
template<typename T>
inline BinaryFStream& operator>>(BinaryFStream& bf, sf::Rect<T>& x) {
	bf >> x.position >> x.size;
	return bf;
}
template<typename T>
inline BinaryFStream& operator<<(BinaryFStream& bf, const sf::Rect<T>& x) {
	bf << x.position << x.size;
	return bf;
}
inline BinaryFStream& operator>>(BinaryFStream& bf, sf::String& x) {
	std::string s;
	bf >> s;
	x = sf::String::fromUtf8(s.begin(), s.end());
	return bf;
}
inline BinaryFStream& operator<<(BinaryFStream& bf, const sf::String& x) {
	bf << x.toUtf8().size();
	for (uint8_t& elem : x.toUtf8())
		bf << elem;
	return bf;
}
inline BinaryFStream& operator>>(BinaryFStream& bf, sf::Image& x) {
	std::string s;
	bf >> s;
	if (!x.loadFromMemory(s.data(), s.size())) {
		std::cerr << "[BinaryFStream] Image Read Failed Image¶ÁÈ¡Ê§°Ü" << std::endl;
		throw std::runtime_error("[BinaryFStream] Image Read Failed Image¶ÁÈ¡Ê§°Ü");
	}
	return bf;
}
inline BinaryFStream& operator<<(BinaryFStream& bf, const sf::Image& x) {
	bf << *x.saveToMemory("png");
	return bf;
}
inline BinaryFStream& operator>>(BinaryFStream& bf, sf::Texture& x) {
	std::string s;
	bf >> s;
	if (!x.loadFromImage(sf::Image(s.data(), s.size()))) {
		std::cerr << "[BinaryFStream] Texture Read Failed Texture¶ÁÈ¡Ê§°Ü" << std::endl;
		throw std::runtime_error("[BinaryFStream] Texture Read Failed Texture¶ÁÈ¡Ê§°Ü");
	}
	return bf;
}
inline BinaryFStream& operator<<(BinaryFStream& bf, const sf::Texture& x) {
	bf << *x.copyToImage().saveToMemory("png");
	return bf;
}
