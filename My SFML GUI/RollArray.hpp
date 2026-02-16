#pragma once
template<typename T, int capacity>
class RollArray {
private:
	T array[capacity];
	size_t realSize = 0;
	size_t backPosition = 0;
public:
	inline void emplace_back() {
		if (realSize < capacity) {
			realSize++;
			backPosition = realSize - 1;
		}
		else {
			backPosition = (backPosition + 1) % capacity;
		}
	}
	inline T& front() {
		if (realSize < capacity)
			return array[0];
		else return array[(backPosition + 1) % capacity];
	}
	inline T& back() {
		return array[backPosition];
	}
	inline T& operator[](size_t pos) {
		return array[(((realSize < capacity ? 0 : (backPosition + 1) % capacity) + pos) % capacity + capacity) % capacity];
	}
	inline bool copy_back() {
		emplace_back();
		array[backPosition] = array[(backPosition - 1 + capacity) % capacity];
		return true;
	}
	inline size_t size() {
		return realSize;
	}
};