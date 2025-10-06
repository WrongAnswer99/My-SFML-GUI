//Author : WrongAnswer99
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <map>
#include <any>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "attr.cpp"
using namespace std;
//if exist then check condition
#define ensure(existCondition,condition) (!(existCondition)||((existCondition)&&(condition)))

static bool isBigEndianCheck() {
	uint16_t num = 0x01;
	return *reinterpret_cast<uint8_t*>(&num) != 0x01;
}
//不使用 constexpr编译期确定 或 std::endian 是因为需要检测运行时的电脑是否为大端,而非编译时的电脑
const bool isBigEndian = isBigEndianCheck();
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
	template<typename T>
	inline T changeEndian(T x) {
		if constexpr (sizeof(x) == 1)
			return x;
		if constexpr (sizeof(x) == 2) {
			char* ptr = reinterpret_cast<char*>(&x);
			std::swap(*ptr, *(ptr + 1));
			return x;
		}
		if constexpr (sizeof(x) == 4) {
			char* ptr = reinterpret_cast<char*>(&x);
			std::swap(*ptr, *(ptr + 3));
			std::swap(*(ptr + 1), *(ptr + 2));
			return x;
		}
		if constexpr (sizeof(x) == 8) {
			char* ptr = reinterpret_cast<char*>(&x);
			std::swap(*ptr, *(ptr + 7));
			std::swap(*(ptr + 1), *(ptr + 6));
			std::swap(*(ptr + 2), *(ptr + 5));
			std::swap(*(ptr + 3), *(ptr + 4));
			return x;
		}
	}
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
	BinaryFStream(std::string filename) :filename(filename) {}
	template<typename T, typename = std::enable_if_t<standardizedSize<std::decay_t<T>>() != 0>>
	inline BinaryFStream& operator>>(T& x) {
		prepareInMode();
		standardizedType<std::decay_t<T>> std_x{};
		fileIn.read(reinterpret_cast<char*>(&std_x), sizeof(std_x));
		x = isBigEndian ?
			changeEndian(static_cast<T>(std_x)) :
			static_cast<T>(std_x);
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
		standardizedType<std::decay_t<T>> std_x = isBigEndian ?
			changeEndian(static_cast<standardizedType<std::decay_t<T>>>(x)) :
			static_cast<standardizedType<std::decay_t<T>>>(x);
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
			return fileIn.good();
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
class Vec2 {
private:
	int x, y;
public:
	template<typename T1, typename T2>
	Vec2(T1 x, T2 y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {}
	template<typename T>
	operator sf::Vector2<T>() {
		return sf::Vector2<T>(static_cast<T>(x), static_cast<T>(y));
	}
};

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
namespace game {
	//statu = {"name1", value1, "name2", value2, ...};
	//statu["name"] = value;
	//T val = statu["name"].cast<T>();
	//statu["name"]["subname"];当"name"键也为Statu类型时可直接使用
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
			template <typename T>
			castable_any(T val) {
				if constexpr (is_same_v<const char*, decay_t<T>>) {
					_data = string(val);
				}
				else if constexpr (is_same_v<const wchar_t*, decay_t<T>>) {
					_data = wstring(val);
				}
				else _data = val;
			}
			//返回:转化为指定类型的值
			template <typename T>
			T& cast() {
				return any_cast<T&>(_data);
			}
			castable_any& operator[](string name) {
				return any_cast<Statu&>(_data)[name];
			}
		};
		unordered_map<string, castable_any>data;
	public:
		Statu() {}
		Statu(initializer_list<castable_any> list) {
			for (auto i = list.begin(); i != list.end(); i++) {
				string name = any_cast<string>(i->_data);
				i++;
				data[name] = *i;
			}
		}
		//可变参数递归调用终止条件:处理无参数或参数满足递归结束的情况
		bool contain() {
			return true;
		}
		//用法:statu.contain("name1", value1, "name2", value2, ...);
		//需要保证参数个数为偶数(因为是键值对),否则抛出runtime_error
		//返回:是否包含键值对
		template<typename T1, typename T2, typename ...Args>
		bool contain(T1 name, T2 val, Args ...args) {
			static_assert(sizeof...(Args) % 2 == 0, "参数个数必须为偶数");
			if (!data.count(static_cast<string>(name)))return false;
			try {
				if constexpr (is_same_v<const char*, decay_t<T2>>) {
					if (any_cast<string>(data[static_cast<string>(name)]._data) != string(val))
						return false;
				}
				else if constexpr (is_same_v<const wchar_t*, decay_t<T2>>) {
					if (any_cast<wstring>(data[static_cast<string>(name)]._data) != wstring(val))
						return false;
				}
				else {
					if (any_cast<T2>(data[static_cast<string>(name)]._data) != val)
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
		castable_any& operator[](string name) {
			return data[name];
		}
		//批量添加键值对
		void operator+=(Statu _statu) {
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
	//Event(eventEnum, {"name1", value1, "name2", value2});
	class Event :public Statu {
	public:
		string eventId;
		Event() {}
		Event(string eventId) :eventId(eventId) {}
		Event(string eventId, initializer_list<castable_any> list) :eventId(eventId) {
			for (auto i = list.begin(); i != list.end(); i++) {
				string name = any_cast<string>(i->_data);
				i++;
				data[name] = *i;
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
	//全局事件
	EventManager globalEvent;

	class FontManager {
	private:
		unordered_map<string, sf::Font>font;
	public:
		bool loadFont(string name, string filename) {
			return font[name].openFromFile(filename);
		}
		sf::Font& operator[](string name) {
			return font[name];
		}
	}fontManager;

	//存储图片Texture
	unordered_map<int, sf::Texture>imageManager;


	//记录当前的frame状态
	struct Frame {
		int frame = 0, tick = 0;
	};


	//存储类型，实体动画+实体标签
	struct AnimationObject {
	private:
		//存储类型，实体动画
		class Animation {
		private:
			vector<int>imageId;

			vector<sf::Vector2f>scale, origin;
			vector<sf::Color>spriteColor;

			//存储了延迟时间，单位为tick
			//显示时间 = 延迟时间 + 1 tick
			vector<int>delay;
		public:
			//将图像数据加到sprite中
			//不会更改位置和方向
			void attachSpriteVal(sf::Sprite& sprite, Frame frame) {
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



	class ObjectDataManager {
		unordered_map<string, AnimationObject>data;
	public:
		AnimationObject& operator[](string name) {
			return data[name];
		}
	};
	//存储了方块信息
	ObjectDataManager blockData;
	//存储了实体信息
	ObjectDataManager entityData;


	//方块类型
	class Block {
	public:
		//方块编号（类型）
		string id = "";
		Frame frame = Frame();
		Block() {}
		//创建方块初始化
		Block(string id) :id(id) {}
		//更新frame指定的原始图像至sprite
		void updateToSprite(sf::Sprite& sprite) const {
			blockData[id].aniMain().attachSpriteVal(sprite, frame);
		}
		void nextTick() {
			blockData[id].aniMain().frameNext(frame);
		}
	};
	//地图类型
	class Map {
	private:
		//存储了block的方块编号
		unordered_map<int64_t, Block>_block;
	public:
		//获取(x,y)位置上的值
		Block& block(int32_t x, int32_t y) {
			return _block[(static_cast<int64_t>(x) << 32) + y];
		}
	};



	//实体类型
	class Entity {
	public:
		//实体编号（类型）
		string id = "";
		string aniId;
		Frame frame = Frame();
		Statu tag;
		Entity() {}
		//创建实体初始化
		Entity(string id, float x, float y, float angle, Statu _tag = Statu()) :id(id), x(x), y(y), angle(angle) {
			tag = entityData[id].tag;
			tag += _tag;
		}
		float x = 0, y = 0;
		float angle = 0;
		//event中存储了未处理事件
		vector<Event> event;

		//更新frame指定的原始图像至sprite
		void updateToSprite(sf::Sprite& sprite) const {
			sprite.setPosition({ x, y });
			sprite.setRotation(sf::degrees(angle));
			entityData[id].ani[aniId].attachSpriteVal(sprite, frame);
		}
		void nextTick() {
			if (!entityData[id].ani[aniId].frameNext(frame)) {
				event.push_back(Event(attr::ani::AnimationEnd));
			}
		}
	};
	class EntityManager {
		//链式前向星
		struct point {
			point() {}
			Entity data;
			int prev = 0, nxt = 0;
		};
		unordered_map<int, point>entity;
		unordered_map<string, int>head;
		unordered_map<string, int>name;
		int maxSize = 0;
	public:
		int spawn(Entity _entity, string _name = "") {
			maxSize++;
			int cur = maxSize;
			if (_name != "")
				name[_name] = cur;
			entity[cur].data = _entity;
			int prev = head[_entity.id];
			if (prev != 0)
				entity[prev].nxt = cur;
			entity[cur].prev = prev;
			entity[cur].nxt = 0;
			head[_entity.id] = cur;
			return cur;
		}
		void erase(int pos) {
			if (!entity.count(pos))return;
			int cur = pos, prev = entity[pos].prev, nxt = entity[pos].nxt;
			string id = entity[pos].data.id;
			if (prev != 0)
				entity[prev].nxt = nxt;
			if (head[id] == cur)
				head[id] = prev;
			else entity[nxt].prev = prev;
			entity.erase(pos);
		}
		void erase(string _name) {
			erase(name[_name]);
		}
		Entity& operator[](int pos) {
			return entity[entity.count(pos) ? pos : 0].data;
		}
		Entity& operator[](string _name) {
			return operator[](name.count(_name) ? (name[_name]) : 0);
		}
		template <typename ...Args>
		vector<int> search(string id = "", Args ...args) {
			vector<int>ret; ret.clear();
			if (id == "") {
				for (int i = 0; i < entity.size(); i++) {
					if (entity[i].data.tag.contain(args...))
						ret.push_back(i);
				}
			}
			else {
				for (int i = head[id]; i != 0; i = entity[i].prev) {
					if (entity[i].data.tag.contain(args...))
						ret.push_back(i);
				}
				reverse(ret.begin(), ret.end());
			}
			return ret;
		}
	}entityManager;


	namespace Data {
		//加密解密文件
		static string encrypt(string filename, uint32_t key) {
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
			static void Rect(sf::RenderTarget& r, sf::Vector2f point1, sf::Vector2f point2, sf::Color fillcolor, sf::Color linecolor = sf::Color::Transparent, float thickness = 0) {
				sf::RectangleShape rect;
				rect.setOrigin((point2 - point1) / 2.0f);
				rect.setPosition((point1 + point2) / 2.f);
				rect.setSize(point2 - point1);
				rect.setFillColor(fillcolor);
				rect.setOutlineColor(linecolor);
				rect.setOutlineThickness(thickness);
				r.draw(rect);
			}
			static void Line(sf::RenderTarget& r, sf::Vector2f point1, sf::Vector2f point2, sf::Color color, float thickness) {
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