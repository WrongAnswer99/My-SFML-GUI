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
//if exist then check condition
#define ensure(existCondition,condition) (!(existCondition)||((existCondition)&&(condition)))

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
//ȷ��T��.id��Ա�Ҳ����ⲿ���޸�
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
class FontManager {
private:
	unordered_map<string, sf::Font>font;
	//noncopyable
	FontManager(const FontManager& other) = delete;
	FontManager& operator=(const FontManager& _f) = delete;
public:
	FontManager() {};
	bool loadFont(string name, string filename) {
		return font[name].openFromFile(filename);
	}
	sf::Font& operator[](string name) {
		return font[name];
	}
}fontManager;
typedef unordered_map<int, sf::Texture> ImageManager;
sf::Texture nullTexture;
namespace game {
	//ȫ���¼�
	EventManager globalEvent;

	//�洢ͼƬTexture
	ImageManager imageManager;

	//��¼��ǰ��frame״̬
	struct Frame {
		int frame = 0, tick = 0;
	};


	//�洢���ͣ�ʵ�嶯��+ʵ���ǩ
	class AnimationData {
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
			void attachSprite(sf::Sprite& sprite, Frame frame) {
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


	//�洢�˷���/ʵ�嶯����Ϣ
	unordered_map<string, AnimationData>animationData;
	//�洢�˷���/ʵ���ʼ����Ϣ
	unordered_map<string, Statu>objInitData;

	//��������
	class Block {
	public:
		//�����ţ����ͣ�
		string typeId = "";
		Frame frame = Frame();
		sf::Sprite sprite{ nullTexture };
		Block() {}
		//���������ʼ��
		Block(string typeId) :typeId(typeId) {}
		//����frameָ����ԭʼͼ����sprite
		void updateSprite() {
			animationData[typeId].aniMain().attachSprite(sprite, frame);
		}
		void nextTick() {
			animationData[typeId].aniMain().frameNext(frame);
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
		string typeId;
		int id;
		string aniId;
		Frame frame = Frame();
		Statu tag;
		sf::Sprite sprite{ nullTexture };
		Entity(string typeId, int id) :typeId(typeId), id(id) {}
		sf::Vector2f pos;
		float angle = 0;
		//event�д洢��δ�����¼�
		vector<Event> event;

		//����frameָ����ԭʼͼ����sprite
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
		Entity& New(const string& type,string nickId="") {
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
				cerr << "[EntityManager::entity] Entity NickID not found δ�ҵ�ָ��ʵ���ǳ�\n  nickId: " << nickId << "\n";
				throw std::runtime_error("[EntityManager::entity] Entity NickID not found δ�ҵ�ָ��ʵ���ǳ�\n  nickId: " + nickId + "\n");
			}
		}
		Entity& entity(const string& type,int _index) {
			if (idMap[type].count(_index))
				return *idMap[type][_index];
			else {
				cerr << "[EntityManager::entity] Entity ID not found δ�ҵ�ָ��ʵ��ID\n  type: " << type << "\n  index: " << _index << "\n";
				throw std::runtime_error("[EntityManager::entity] Entity ID not found δ�ҵ�ָ��ʵ��ID\n  type: " + type + "\n  index: " + to_string(_index) + "\n");
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