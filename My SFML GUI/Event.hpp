#pragma once
#include <any>
#include <string>
#include <unordered_map>
//statu = {"name1", value1, "name2", value2, ...};
//statu["name"] = value;
//T val = statu["name"].cast<T>();
//statu["name"]["subname"];当"name"键也为Statu类型时可直接使用
//自动将const char*转换为string,自动将const wchar_t*转换为wstring
class AnyStatu {
protected:
	//方便转化的any类型
	//自动将const char*转换为string,自动将const wchar_t*转换为wstring
	class castable_any {
		friend class AnyStatu;
		friend class Event;
		// private:
	private:
		std::any data;
	public:
		castable_any() {}
		template <typename T>
		castable_any(T val) {
			if constexpr (std::is_same_v<const char*, std::decay_t<T>>) {
				data = string(val);
			}
			else if constexpr (std::is_same_v<const wchar_t*, std::decay_t<T>>) {
				data = wstring(val);
			}
			else data = val;
		}
		//返回:转化为指定类型的值
		template <typename T>
		T& cast() {
			return any_cast<T&>(data);
		}
		castable_any& operator[](std::string name) {
			return any_cast<AnyStatu&>(data)[name];
		}
	};
	std::unordered_map<std::string, castable_any>data;
public:
	AnyStatu() {}
	AnyStatu(std::initializer_list<castable_any> list) {
		for (auto i = list.begin(); i != list.end(); i++) {
			std::string name = any_cast<std::string>(i->data);
			i++;
			data[name] = *i;
		}
	}
	//可变参数递归调用终止条件:处理无参数或参数满足递归结束的情况
	bool contain() {
		return true;
	}
	//用法:statu.contain("name1", value1, "name2", value2, ...);
	//需要保证参数个数为偶数(因为是键值对)
	//返回:是否包含键值对
	template<typename T1, typename T2, typename ...Args>
	bool contain(T1 name, T2 val, Args ...args) {
		static_assert(sizeof...(Args) % 2 == 0, "参数个数必须为偶数");
		if (!data.count(static_cast<std::string>(name)))return false;
		try {
			if constexpr (std::is_same_v<const char*, std::decay_t<T2>>) {
				if (std::any_cast<std::string>(data[static_cast<std::string>(name)].data) != string(val))
					return false;
			}
			else if constexpr (std::is_same_v<const wchar_t*, std::decay_t<T2>>) {
				if (std::any_cast<std::wstring>(data[static_cast<std::string>(name)].data) != wstring(val))
					return false;
			}
			else {
				if (any_cast<T2>(data[static_cast<std::string>(name)].data) != val)
					return false;
			}
		}
		catch (std::bad_any_cast) {
			return false;
		}
		return contain(args...);
	}
	//可变参数递归调用终止条件:处理无参数或参数满足递归结束的情况
	bool count() {
		return true;
	}
	//用法:statu.count("name1", "name2", ...);
	//返回:是否包含键
	template<typename T, typename ...Args>
	bool count(T name, Args ...args) {
		if (!data.count(name))
			return false;
		else return count(args...);
	}
	//可变参数递归调用终止条件:处理无参数或参数满足递归结束的情况
	void erase() {
		return;
	}
	//用法:statu.erase("name1", "name2", ...);
	template<typename T, typename ...Args>
	void erase(T name, Args ...args) {
		if (data.count(name)) {
			data.erase(name);
		}
		erase(args...);
		return;
	}
	//返回:指定键的值
	castable_any& operator[](std::string name) {
		return data[name];
	}
	//批量添加键值对
	void operator+=(AnyStatu other) {
		for (auto& elem : other.data) {
			data[elem.first] = elem.second;
		}
	}
	//返回:是否为空
	bool empty() const {
		return data.empty();
	}
	//返回:键值对个数
	size_t size() const {
		return data.size();
	}
};
//statu = {"name1", value1, "name2", value2, ...};
//statu["name"] = value;
//T val = statu["name"].cast<T>();
//自动将const char*转换为string,自动将const wchar_t*转换为wstring
class Statu {
	//自动转化的string类型
protected:
	class castable_string {
		friend class Statu;
		friend class Event;
	protected:
		std::string data;
		template <typename T>
		void assign(T val) {
			if constexpr (std::is_same_v<const char*, std::decay_t<T>>)
				data = std::string(val);
			else if constexpr (std::is_same_v<std::string, std::decay_t<T>>)
				data = val;
			else if constexpr (std::is_same_v<const wchar_t*, std::decay_t<T>>)
				data = std::string(reinterpret_cast<const char*>(val), wcslen(val) * sizeof(wchar_t));
			else if constexpr (std::is_same_v<std::wstring, std::decay_t<T>>)
				data = std::string(reinterpret_cast<const char*>(val.data()), val.size() * sizeof(wchar_t));
			else data = std::string(reinterpret_cast<char*>(&val), sizeof(val));
		}
	public:
		castable_string() {}
		template <typename T>
		castable_string(T val) {
			assign(val);
		}
		template<typename T>
		castable_string& operator=(T val) {
			assign(val);
			return *this;
		}
		//返回:转化为指定类型的值
		template <typename T>
		T cast() {
			if constexpr (std::is_same_v<std::string, std::decay_t<T>>)
				return data;
			else if constexpr (std::is_same_v<std::wstring, std::decay_t<T>>)
				return std::wstring(reinterpret_cast<const wchar_t*>(data.data()), data.size() / sizeof(wchar_t));
			else {
				assert(sizeof(T) == data.size());
				T val;
				memcpy(reinterpret_cast<char*>(&val), data.data(), sizeof(T));
				return val;
			}
		}
		std::string& raw() {
			return data;
		}
		bool operator==(const castable_string& other) const {
			return data == other.data;
		}
	};
	std::unordered_map<std::string, castable_string>data;
public:
	Statu() {}
	Statu(std::initializer_list<castable_string> list) {
		for (auto i = list.begin(); i != list.end(); i++) {
			std::string name = i->data;
			i++;
			data[name] = *i;
		}
	}
	//可变参数递归调用终止条件:处理无参数或参数满足递归结束的情况
	bool contain() {
		return true;
	}
	//用法:statu.contain("name1", value1, "name2", value2, ...);
	//需要保证参数个数为偶数(因为是键值对)
	//返回:是否包含键值对
	template<typename T1, typename T2, typename ...Args>
	bool contain(T1 name, T2 val, Args ...args) {
		static_assert(sizeof...(Args) % 2 == 0, "参数个数必须为偶数");
		if (!data.count(static_cast<std::string>(name)))return false;
		if (data[castable_string(name).data] != castable_string(val))return false;
		return contain(args...);
	}
	//可变参数递归调用终止条件:处理无参数或参数满足递归结束的情况
	bool count() {
		return true;
	}
	//用法:statu.count("name1", "name2", ...);
	//返回:是否包含键
	template<typename T, typename ...Args>
	bool count(T name, Args ...args) {
		if (!data.count(name))
			return false;
		else return count(args...);
	}
	//可变参数递归调用终止条件:处理无参数或参数满足递归结束的情况
	void erase() {
		return;
	}
	//用法:statu.erase("name1", "name2", ...);
	template<typename T, typename ...Args>
	void erase(T name, Args ...args) {
		if (data.count(name)) {
			data.erase(name);
		}
		erase(args...);
		return;
	}
	//返回:指定键的值
	castable_string& operator[](std::string name) {
		return data[name];
	}
	//批量添加键值对
	void operator+=(Statu other) {
		for (auto& elem : other.data) {
			data[elem.first] = elem.second;
		}
	}
	bool operator==(const Statu& other) const {
		return data == other.data;
	}
	//返回:是否为空
	bool empty() const {
		return data.empty();
	}
	//返回:键值对个数
	size_t size() const {
		return data.size();
	}
};
//Event(eventEnum, {"key1", value1, "key2", value2});
class Event :public Statu {
public:
	std::string eventId;
	Event() {}
	Event(std::string eventId) :eventId(eventId) {}
	Event(std::string eventId, std::initializer_list<castable_string> list) :eventId(eventId) {
		for (auto i = list.begin(); i != list.end(); i++) {
			std::string key = any_cast<std::string>(i->data);
			i++;
			data[key] = *i;
		}
	}
};
#include <queue>
class EventManager {
protected:
	std::queue<Event> eventList;
public:
	bool pollEvent(Event& event) {
		if (!eventList.empty()) {
			event = eventList.front();
			eventList.pop();
			return true;
		}
		return false;
	}
};
