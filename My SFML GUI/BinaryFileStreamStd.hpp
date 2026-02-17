#pragma once
#include "BinaryFileStream.hpp"
#include <map>
#include <vector>
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
inline BinaryFStream& operator>>(BinaryFStream& bf, std::map<T, U>& x) {
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
