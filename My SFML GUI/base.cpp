//Author : WrongAnswer99
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <any>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "attr.cpp"
using namespace std;
namespace fs = std::filesystem;
//if exist then check condition
#define ensure(existCondition,condition) (!(existCondition)||((existCondition)&&(condition)))

//跨平台二进制文件输入输出
//标准化类型大小
//统一小端存储
//支持struct二进制序列化
class BinaryFStream {
private:
	std::ifstream fileIn;
	std::ofstream fileOut;
	std::string filename;
	bool fileInGood = true;
	template<typename T>
	static constexpr char standardizedSize() {
		if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, char> || std::is_same_v<T, char8_t> || std::is_same_v<T, unsigned char>)
			return 1;
		else if constexpr (std::is_same_v<T, short> || std::is_same_v<T, char16_t> || std::is_same_v<T, unsigned short>)
			return 2;
		else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, char32_t> || std::is_same_v<T, unsigned int>)
			return 4;
		else if constexpr (std::is_same_v<T, long long> || std::is_same_v<T, size_t> || std::is_same_v<T, unsigned long long>)
			return 8;
		else if constexpr (std::is_same_v<T, float>)
			return -4;
		else if constexpr (std::is_same_v<T, double>)
			return -8;
		else return 0;
	}
	template<char size>
	struct standardizedTypeEnum {};
	template<>
	struct standardizedTypeEnum<1> {
		using type = uint8_t;
	};
	template<>
	struct standardizedTypeEnum<2> {
		using type = uint16_t;
	};
	template<>
	struct standardizedTypeEnum<4> {
		using type = uint32_t;
	};
	template<>
	struct standardizedTypeEnum<8> {
		using type = uint64_t;
	};
	template<>
	struct standardizedTypeEnum<-4> {
		using type = float;
	};
	template<>
	struct standardizedTypeEnum<-8> {
		using type = double;
	};
	//灵感来自 enable_if_t<>=enable_if<>::type
	template<typename T>
	using standardizedType = standardizedTypeEnum<standardizedSize<T>()>::type;
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
	BinaryFStream(const string& filename) :filename(filename) {}
	template<typename T, typename = std::enable_if_t<standardizedSize<std::decay_t<T>>() != 0>>
	inline BinaryFStream& operator>>(T& x) {
		prepareInMode();
		standardizedType<std::decay_t<T>> std_x{};
		fileIn.read(reinterpret_cast<char*>(&std_x), sizeof(std_x));
		x = static_cast<T>(std_x);
		return *this;
	}
	inline BinaryFStream& operator>>(std::string& x) {
		size_t size;
		*this >> size;
		x.resize(size);
		fileIn.read(&x[0], size);
		return *this;
	}
	template<typename T, typename = std::enable_if_t<standardizedSize<std::decay_t<T>>() != 0>>
	inline BinaryFStream& operator<<(const T& x) {
		prepareOutMode();
		standardizedType<std::decay_t<T>> std_x = static_cast<standardizedType<std::decay_t<T>>>(x);
		fileOut.write(reinterpret_cast<const char*>(&std_x), sizeof(std_x));
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
			return fileIn.good()&&fileInGood;
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
template<typename T, typename U>
inline static BinaryFStream& operator>>(BinaryFStream& bf, pair<T, U>& x) {
	bf >> x.first >> x.second;
	return bf;
}
template<typename T, typename U>
inline static BinaryFStream& operator<<(BinaryFStream& bf, const pair<T, U>& x) {
	bf << x.first << x.second;
	return bf;
}
template<typename T, typename U>
inline static BinaryFStream& operator>>(BinaryFStream& bf, unordered_map<T, U>& x) {
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
inline static BinaryFStream& operator<<(BinaryFStream& bf, const unordered_map<T, U>& x) {
	bf << x.size();
	for (auto& elem : x) {
		bf << elem.first << elem.second;
	}
	return bf;
}
template<typename T>
inline static BinaryFStream& operator>>(BinaryFStream& bf, vector<T>& x) {
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
inline static BinaryFStream& operator<<(BinaryFStream& bf, const vector<T>& x) {
	bf << x.size();
	for (auto& elem : x) {
		bf << elem;
	}
	return bf;
}
inline static BinaryFStream& operator>>(BinaryFStream& bf, sf::Color& x) {
	bf >> x.r >> x.g >> x.b >> x.a;
	return bf;
}
inline static BinaryFStream& operator<<(BinaryFStream& bf, const sf::Color& x) {
	bf << x.r << x.g << x.b << x.a;
	return bf;
}
template<typename T>
inline static BinaryFStream& operator>>(BinaryFStream& bf, sf::Vector2<T>& x) {
	bf >> x.x >> x.y;
	return bf;
}
template<typename T>
inline static BinaryFStream& operator<<(BinaryFStream& bf, const sf::Vector2<T>& x) {
	bf << x.x << x.y;
	return bf;
}
template<typename T>
inline static BinaryFStream& operator>>(BinaryFStream& bf, sf::Rect<T>& x) {
	bf >> x.position >> x.size;
	return bf;
}
template<typename T>
inline static BinaryFStream& operator<<(BinaryFStream& bf, const sf::Rect<T>& x) {
	bf << x.position << x.size;
	return bf;
}
inline static BinaryFStream& operator>>(BinaryFStream& bf, sf::String& x) {
	string s;
	bf >> s;
	x = sf::String::fromUtf8(s.begin(), s.end());
	return bf;
}
inline static BinaryFStream& operator<<(BinaryFStream& bf, const sf::String& x) {
	bf << x.toUtf8().size();
	for (uint8_t& elem : x.toUtf8())
		bf << elem;
	return bf;
}
inline static BinaryFStream& operator>>(BinaryFStream& bf, sf::Image& x) {
	string s;
	bf >> s;
	if (!x.loadFromMemory(s.data(), s.size()))
		cerr << "[BinaryFStream] Image Read Failed 图片读取失败\n" << "\n";
	return bf;
}
inline static BinaryFStream& operator<<(BinaryFStream& bf, const sf::Image& x) {
	bf << *x.saveToMemory("png");
	return bf;
}
inline static BinaryFStream& operator>>(BinaryFStream& bf, sf::Texture& x) {
	string s;
	bf >> s;
	if (!x.loadFromImage(sf::Image(s.data(), s.size())))
		cerr << "[BinaryFStream] Image Read Failed 图片读取失败\n" << "\n";
	return bf;
}
inline static BinaryFStream& operator<<(BinaryFStream& bf, const sf::Texture& x) {
	bf << *x.copyToImage().saveToMemory("png");
	return bf;
}

class Skip_t {};
#define Skip Skip_t()
//可跳过参数
template<typename T>
class Skipable {
private:
	T val{};
	bool isSkip=true;
public:
	template<typename U>
	Skipable(U val) :val(static_cast<T>(val)), isSkip(false) {}
	Skipable(Skip_t) :isSkip(true) {}
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
//确保T有.id成员且不在外部被修改
template <typename T>
class OrderedHashMap {
private:
	unordered_map<string, typename list<T>::iterator>idMap;
	list<T>data;
public:
	OrderedHashMap() {}
	OrderedHashMap(const OrderedHashMap& other) {
		data = other.data;
		for (auto it = data.begin(); it != data.end(); it++) {
			idMap[it->id] = it;
		}
	}
	OrderedHashMap& operator=(const OrderedHashMap& other) {
		if (this == &other) {
			return *this;
		}
		data = other.data;
		idMap.clear();
		for (auto it = data.begin(); it != data.end(); it++) {
			idMap[it->id] = it;
		}
		return *this;
	}
	T& operator[](const string& key) {
		if (!idMap.count(key)) {
			data.emplace_back();
			data.back().id = key;
			idMap[key] = --data.end();
		}
		return *idMap[key];
	}
	inline list<T>& iterate() {
		return data;
	}
	void erase(const string& key) {
		if (idMap.count(key)) {
			data.erase(idMap[key]);
			idMap.erase(key);
		}
	}
	size_t count(const string& key)const {
		return idMap.count(key);
	}
private:
	void bfin(BinaryFStream& bf) {
		size_t size;
		bf >> size;
		idMap.clear(); data.clear();
		idMap.reserve(size);
		T t{};
		for (size_t i = 0; i < size; i++) {
			bf >> t;
			data.emplace_back(std::move(t));
			idMap.emplace(data.back().id, --data.end());
			t = T{};
		}
	}
	void bfout(BinaryFStream& bf)const {
		bf << data.size();
		for (auto& elem : data) {
			bf << elem;
		}
	}
	friend inline BinaryFStream& operator>>(BinaryFStream& bf, OrderedHashMap<T>& x) {
		x.bfin(bf);
		return bf;
	}
	friend inline BinaryFStream& operator<<(BinaryFStream& bf, const OrderedHashMap<T>& x) {
		x.bfout(bf);
		return bf;
	}
};
//statu = {"key1", value1, "key2", value2, ...};
//statu["key"] = value;
//T val = statu["key"].cast<T>();
//statu["key"]["subkey"];当"key"键也为Statu类型时可直接使用
//自动将const char*转换为string,自动将const wchar_t*转换为wstring
class Statu {
protected:
	//方便转化的any类型
	//自动将const char*转换为string,自动将const wchar_t*转换为wstring
	class castable_any {
		friend class Statu;
		friend class Event;
		// private:
	private:
		any _data;
	public:
		castable_any() {}
		castable_any(const castable_any& other) : _data(other._data) {}
		template <typename T>
		castable_any(T&& val) {
			if constexpr (is_same_v<const char*, decay_t<T>>) {
				_data = string(forward<T>(val));
			}
			else if constexpr (is_same_v<const wchar_t*, decay_t<T>>) {
				_data = wstring(forward<T>(val));
			}
			else _data = forward<T>(val);
		}
		//返回:转化为指定类型的值
		template <typename T>
		T& cast() {
			return any_cast<T&>(_data);
		}
		template <typename T>
		const T& cast() const {
			return any_cast<const T&>(_data);
		}
		castable_any& operator[](const string& key) {
			return any_cast<Statu&>(_data)[key];
		}
		const castable_any& operator[](const string& key) const {
			return any_cast<const Statu&>(_data)[key];
		}
	};
	unordered_map<string, castable_any>data;
public:
	Statu() {}
	Statu(initializer_list<castable_any> list) {
		for (auto i = list.begin(); i != list.end(); i++) {
			string key = any_cast<string>(i->_data);
			i++;
			data[key] = *i;
		}
	}
	//可变参数递归调用终止条件:处理无参数或参数满足递归结束的情况
	bool contain() {
		return true;
	}
	//用法:statu.contain("key1", value1, "key2", value2, ...);
	//需要保证参数个数为偶数(因为是键值对),否则抛出runtime_error
	//返回:是否包含键值对
	template<typename T1, typename T2, typename ...Args>
	bool contain(const T1& key, const T2& val, const Args& ...args) {
		static_assert(sizeof...(Args) % 2 == 0, "参数个数必须为偶数");
		if (!data.count(static_cast<string>(key)))return false;
		try {
			if constexpr (is_same_v<const char*, decay_t<T2>>) {
				if (any_cast<const string&>(data[static_cast<string>(key)]._data) != static_cast<string>(val))
					return false;
			}
			else if constexpr (is_same_v<const wchar_t*, decay_t<T2>>) {
				if (any_cast<const wstring&>(data[static_cast<wstring>(key)]._data) != static_cast<wstring>(val))
					return false;
			}
			else {
				if (any_cast<const T2&>(data[static_cast<string>(key)]._data) != val)
					return false;
			}
		}
		catch (bad_any_cast) {
			return false;
		}
		return contain(args...);
	}
	//可变参数递归调用终止条件:处理无参数或参数满足递归结束的情况
	bool count() {
		return true;
	}
	//用法:statu.count("key1", "key2", ...);
	//返回:是否包含键
	template<typename T, typename ...Args>
	bool count(const T& key, const Args& ...args) {
		if (!data.count(key))
			return false;
		else return count(args...);
	}
	//可变参数递归调用终止条件:处理无参数或参数满足递归结束的情况
	void erase() {
		return;
	}
	//用法:statu.erase("key1", "key2", ...);
	template<typename T, typename ...Args>
	void erase(const T& key, const Args& ...args) {
		if (data.count(key)) {
			data.erase(key);
		}
		erase(args...);
		return;
	}
	//返回:指定键的值
	castable_any& operator[](const string& key) {
		return data[key];
	}
	const castable_any& operator[](const string& key) const {
		return data.at(key);
	}
	//批量添加键值对
	void operator+=(const Statu& _statu) {
		for (auto& elem : _statu.data) {
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
//Event(eventEnum, {"key1", value1, "key2", value2});
class Event :public Statu {
public:
	string eventId;
	Event() {}
	Event(string eventId) :eventId(eventId) {}
	Event(string eventId, initializer_list<castable_any> list) :eventId(eventId) {
		for (auto i = list.begin(); i != list.end(); i++) {
			string key = any_cast<string>(i->_data);
			i++;
			data[key] = *i;
		}
	}
};
class EventManager {
protected:
	queue<Event> eventList;
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
class FontManager {
private:
	unordered_map<string, sf::Font>font;
	//noncopyable
	FontManager(const FontManager& other) = delete;
	FontManager& operator=(const FontManager& _f) = delete;
public:
	FontManager() {};
	bool loadFont(const string& name, const fs::path& filename) {
		return font[name].openFromFile(filename);
	}
	sf::Font& operator[](const string& name) {
		return font[name];
	}
}fontManager;
sf::Texture nullTexture;
//存储图片Texture
class ImageManager {
private:
	unordered_map<string, sf::Texture>image;
	//noncopyable
	ImageManager(const ImageManager& other) = delete;
	ImageManager& operator=(const ImageManager& _f) = delete;
public:
	ImageManager() {};
	bool loadImage(const fs::path& filename) {
		return image[filename.stem().string()].loadFromFile(filename);
	}
	sf::Texture& operator[](const string& name) {
		return image[name];
	}
	friend inline BinaryFStream& operator>>(BinaryFStream& bf, ImageManager& x) {
		size_t size;
		bf >> size;
		string t{};
		sf::Texture u{};
		for (size_t i = 0; i < size; ++i) {
			bf >> t >> u;
			x.image.emplace(std::move(t), std::move(u));
			t = string{};
			u = sf::Texture{};
		}
		return bf;
	}
	friend inline BinaryFStream& operator<<(BinaryFStream& bf, const ImageManager& x) {
		bf << x.image;
		return bf;
	}
}imageManager;
namespace game {
	//全局事件
	EventManager globalEvent;

	//记录当前的frame状态
	struct Frame {
		int frame = 0, tick = 0;
	};


	//存储类型，实体动画+实体标签
	class AnimationData {
	private:
		//存储类型，实体动画
		class Animation {
		private:
			vector<string>imageId;

			vector<sf::Vector2f>scale, origin;
			vector<sf::Color>spriteColor;

			//存储了延迟时间，单位为tick
			//显示时间 = 延迟时间 + 1 tick
			vector<int>delay;
		public:
			//将图像数据加到sprite中
			//不会更改位置和方向
			void attachSprite(sf::Sprite& sprite, Frame frame) {
				sprite.setTexture(imageManager[imageId[frame.frame]], true);
				sprite.setOrigin(origin[frame.frame]);
				sprite.setScale(scale[frame.frame]);
				sprite.setColor(spriteColor[frame.frame]);
				return;
			}
			//返回下一帧，若为sector末尾则返回值为false
			bool frameNext(Frame& frame) {
				frame.tick++;
				if (frame.tick == delay[frame.frame]) {
					frame.tick = 0;
					frame.frame++;
					if (frame.frame == delay.size()) {
						frame.frame = 0;
						return false;
					}
				}
				return true;
			}
		};
	public:
		unordered_map<string, Animation> ani;
		Animation& aniMain() {
			return ani[""];
		}
		Statu tag;
	};


	//存储了方块/实体动画信息
	unordered_map<string, AnimationData>animationData;
	//存储了方块/实体初始化信息
	unordered_map<string, Statu>objInitData;

	//方块类型
	class Block {
	public:
		//方块编号（类型）
		string typeId = "";
		Frame frame = Frame();
		sf::Sprite sprite{ nullTexture };
		Block() {}
		//创建方块初始化
		Block(const string& typeId) :typeId(typeId) {}
		//更新frame指定的原始图像至sprite
		void updateSprite() {
			animationData[typeId].aniMain().attachSprite(sprite, frame);
		}
		void nextTick() {
			animationData[typeId].aniMain().frameNext(frame);
		}
	};
	//地图类型
	class Map {
	private:
		//存储了block的方块编号
		unordered_map<int64_t, Block>blockData;
	public:
		//获取(x,y)位置上的值
		Block& block(int32_t x, int32_t y) {
			return blockData[(static_cast<int64_t>(x) << 32) + y];
		}
	};


	//实体类型
	class Entity {
	public:
		//实体编号（类型）
		string typeId;
		int id;
		string aniId;
		Frame frame = Frame();
		Statu tag;
		sf::Sprite sprite{ nullTexture };
		Entity(string typeId, int id) :typeId(typeId), id(id) {}
		sf::Vector2f pos;
		float angle = 0;
		//event中存储了未处理事件
		vector<Event> event;

		//更新frame指定的原始图像至sprite
		void updateSprite() {
			sprite.setPosition(pos);
			sprite.setRotation(sf::degrees(angle));
			animationData[typeId].ani[aniId].attachSprite(sprite, frame);
		}
		void nextTick() {
			if (!animationData[typeId].ani[aniId].frameNext(frame)) {
				event.push_back(Event(attr::ani::AnimationEnd));
			}
		}
	};
	class EntityManager {
	private:
		unordered_map<string, unordered_map<int, typename list<Entity>::iterator>>idMap;
		unordered_map<string, pair<string, int>>nickIdMap;
		unordered_map<string, int>index;
		list<Entity>data;
		//noncopyable
		EntityManager(const EntityManager& other) = delete;
		EntityManager& operator=(const EntityManager& other) = delete;
		void check(const string& nickId) {
			if (!count(nickId))
				nickIdMap.erase(nickId);
		}
	public:
		EntityManager() {}
		Entity& New(const string& type, const string& nickId="") {
			data.emplace_back(type, index[type]);
			idMap[type][index[type]] = --data.end();
			if (nickId!="")
				nickIdMap[nickId] = make_pair(type, index[type]);
			index[type]++;
			return data.back();
		}
		Entity& entity(const string& nickId) {
			check(nickId);
			if (nickIdMap.count(nickId))
				return *idMap[nickIdMap[nickId].first][nickIdMap[nickId].second];
			else {
				cerr << "[EntityManager::entity] Entity NickID not found 未找到指定实体昵称\n  nickId: " << nickId << "\n";
				throw std::runtime_error("[EntityManager::entity] Entity NickID not found 未找到指定实体昵称\n  nickId: " + nickId + "\n");
			}
		}
		Entity& entity(const string& type,int _index) {
			if (idMap[type].count(_index))
				return *idMap[type][_index];
			else {
				cerr << "[EntityManager::entity] Entity ID not found 未找到指定实体ID\n  type: " << type << "\n  index: " << _index << "\n";
				throw std::runtime_error("[EntityManager::entity] Entity ID not found 未找到指定实体ID\n  type: " + type + "\n  index: " + to_string(_index) + "\n");
			}
		}
		inline const unordered_map<int, typename list<Entity>::iterator>& iterate(const string& type) {
			return idMap[type];
		}
		inline list<Entity>& iterate() {
			return data;
		}
		void erase(const string& nickId) {
			check(nickId);
			if (nickIdMap.count(nickId)) {
				erase(nickIdMap[nickId].first, nickIdMap[nickId].second);
				nickIdMap.erase(nickId);
			}
		}
		void erase(const string& type,int _index) {
			if (idMap[type].count(_index)) {
				data.erase(idMap[type][_index]);
				idMap[type].erase(_index);
			}
		}
		bool count(const string& nickId) {
			return nickIdMap.count(nickId) && idMap[nickIdMap[nickId].first].count(nickIdMap[nickId].second);
		}
		bool count(const string& type,int _index){
			return idMap[type].count(_index);
		}
	};
	EntityManager entityManager;


	namespace Data {
		//加密解密文件
		static string encrypt(const string& filename, uint32_t key) {
			string res = "";
			ifstream fin;
			fin.open(filename, ios::binary);
			uint8_t keypart[4] = {};
			for (int i = 3; i >= 0; i--) {
				keypart[i] = key & 0xFF;
				key >>= 8;
			}
			int part = 0;
			while (fin.good()) {
				char tmp;
				fin.read(&tmp, sizeof(char));
				res += (tmp ^ keypart[part]);
				part++; part %= 4;
			}
			res.pop_back();
			fin.close();
			return res;
		}
		//加载文件
		static void loadFile(string filename) {

		}
	}
	//定义窗口
	sf::RenderWindow window;
	namespace Display {
		const int defaultFPS = 60;
		namespace Pixelate {
			sf::RenderTexture pixelatedScreen;
			static bool init() {
				return pixelatedScreen.resize((window.getSize() + sf::Vector2u(2, 2)) / 3u);
			}
			static void start() {
				pixelatedScreen.clear(sf::Color(0, 0, 0, 0));
			}
			static void draw(sf::Sprite sprite) {
				sprite.setScale(sprite.getScale() / 3.0f);
				sprite.setPosition(sprite.getPosition() / 3.0f);
				pixelatedScreen.draw(sprite);
			}
			static void end() {
				pixelatedScreen.display();
				sf::Sprite pixelatedSprite(pixelatedScreen.getTexture());
				pixelatedSprite.setScale({ 3.0f, 3.0f });
				window.draw(pixelatedSprite);
			}
		}
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
	}
}