#pragma once
#include <fstream>
//跨平台二进制文件输入输出
//标准化类型大小
//支持struct二进制序列化
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
	//灵感来自 enable_if_t<>=enable_if<>::type
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
	template<typename T, typename...Args>
	inline void structInSolution(int index, int indexRead, T& x, Args& ...args) {
		if (indexRead == 0)
			(*this) >> indexRead;
		if (indexRead == 0)
			return;
		//indexRead!=0
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
		//indexRead!=0
		if (index == indexRead)
			(*this) >> x;
		(*this) >> indexRead;
		if (indexRead == 0)
			return;
		else {
			fileInGood = false;// 旧程序遇到新字段时标记错误
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
		//0是占位符,表示当前struct结束,用于嵌套struct
	}
public:
	BinaryFStream(const std::string& filename) :filename(filename) {}
	template<typename T, typename = std::enable_if_t<isSafeType<T>>>
	inline BinaryFStream& operator>>(T& x) {
		prepareInMode();
		fileIn.read(reinterpret_cast<char*>(&x), sizeof(x));
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
	template<typename T, typename = std::enable_if_t<isSafeType<T>>>
	inline BinaryFStream& operator<<(const T& x) {
		prepareOutMode();
		fileOut.write(reinterpret_cast<const char*>(&x), sizeof(x));
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
	//关闭文件输入与输出
	inline void close() {
		if (fileIn.is_open())
			fileIn.close();
		if (fileOut.is_open())
			fileOut.close();
	}
	//清空文件内容
	//这个函数不会关闭文件输出,需使用close()关闭
	inline void clear() {
		close();
		fileOut.open(filename, std::ios::binary | std::ios::trunc);
	}
	//文件输入或输出是否良好
	//当未打开文件时返回false
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
