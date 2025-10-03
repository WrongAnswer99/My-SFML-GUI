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
static bool isBigEndianCheck() {
	uint16_t num = 0x01;
	return *reinterpret_cast<uint8_t*>(&num) != 0x01;
}
//��ʹ�� constexpr������ȷ�� �� std::endian ����Ϊ��Ҫ�������ʱ�ĵ����Ƿ�Ϊ���,���Ǳ���ʱ�ĵ���
const bool isBigEndian = isBigEndianCheck();
//��ƽ̨�������ļ��������
//��׼�����ʹ�С
//ͳһС�˴洢
//֧��struct���������л�
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
	//������� enable_if_t<>=enable_if<>::type
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
			fileInGood = false;// �ɳ����������ֶ�ʱ��Ǵ���
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
		//0��ռλ��,��ʾ��ǰstruct����,����Ƕ��struct
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
	//�ر��ļ����������
	inline void close() {
		if (fileIn.is_open())
			fileIn.close();
		if (fileOut.is_open())
			fileOut.close();
	}
	//����ļ�����
	//�����������ر��ļ����,��ʹ��close()�ر�
	inline void clear() {
		close();
		fileOut.open(filename, std::ios::binary | std::ios::trunc);
	}
	//�ļ����������Ƿ�����
	//��δ���ļ�ʱ����false
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
//����������
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
	//statu["name"]["subname"];��"name"��ҲΪStatu����ʱ��ֱ��ʹ��
	//�Զ���const char*ת��Ϊstring,�Զ���const wchar_t*ת��Ϊwstring
	class Statu {
	protected:
		//����ת����any����
		//�Զ���const char*ת��Ϊstring,�Զ���const wchar_t*ת��Ϊwstring
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
			//����:ת��Ϊָ�����͵�ֵ
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
		//�ɱ�����ݹ������ֹ����:�����޲������������ݹ���������
		bool contain() {
			return true;
		}
		//�÷�:statu.contain("name1", value1, "name2", value2, ...);
		//��Ҫ��֤��������Ϊż��(��Ϊ�Ǽ�ֵ��),�����׳�runtime_error
		//����:�Ƿ������ֵ��
		template<typename T1, typename T2, typename ...Args>
		bool contain(T1 name, T2 val, Args ...args) {
			static_assert(sizeof...(Args) % 2 == 0, "������������Ϊż��");
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
		//�ɱ�����ݹ������ֹ����:�����޲������������ݹ���������
		bool count() {
			return true;
		}
		//�÷�:statu.count("name1", "name2", ...);
		//����:�Ƿ������
		template<typename T, typename ...Args>
		bool count(T name, Args ...args) {
			if (!data.count(name))
				return false;
			else return count(args...);
		}
		//�ɱ�����ݹ������ֹ����:�����޲������������ݹ���������
		void erase() {
			return;
		}
		//�÷�:statu.erase("name1", "name2", ...);
		template<typename T, typename ...Args>
		void erase(T name, Args ...args) {
			if (data.count(name)) {
				data.erase(name);
			}
			erase(args...);
			return;
		}
		//����:ָ������ֵ
		castable_any& operator[](string name) {
			return data[name];
		}
		//������Ӽ�ֵ��
		void operator+=(Statu _statu) {
			for (auto& elem : _statu.data) {
				data[elem.first] = elem.second;
			}
		}
		//����:�Ƿ�Ϊ��
		bool empty() const {
			return data.empty();
		}
		//����:��ֵ�Ը���
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
	//ȫ���¼�
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

	//�洢ͼƬTexture
	unordered_map<int, sf::Texture>imageManager;


	//��¼��ǰ��frame״̬
	struct Frame {
		int frame = 0, tick = 0;
	};


	//�洢���ͣ�ʵ�嶯��+ʵ���ǩ
	struct AnimationObject {
	private:
		//�洢���ͣ�ʵ�嶯��
		class Animation {
		private:
			vector<int>imageId;

			vector<sf::Vector2f>scale, origin;
			vector<sf::Color>spriteColor;

			//�洢���ӳ�ʱ�䣬��λΪtick
			//��ʾʱ�� = �ӳ�ʱ�� + 1 tick
			vector<int>delay;
		public:
			//��ͼ�����ݼӵ�sprite��
			//�������λ�úͷ���
			void attachSpriteVal(sf::Sprite& sprite, Frame frame) {
				sprite.setTexture(imageManager[imageId[frame.frame]], true);
				sprite.setOrigin(origin[frame.frame]);
				sprite.setScale(scale[frame.frame]);
				sprite.setColor(spriteColor[frame.frame]);
				return;
			}
			//������һ֡����Ϊsectorĩβ�򷵻�ֵΪfalse
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
	//�洢�˷�����Ϣ
	ObjectDataManager blockData;
	//�洢��ʵ����Ϣ
	ObjectDataManager entityData;


	//��������
	class Block {
	public:
		//�����ţ����ͣ�
		string id = "";
		Frame frame = Frame();
		Block() {}
		//���������ʼ��
		Block(string id) :id(id) {}
		//����frameָ����ԭʼͼ����sprite
		void updateToSprite(sf::Sprite& sprite) const {
			blockData[id].aniMain().attachSpriteVal(sprite, frame);
		}
		void nextTick() {
			blockData[id].aniMain().frameNext(frame);
		}
	};
	//��ͼ����
	class Map {
	private:
		//�洢��block�ķ�����
		unordered_map<int64_t, Block>_block;
	public:
		//��ȡ(x,y)λ���ϵ�ֵ
		Block& block(int32_t x, int32_t y) {
			return _block[(static_cast<int64_t>(x) << 32) + y];
		}
	};



	//ʵ������
	class Entity {
	public:
		//ʵ���ţ����ͣ�
		string id = "";
		string aniId;
		Frame frame = Frame();
		Statu tag;
		Entity() {}
		//����ʵ���ʼ��
		Entity(string id, float x, float y, float angle, Statu _tag = Statu()) :id(id), x(x), y(y), angle(angle) {
			tag = entityData[id].tag;
			tag += _tag;
		}
		float x = 0, y = 0;
		float angle = 0;
		//event�д洢��δ�����¼�
		vector<Event> event;

		//����frameָ����ԭʼͼ����sprite
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
		//��ʽǰ����
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
		//���ܽ����ļ�
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
		//�����ļ�
		static void loadFile(string filename) {

		}
	}
	//���崰��
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
		//���ٻ��Ƽ�ͼ��
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