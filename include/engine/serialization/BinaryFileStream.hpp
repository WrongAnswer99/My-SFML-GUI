#pragma once
#include <fstream>
#include <type_traits>
#include <cstdint>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <utility>
#include <iostream>

//跨平台二进制文件输入输出
//标准化类型大小
//支持struct二进制序列化
//支持标准容器(sf::Vector2, sf::Rect, sf::String, sf::Color, std::vector, std::map, std::unordered_map, std::pair)
class BinaryFStream {
private:
	std::ifstream fileIn;
	std::ofstream fileOut;
	std::string filename;
	bool fileInGood = true;

	template<typename T>
	static constexpr bool isSafeType =
		std::is_same_v<T, bool> ||
		std::is_same_v<T, char> || std::is_same_v<T, unsigned char> ||
		std::is_same_v<T, short> || std::is_same_v<T, unsigned short> ||
		std::is_same_v<T, int> || std::is_same_v<T, unsigned int> ||
		std::is_same_v<T, long long> || std::is_same_v<T, unsigned long long> ||
		std::is_same_v<T, float> || std::is_same_v<T, double> ||
		std::is_same_v<T, char8_t> || std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t> ||
		std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t> ||
		std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>;

	inline void prepareInMode() {
		if (fileOut.is_open()) {
			fileOut.close();
		}
		if (!fileIn.is_open()) {
			fileIn.open(filename, std::ios::binary);
			fileInGood = true;
		}
	}

	inline void prepareOutMode() {
		if (fileIn.is_open()) {
			fileIn.close();
			fileInGood = true;
		}
		if (!fileOut.is_open()) {
			fileOut.open(filename, std::ios::binary | std::ios::app);
		}
	}

	template<typename T, std::enable_if_t<isSafeType<T>, int> = 0>
	inline void readSafe(T& x) {
		fileIn.read(reinterpret_cast<char*>(&x), sizeof(x));
	}

	template<typename T, std::enable_if_t<isSafeType<T>, int> = 0>
	inline void writeSafe(const T& x) {
		fileOut.write(reinterpret_cast<const char*>(&x), sizeof(x));
	}

	template<typename T, typename...Args>
	inline void structInSolution(int index, int indexRead, T& x, Args& ...args) {
		if (indexRead == 0)
			(*this) >> indexRead;
		if (indexRead == 0)
			return;
		if (index == indexRead) {
			(*this) >> x;
			structInSolution(index + 1, 0, (args)...);
		}
		else {
			structInSolution(index + 1, indexRead, (args)...);
		}
	}

	template<typename T>
	inline void structInSolution(int index, int indexRead, T& x) {
		if (indexRead == 0)
			(*this) >> indexRead;
		if (indexRead == 0)
			return;
		if (index == indexRead)
			(*this) >> x;
		(*this) >> indexRead;
		if (indexRead == 0)
			return;
		else {
			fileInGood = false;
		}
	}

	template<typename T, typename...Args>
	inline void structOutSolution(int index, const T& x, Args&& ...args) {
		(*this) << index << x;
		structOutSolution(index + 1, std::forward<Args>(args)...);
	}

	template<typename T>
	inline void structOutSolution(int index, const T& x) {
		(*this) << index << x << 0;
	}

public:
	BinaryFStream(const std::string& filename) :filename(filename) {}

	template<typename T, std::enable_if_t<isSafeType<T>, int> = 0>
	inline BinaryFStream& operator>>(T& x) {
		prepareInMode();
		readSafe(x);
		return *this;
	}

	inline BinaryFStream& operator>>(size_t& x) {
		prepareInMode();
		unsigned long long ullx;
		fileIn.read(reinterpret_cast<char*>(&ullx), sizeof(ullx));
		x = static_cast<size_t>(ullx);
		return *this;
	}

	inline BinaryFStream& operator>>(std::string& x) {
		size_t size;
		*this >> size;
		x.resize(size);
		fileIn.read(&x[0], size);
		return *this;
	}

	template<typename T, std::enable_if_t<isSafeType<T>, int> = 0>
	inline BinaryFStream& operator<<(const T& x) {
		prepareOutMode();
		writeSafe(x);
		return *this;
	}

	inline BinaryFStream& operator<<(const size_t& x) {
		prepareOutMode();
		const unsigned long long ullx = static_cast<const unsigned long long>(x);
		fileOut.write(reinterpret_cast<const char*>(&ullx), sizeof(ullx));
		return *this;
	}

	inline BinaryFStream& operator<<(const std::string& x) {
		*this << x.size();
		fileOut.write(x.data(), x.size());
		return *this;
	}

	inline void close() {
		if (fileIn.is_open())
			fileIn.close();
		if (fileOut.is_open())
			fileOut.close();
	}

	inline void clear() {
		close();
		fileOut.open(filename, std::ios::binary | std::ios::trunc);
	}

	inline bool good() {
		if (fileIn.is_open())
			return fileIn.good() && fileInGood;
		if (fileOut.is_open())
			return fileOut.good();
		return false;
	}

	template <typename ...Args>
	inline BinaryFStream& structIn(Args&...args) {
		structInSolution(1, 0, (args)...);
		return *this;
	}

	template <typename ...Args>
	inline BinaryFStream& structOut(const Args&...args) {
		structOutSolution(1, (args)...);
		return *this;
	}
};

// Standard container serialization
template<typename T, typename U>
inline BinaryFStream& operator>>(BinaryFStream& bf, std::pair<T, U>& x) {
	bf >> x.first >> x.second;
	return bf;
}
template<typename T, typename U>
inline BinaryFStream& operator<<(BinaryFStream& bf, const std::pair<T, U>& x) {
	bf << x.first << x.second;
	return bf;
}
template<typename T, typename U>
inline BinaryFStream& operator>>(BinaryFStream& bf, std::unordered_map<T, U>& x) {
	size_t size;
	bf >> size;
	x.clear();
	x.reserve(size);
	T t{};
	U u{};
	for (size_t i = 0; i < size; ++i) {
		bf >> t >> u;
		x.emplace(std::move(t), std::move(u));
		t = T{};
		u = U{};
	}
	return bf;
}
template<typename T, typename U>
inline BinaryFStream& operator<<(BinaryFStream& bf, const std::unordered_map<T, U>& x) {
	bf << x.size();
	for (auto& elem : x) {
		bf << elem.first << elem.second;
	}
	return bf;
}
template<typename T, typename U>
inline BinaryFStream& operator>>(BinaryFStream& bf, std::map<T, U>& x) {
	size_t size;
	bf >> size;
	x.clear();
	T t{};
	U u{};
	for (size_t i = 0; i < size; ++i) {
		bf >> t >> u;
		x.emplace(std::move(t), std::move(u));
		t = T{};
		u = U{};
	}
	return bf;
}
template<typename T, typename U>
inline BinaryFStream& operator<<(BinaryFStream& bf, const std::map<T, U>& x) {
	bf << x.size();
	for (auto& elem : x) {
		bf << elem.first << elem.second;
	}
	return bf;
}
template<typename T>
inline BinaryFStream& operator>>(BinaryFStream& bf, std::vector<T>& x) {
	size_t size;
	bf >> size;
	T t{};
	x.clear();
	x.reserve(size);
	for (size_t i = 0; i < size; ++i) {
		bf >> t;
		x.emplace_back(std::move(t));
		t = T{};
	}
	return bf;
}
template<typename T>
inline BinaryFStream& operator<<(BinaryFStream& bf, const std::vector<T>& x) {
	bf << x.size();
	for (auto& elem : x) {
		bf << elem;
	}
	return bf;
}

// SFML type serialization
#include <SFML/Graphics.hpp>

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
		std::cerr << "[BinaryFStream] Image Read Failed" << std::endl;
		throw std::runtime_error("[BinaryFStream] Image Read Failed");
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
		std::cerr << "[BinaryFStream] Texture Read Failed" << std::endl;
		throw std::runtime_error("[BinaryFStream] Texture Read Failed");
	}
	return bf;
}
inline BinaryFStream& operator<<(BinaryFStream& bf, const sf::Texture& x) {
	bf << *x.copyToImage().saveToMemory("png");
	return bf;
}
