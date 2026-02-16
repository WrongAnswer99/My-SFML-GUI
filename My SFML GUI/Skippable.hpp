#pragma once
class Skip_t {};
inline constexpr Skip_t Skip;
//可跳过参数
template<typename T>
class Skippable {
private:
	T val{};
	bool isSkip = true;
public:
	template<typename U>
	Skippable(U val) :val(static_cast<T>(val)), isSkip(false) {}
	Skippable(Skip_t) :isSkip(true) {}
	operator T()const {
		return val;
	}
	bool skipped() {
		return isSkip;
	}
	template<typename U>
	inline void assignTo(U& tar) {
		if (!isSkip) {
			tar = static_cast<U>(val);
		}
	}
};