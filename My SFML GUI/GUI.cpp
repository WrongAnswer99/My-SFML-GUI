//Author : WrongAnswer99
#include "base.cpp"
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
template<typename T,typename U>
inline static BinaryFStream& operator>>(BinaryFStream& bf, pair<T,U>& x) {
	bf >> x.first >> x.second;
	return bf;
}
template<typename T,typename U>
inline static BinaryFStream& operator<<(BinaryFStream& bf, const pair<T,U>& x) {
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
#ifdef LET_ME_SEE_SEE
#define private public
#define protected public
#endif
namespace game {
	namespace gui {
		class WindowManager :public EventManager {
		private:
			Statu focus, overFocus;
			bool mousePressed = false;
			unsigned int cursorBlinkTick = 0;
			//cursorBlinkRate : how many ticks the cursor blinks
			const unsigned int cursorBlinkRate = 30;
			const float scrollResistance = 10.f;
			template<typename T, int size>
			class RollArray {
			private:
				T array[size];
				int realSize = 0;
				int backPosition = 0;
			public:
				inline void emplace_back() {
					if (realSize < size) {
						realSize++;
						backPosition = realSize - 1;
					}
					else {
						backPosition = (backPosition + 1) % size;
					}
				}
				inline T& front() {
					if (realSize < size) {
						return array[0];
					}
					else {
						return array[(backPosition + 1) % size];
					}
				}
				inline int frontPos() {
					if (realSize < size) {
						return 0;
					}
					else {
						return backPosition + 1;
					}
				}
				inline T& back() {
					return array[backPosition];
				}
				inline int& backPos() {
					return backPosition;
				}
				inline T& operator[](int pos) {
					return array[((pos % size) + size) % size];
				}
				inline bool copy_back() {
					emplace_back();
					array[backPosition] = array[(backPosition - 1 + size) % size];
					return true;
				}
				inline size_t Size() {
					return realSize;
				}
			};
			//scroll sensitivity = 6 tick = 0.1 s (60 FPS)
			RollArray<sf::Vector2f, 6>mousePos;
			sf::Vector2f mouseVelocity() {
				return mousePos.back() - mousePos.front();
			}
			//noncopyable
			WindowManager& operator=(const WindowManager& _w)const {}
		public:
			void clear() {
				while (!eventList.empty())
					eventList.pop();
				focus = Statu();
				windowData.clear();
				windowId.clear();
			}
			class ObjBase {
				friend class WindowManager;
			protected:
				string id;
				sf::FloatRect posRect = sf::FloatRect(sf::Vector2f(), sf::Vector2f(1.f, 1.f));
			public:
				class Style {
					friend class WindowManager;
				private:
					sf::Color backgroundColor = sf::Color(0, 0, 0, 0), outlineColor = sf::Color(0, 0, 0, 0);
					float outlineThickness = 1;
					sf::Color textColor = sf::Color(0, 0, 0, 0);
				public:
					Style() {}
					void set(Skipable<sf::Color> _backgroundColor, Skipable<sf::Color>_outlineColor, Skipable<float>_outlineThickness, Skipable<sf::Color>_textColor) {
						_backgroundColor.assignTo(backgroundColor);
						_outlineColor.assignTo(outlineColor);
						_outlineThickness.assignTo(outlineThickness);
						_textColor.assignTo(textColor);
					}
					friend inline BinaryFStream& operator>>(BinaryFStream& bf, Style& x) {
						return bf.structIn(x.backgroundColor, x.outlineColor, x.outlineThickness, x.textColor);
					}
					friend inline BinaryFStream& operator<<(BinaryFStream& bf, const Style& x) {
						return bf.structOut(x.backgroundColor, x.outlineColor, x.outlineThickness, x.textColor);
					}
				};
			protected:
				Style styles[3];
				int currentStatu = attr::gui::Statu::normal;
				void drawBase(sf::RenderTarget& r, sf::FloatRect displayArea) {
					if (posRect.findIntersection(displayArea)) {
						Display::Draw::Rect(
							r,
							posRect.position - displayArea.position,
							posRect.position - displayArea.position + posRect.size,
							styles[currentStatu].backgroundColor,
							styles[currentStatu].outlineColor,
							styles[currentStatu].outlineThickness
						);
					}
				}
			public:
				ObjBase& setPosition(sf::Vector2f _pos) {
					posRect.position = _pos;
					return *this;
				}
				//use this setter
				//after : setPosition() , setSize()
				//* this setter is always the last one
				ObjBase& setCenter() {
					posRect.position -= posRect.size / 2.f;
					return *this;
				}
				ObjBase& setSize(sf::Vector2f _size) {
					posRect.size = _size;
					return *this;
				}
				ObjBase& setStyle(Skipable<Style>_normalStyle, Skipable<Style>_overStyle, Skipable<Style>_focusStyle) {
					_normalStyle.assignTo(styles[attr::gui::Statu::normal]);
					_overStyle.assignTo(styles[attr::gui::Statu::over]);
					_focusStyle.assignTo(styles[attr::gui::Statu::focus]);
					return *this;
				}
				Style& style(int id) {
					return styles[id];
				}
				int getStatu() const {
					return currentStatu;
				}
				friend inline BinaryFStream& operator>>(BinaryFStream& bf, ObjBase& x) {
					return bf.structIn(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
				}
				friend inline BinaryFStream& operator<<(BinaryFStream& bf, const ObjBase& x) {
					return bf.structOut(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
				}
			protected:
				void loseOver() {
					if (currentStatu == attr::gui::over)
						currentStatu = attr::gui::normal;
				}
				void gainOver() {
					if (currentStatu == attr::gui::normal)
						currentStatu = attr::gui::over;
				}
				void loseFocus() {
					currentStatu = attr::gui::normal;
				}
			};
			class TextObj :public ObjBase {
				friend class WindowManager;
			public:
				TextObj() {
					styles[attr::gui::Statu::normal].set(Skip, Skip, Skip, sf::Color::Black);
					styles[attr::gui::Statu::over].set(Skip, Skip, Skip, sf::Color::Black);
					styles[attr::gui::Statu::focus].set(Skip, Skip, Skip, sf::Color::Black);
				}
				//use this setter
				//before : setCenter()
				//after : setStyle() , setGeneralStyle() , setText()
				TextObj& setSizeSyncText() {
					textObj.setFont(game::fontManager[font]);
					textObj.setCharacterSize(characterSize);
					textObj.setLineSpacing(lineSpacing);
					textObj.setLetterSpacing(letterSpacing);
					textObj.setString(text);
					posRect.size.x = 0;
					for (int i = 0; i <= text.getSize(); i++) {
						if (textObj.findCharacterPos(i).x > posRect.size.x)
							posRect.size.x = textObj.findCharacterPos(i).x;
					}
					posRect.size.y = textObj.findCharacterPos(textObj.getString().getSize()).y + characterSize;
					return *this;
				}
			protected:
				string font = "";
				unsigned int characterSize = 30;
				float letterSpacing = 1, lineSpacing = 1;
				sf::String text = "";
				sf::Text textObj{ fontManager[font] };
				sf::FloatRect textRect;
				sf::Vector2i justification = { attr::gui::Mid,attr::gui::Mid };
				void draw(sf::RenderTarget& r, sf::FloatRect displayArea) {
					drawBase(r, displayArea);

					//render text
					textObj.setString("_");
					textObj.setFont(game::fontManager[font]);
					textObj.setCharacterSize(characterSize);
					textObj.setLineSpacing(lineSpacing);
					textObj.setLetterSpacing(letterSpacing);
					textObj.setFillColor(styles[currentStatu].textColor);
					textObj.setPosition({ 0,0 });
					//fix position offset
					sf::Vector2f offsetFix;
					offsetFix.x = textObj.getGlobalBounds().position.x;
					offsetFix.y = textObj.getGlobalBounds().position.y + textObj.getGlobalBounds().size.y - characterSize;
					textObj.setString(text);
					textRect.size.x = 0;
					for (int i = 0; i <= text.getSize(); i++) {
						if (textObj.findCharacterPos(i).x > textRect.size.x)
							textRect.size.x = textObj.findCharacterPos(i).x;
					}
					textRect.size.y = textObj.findCharacterPos(textObj.getString().getSize()).y + characterSize;
					textObj.setPosition(posRect.position - offsetFix + ((posRect.size - textRect.size) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(justification)) - displayArea.position);
					textRect.position = posRect.position + ((posRect.size - textRect.size) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(justification));

					if (textRect.findIntersection(displayArea)) {
						//debug
						/*Display::Draw::Rect(
							r,
							textRect.position - displayArea.position,
							textRect.position - displayArea.position + textRect.size,
							sf::Color::Red
							//styles[currentStatu].backgroundColor
						);*/
						r.draw(textObj);
					}
				}
			public:
				TextObj& setFont(string _font) {
					font = _font;
					return *this;
				}
				TextObj& setCharacterSize(int _characterSize) {
					characterSize = _characterSize;
					return *this;
				}
				TextObj& setSpacing(Skipable<int> _letterSpacing,Skipable<int> _lineSpacing) {
					_letterSpacing.assignTo(letterSpacing);
					_lineSpacing.assignTo(lineSpacing);
					return *this;
				}
				TextObj& setJustification(Skipable<attr::gui::TextJustification> xJus, Skipable<attr::gui::TextJustification> yJus) {
					xJus.assignTo(justification.x);
					yJus.assignTo(justification.y);
					return *this;
				}
				TextObj& setText(sf::String _text) {
					text = _text;
					return *this;
				}
				sf::String getText() {
					return text;
				}
				friend inline BinaryFStream& operator>>(BinaryFStream& bf, TextObj& x) {
					bf.structIn(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structIn(x.font, x.characterSize, x.justification, x.letterSpacing, x.lineSpacing, x.text);
					return bf;
				}
				friend inline BinaryFStream& operator<<(BinaryFStream& bf, const TextObj& x) {
					bf.structOut(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structOut(x.font, x.characterSize, x.justification, x.letterSpacing, x.lineSpacing, x.text);
					return bf;
				}
			};
			class ButtonObj :public TextObj {
				friend class WindowManager;
			public:
				ButtonObj() {
					styles[attr::gui::Statu::normal].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2, sf::Color::Black);
					styles[attr::gui::Statu::over].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2, sf::Color::Black);
					styles[attr::gui::Statu::focus].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2, sf::Color::Black);
				}
				friend inline BinaryFStream& operator>>(BinaryFStream& bf, ButtonObj& x) {
					bf.structIn(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structIn(x.font, x.characterSize, x.justification, x.letterSpacing, x.lineSpacing, x.text);
					return bf;
				}
				friend inline BinaryFStream& operator<<(BinaryFStream& bf, const ButtonObj& x) {
					bf.structOut(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structOut(x.font, x.characterSize, x.justification, x.letterSpacing, x.lineSpacing, x.text);
					return bf;
				}
			};
			class InputObj :public TextObj {
				friend class WindowManager;
			public:
				InputObj() {
					styles[attr::gui::Statu::normal].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2, sf::Color::Black);
					styles[attr::gui::Statu::over].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2, sf::Color::Black);
					styles[attr::gui::Statu::focus].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2, sf::Color::Black);
				}
			protected:
				int typeLimit = attr::gui::InputTypeLimit::String;
				size_t sizeLimit = INT_MAX;
				class InputLimit {
					bool isAllowList = false;
					vector<char32_t>single;
					vector<pair<char32_t, char32_t>>range;
				public:
					void set(bool _isAllowList, initializer_list<char32_t> _single,initializer_list<pair<char32_t, char32_t>> _range) {
						isAllowList = _isAllowList;
						single = _single;
						range = _range;
						for (auto& elem : range)
							if (elem.first > elem.second)
								swap(elem.first, elem.second);
					}
					bool isLegal(char32_t ch) {
						for (auto& elem : single)
							if (ch == elem)return isAllowList;
						for (auto& elem : range)
							if (ch >= elem.first && ch <= elem.second)return isAllowList;
						return !isAllowList;
					}
					size_t legalize(sf::String& st) {
						size_t pos=0;
						for (size_t i = 0; i < st.getSize();i++) {
							if (isLegal(st[i])) {
								if (pos != i)
									st[pos] = st[i];
								pos++;
							}
						}
						return pos;
					}
					friend inline BinaryFStream& operator>>(BinaryFStream& bf, InputLimit& x) {
						bf.structIn(x.isAllowList, x.single, x.range);
						return bf;
					}
					friend inline BinaryFStream& operator<<(BinaryFStream& bf, const InputLimit& x) {
						bf.structOut(x.isAllowList, x.single, x.range);
						return bf;
					}
				}inputLimit;
				size_t cursor = 0;
				sf::Vector2f scroll;
				void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager) {
					if (posRect.findIntersection(displayArea)) {
						drawBase(r, displayArea);
						sf::FloatRect displayAreaCur(-scroll, posRect.size);
						sf::RenderTexture rCur(static_cast<sf::Vector2u>(posRect.size));
						rCur.clear(sf::Color::Transparent);
						//draw text

						//render text
						textObj.setString("_");
						textObj.setFont(game::fontManager[font]);
						textObj.setCharacterSize(characterSize);
						textObj.setLineSpacing(lineSpacing);
						textObj.setLetterSpacing(letterSpacing);
						textObj.setFillColor(styles[currentStatu].textColor);
						textObj.setPosition({ 0,0 });
						//fix position offset
						sf::Vector2f offsetFix;
						offsetFix.x = textObj.getGlobalBounds().position.x;
						offsetFix.y = textObj.getGlobalBounds().position.y + textObj.getGlobalBounds().size.y - characterSize;
						textObj.setString(text);
						textRect.size.x = 0;
						for (int i = 0; i <= text.getSize(); i++) {
							if (textObj.findCharacterPos(i).x > textRect.size.x)
								textRect.size.x = textObj.findCharacterPos(i).x;
						}
						textRect.size.y = textObj.findCharacterPos(textObj.getString().getSize()).y + characterSize;
						textObj.setPosition(-offsetFix + ((posRect.size - textRect.size) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(justification)) - displayAreaCur.position);
						textRect.position = ((posRect.size - textRect.size) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(justification));
						sf::Vector2f cursorPos = textObj.findCharacterPos(cursor);
						cursorPos += offsetFix;
						if (textRect.size.x > posRect.size.x) {
							if (cursorPos.x < 0)
								scroll.x += 0 - cursorPos.x;
							else if (cursorPos.x > posRect.size.x)
								scroll.x += posRect.size.x - cursorPos.x;
							if (textRect.position.x + scroll.x + textRect.size.x < posRect.size.x)
								scroll.x += posRect.size.x - (textRect.position.x + scroll.x + textRect.size.x);
							if (textRect.position.x + scroll.x > 0)
								scroll.x += 0 - (textRect.position.x + scroll.x);
						}
						else scroll.x = 0;
						if (textRect.size.y > posRect.size.y) {
							if (cursorPos.y < 0)
								scroll.y += 0 - cursorPos.y;
							else if (cursorPos.y + characterSize > posRect.size.y)
								scroll.y += posRect.size.y - (cursorPos.y + characterSize);
							if (textRect.position.y + scroll.y + textRect.size.y < posRect.size.y)
								scroll.y += posRect.size.y - (textRect.position.y + scroll.y + textRect.size.y);
							if (textRect.position.y + scroll.y > 0)
								scroll.y += 0 - (textRect.position.y + scroll.y);
						}
						else scroll.y = 0;
						//debug
						/*Display::Draw::Rect(
							rCur,
							textRect.position - displayAreaCur.position,
							textRect.position - displayAreaCur.position + textRect.size,
							sf::Color::Red
							//styles[currentStatu].backgroundColor
						);*/
						rCur.draw(textObj);

						//draw cursor
						if (currentStatu == attr::gui::Statu::focus && (windowManager.cursorBlinkTick >= 0 && windowManager.cursorBlinkTick < windowManager.cursorBlinkRate / 2)) {
							Display::Draw::Line(
								rCur,
								cursorPos,
								cursorPos + sf::Vector2f(0, (float)characterSize),
								styles[currentStatu].textColor,
								2.0f
							);
						}
						rCur.display();
						sf::Sprite s(rCur.getTexture());
						s.setPosition(posRect.position - displayArea.position);
						r.draw(s);
					}
				}
				inline void insert(char32_t ch) {
					if (text.getSize() >= sizeLimit)return;
					if (typeLimit == attr::gui::InputTypeLimit::Int) {
						if ((ch == '-' && cursor == 0 && ensure(text.getSize() > 0, text[0] != '-')) ||
							(ch >= '0' && ch <= '9' && ensure(text.getSize() > 0, ensure(text[0] == '-', cursor != 0)))) {
							text.insert(cursor, ch);
							cursor++;
						}
					}
					else if (typeLimit == attr::gui::InputTypeLimit::Float) {
						if ((ch == '-' && cursor == 0 && ensure(text.getSize() > 0, text[0] != '-')) ||
							(ch >= '0' && ch <= '9' && ensure(text.getSize() > 0, ensure(text[0] == '-', cursor != 0)))||
							(ch =='.'&& ensure(text.getSize() > 0, ensure(text[0] == '-', cursor != 0))&&text.find('.')==sf::String::InvalidPos)) {
							text.insert(cursor, ch);
							cursor++;
						}
					}
					else {
						if (inputLimit.isLegal(ch)) {
							text.insert(cursor, ch);
							cursor++;
						}
					}
				}
				inline void insert(sf::String st) {
					if (typeLimit == attr::gui::InputTypeLimit::Int || typeLimit == attr::gui::InputTypeLimit::Float) {
						for (auto& elem : st) {
							if (text.getSize() >= sizeLimit)return;
							insert(elem);
						}
					}
					else {
						size_t remainSize = sizeLimit - text.getSize();
						st.replace('\r', "");
						size_t realSize = inputLimit.legalize(st);
						if (sizeLimit - text.getSize() > 0) {
							text.insert(cursor, st.substring(0, min(remainSize,realSize)));
							cursor += min(remainSize, realSize);
						}
					}
				}
				inline void erase(bool isForward) {
					if (isForward) {
						if (text.getSize() > 0 && cursor > 0) {
							text.erase(cursor - 1);
							cursor--;
						}
					}
					else {
						if (text.getSize() > 0 && cursor < text.getSize()) {
							text.erase(cursor);
						}
					}
				}
				inline void moveCursor(bool isForward) {
					if (isForward) {
						if (cursor > 0) {
							cursor--;
						}
					}
					else {
						if (cursor < text.getSize()) {
							cursor++;
						}
					}
				}
			public:
				InputObj& setSizeLimit(int _sizeLimit) {
					sizeLimit = _sizeLimit;
					return *this;
				}
				InputObj& setTypeLimit(attr::gui::InputTypeLimit _typeLimit) {
					typeLimit = _typeLimit;
					return *this;
				}
				InputObj& setStringTypeLimit(bool _isAllowList, initializer_list<char32_t> _single, initializer_list<pair<char32_t, char32_t>> _range) {
					typeLimit = attr::gui::InputTypeLimit::String;
					inputLimit.set(_isAllowList, _single, _range);
					return *this;
				}
				InputObj& setText(sf::String _text) {
					text = _text;
					cursor = text.getSize();
					return *this;
				}
				friend inline BinaryFStream& operator>>(BinaryFStream& bf, InputObj& x) {
					bf.structIn(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structIn(x.font, x.characterSize, x.justification, x.letterSpacing, x.lineSpacing, x.text);
					bf.structIn(x.sizeLimit,x.typeLimit, x.inputLimit);
					x.cursor = x.text.getSize();
					return bf;
				}
				friend inline BinaryFStream& operator<<(BinaryFStream& bf, const InputObj& x) {
					bf.structOut(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structOut(x.font, x.characterSize, x.justification, x.letterSpacing, x.lineSpacing, x.text);
					bf.structOut(x.sizeLimit, x.typeLimit, x.inputLimit);
					return bf;
				}
			};
			class AreaObj :public ObjBase {
				friend class WindowManager;
			protected:
				sf::Vector2f scroll;
				sf::Vector2f scrollable;
				sf::Vector2f scrollVelocity;
				sf::FloatRect scrollLimit;
				unordered_map<string, int>areaId, textId, buttonId, inputId;
				vector<AreaObj>areaData;
				vector<TextObj>textData;
				vector<ButtonObj>buttonData;
				vector<InputObj>inputData;
			public:
				AreaObj() {
					styles[attr::gui::Statu::normal].set(sf::Color::White, sf::Color(200, 200, 200), 2, Skip);
					styles[attr::gui::Statu::over].set(sf::Color::White, sf::Color(200, 200, 200), 2, Skip);
					styles[attr::gui::Statu::focus].set(sf::Color::White, sf::Color(200, 200, 200), 2, Skip);
				}
				AreaObj& area(string id) {
					if (!areaId.count(id)) {
						areaData.emplace_back();
						areaData[areaData.size() - 1].id = id;
						areaId[id] = (int)areaData.size() - 1;
					}
					return areaData[areaId[id]];
				}
				TextObj& text(string id) {
					if (!textId.count(id)) {
						textData.emplace_back();
						textData[textData.size() - 1].id = id;
						textId[id] = (int)textData.size() - 1;
					}
					return textData[textId[id]];
				}
				ButtonObj& button(string id) {
					if (!buttonId.count(id)) {
						buttonData.emplace_back();
						buttonData[buttonData.size() - 1].id = id;
						buttonId[id] = (int)buttonData.size() - 1;
					}
					return buttonData[buttonId[id]];
				}
				InputObj& input(string id) {
					if (!inputId.count(id)) {
						inputData.emplace_back();
						inputData[inputData.size() - 1].id = id;
						inputId[id] = (int)inputData.size() - 1;
					}
					return inputData[inputId[id]];
				}
				AreaObj& setScrollable(bool scrollableX, bool scrollableY) {
					scrollable = sf::Vector2f(scrollableX, scrollableY);
					return *this;
				}
			private:
				inline void addPoint(sf::FloatRect& rect, sf::Vector2f point) {
					if (point.x < rect.position.x) {
						rect.size.x += rect.position.x - point.x;
						rect.position.x = point.x;
					}
					else if (point.x > rect.position.x + rect.size.x) {
						rect.size.x = point.x - rect.position.x;
					}
					if (point.y < rect.position.y) {
						rect.size.y += rect.position.y - point.y;
						rect.position.y = point.y;
					}
					else if (point.y > rect.position.y + rect.size.y) {
						rect.size.y = point.y - rect.position.y;
					}
				}
			public:
				AreaObj& setScrollLimit(sf::FloatRect rect) {
					scrollLimit = sf::FloatRect(sf::Vector2f(), posRect.size);
					addPoint(scrollLimit, rect.position);
					addPoint(scrollLimit, rect.position + rect.size);
					scrollLimit.size -= posRect.size;
					return *this;
				}
				AreaObj& setScrollLimitAuto() {
					scrollLimit = sf::FloatRect(sf::Vector2f(), posRect.size);
					for (auto& elem : areaData) {
						addPoint(scrollLimit, elem.posRect.position);
						addPoint(scrollLimit, elem.posRect.position + elem.posRect.size);
					}
					for (auto& elem : textData) {
						addPoint(scrollLimit, elem.posRect.position);
						addPoint(scrollLimit, elem.posRect.position + elem.posRect.size);
					}
					for (auto& elem : buttonData) {
						addPoint(scrollLimit, elem.posRect.position);
						addPoint(scrollLimit, elem.posRect.position + elem.posRect.size);
					}
					for (auto& elem : inputData) {
						addPoint(scrollLimit, elem.posRect.position);
						addPoint(scrollLimit, elem.posRect.position + elem.posRect.size);
					}
					scrollLimit.size -= posRect.size;
					return *this;
				}
				friend inline BinaryFStream& operator>>(BinaryFStream& bf, AreaObj& x) {
					bf.structIn(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structIn(x.areaId, x.areaData, x.textId, x.textData, x.buttonId, x.buttonData, x.inputId, x.inputData);
					bf.structIn(x.scrollable, x.scrollLimit);
					return bf;
				}
				friend inline BinaryFStream& operator<<(BinaryFStream& bf, const AreaObj& x) {
					bf.structOut(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structOut(x.areaId, x.areaData, x.textId, x.textData, x.buttonId, x.buttonData, x.inputId, x.inputData);
					bf.structOut(x.scrollable, x.scrollLimit);
					return bf;
				}
			private:
				void ensureScrollLimit() {
					if (-scroll.x < scrollLimit.position.x) {
						scroll.x = -(scrollLimit.position.x);
					}
					if (-scroll.x > scrollLimit.position.x + scrollLimit.size.x) {
						scroll.x = -(scrollLimit.position.x + scrollLimit.size.x);
					}
					if (-scroll.y < scrollLimit.position.y) {
						scroll.y = -(scrollLimit.position.y);
					}
					if (-scroll.y > scrollLimit.position.y + scrollLimit.size.y) {
						scroll.y = -(scrollLimit.position.y + scrollLimit.size.y);
					}
				}
				void updateScroll(WindowManager& windowManager) {
					if (scrollable != sf::Vector2f()) {
						if (scrollVelocity.lengthSquared() < windowManager.scrollResistance * windowManager.scrollResistance) {
							scrollVelocity = sf::Vector2f();
						}
						else {
							scrollVelocity -= scrollVelocity
								.componentWiseDiv(sf::Vector2f(scrollVelocity.length(), scrollVelocity.length()))
								.componentWiseMul(sf::Vector2f(windowManager.scrollResistance, windowManager.scrollResistance));
						}
						scroll += scrollVelocity;
						ensureScrollLimit();
					}
					for (auto& elem : areaData) {
						elem.updateScroll(windowManager);
					}
				}
				void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager) {
					if (!posRect.findIntersection(displayArea))
						return;
					drawBase(r, displayArea);
					sf::FloatRect displayAreaCur(-scroll, posRect.size);
					sf::RenderTexture rCur(static_cast<sf::Vector2u>(posRect.size));
					rCur.clear(sf::Color::Transparent);
					//clear
					for (auto& elem : areaData) {
						elem.draw(rCur, displayAreaCur, windowManager);
					}
					for (auto& elem : buttonData) {
						elem.draw(rCur, displayAreaCur);
					}
					for (auto& elem : textData) {
						elem.draw(rCur, displayAreaCur);
					}
					for (auto& elem : inputData) {
						elem.draw(rCur, displayAreaCur, windowManager);
					}
					rCur.display();
					sf::Sprite s(rCur.getTexture());
					s.setPosition(posRect.position - displayArea.position);
					r.draw(s);

				}
			};
		private:
			unordered_map<string, int>windowId;
			vector <AreaObj>windowData;
		public:
			bool newWindow(string id) {
				if (windowId.count(id)) {
					//窗口名称重复
					return false;
				}
				windowData.emplace_back();
				windowData[windowData.size() - 1].id = id;
				windowId[id] = (int)windowData.size() - 1;
				return true;
			}
			bool newWindow(string id, AreaObj& w) {
				if (windowId.count(id)) {
					//窗口名称重复
					return false;
				}
				windowData.push_back(w);
				windowData[windowData.size() - 1].id = id;
				windowId[id] = (int)windowData.size() - 1;
				return true;
			}
			bool topWindowIs(string id) {
				return windowData[windowData.size() - 1].id == id;
			}
			bool closeTopWindow() {
				overFocus = {}; focus = {};
				if (windowData.size() > 0) {
					windowId.erase(windowData[windowData.size() - 1].id);
					windowData.pop_back();
					return true;
				}
				return false;
			}
			AreaObj& window(string id) {
				if (windowId.count(id))
					return windowData[windowId[id]];
				throw std::out_of_range("WindowManager::window: 未找到指定窗口ID: " + id);
			}
			template<typename F>
			auto& visit(string path, F func) {
				AreaObj* areaPtr = nullptr;
				string temp = "";
				for (int i = 0; i < path.size(); i++) {
					if (path[i] == '_') {
						if (areaPtr == nullptr)
							areaPtr = &window(temp);
						else areaPtr = &(areaPtr->area(temp));
						temp = "";
					}
					else temp.push_back(path[i]);
				}
				return func(areaPtr, temp);
			}
			AreaObj& area(string path) {
				return visit(path, [this](AreaObj* areaPtr, const string& name)->AreaObj& {
					if (areaPtr == nullptr)
						areaPtr = &window(name);
					else areaPtr = &(areaPtr->area(name));
					return *areaPtr;
					});
			}
			TextObj& text(string path) {
				return visit(path, [this](AreaObj* areaPtr, const string& name)->TextObj& {
					return areaPtr->text(name);
					});
			}
			ButtonObj& button(string path) {
				return visit(path, [this](AreaObj* areaPtr, const string& name)->ButtonObj& {
					return areaPtr->button(name);
					});
			}
			InputObj& input(string path) {
				return visit(path, [this](AreaObj* areaPtr, const string& name)->InputObj& {
					return areaPtr->input(name);
					});
			}
			AreaObj* updateOver(AreaObj* areaPtr,bool stopScroll=false) {
				if (overFocus.count(attr::gui::ButtonId)) {
					area(overFocus[attr::gui::AreaPath].cast<string>())
						.button(overFocus[attr::gui::ButtonId].cast<string>()).loseOver();
				}
				else if (overFocus.count(attr::gui::InputId)) {
					area(overFocus[attr::gui::AreaPath].cast<string>())
						.input(overFocus[attr::gui::InputId].cast<string>()).loseOver();
				}
				else if (overFocus.count(attr::gui::AreaPath)) {
					area(overFocus[attr::gui::AreaPath].cast<string>()).loseOver();
				}
				sf::Vector2f origin = areaPtr->posRect.position;
				overFocus = {};
				string path = "";
				while (true) {
					if (stopScroll)
						areaPtr->scrollVelocity = sf::Vector2f();
					if (path == "")
						path = areaPtr->id;
					else path = path + '_' + areaPtr->id;
					for (auto& elem : areaPtr->areaData) {
						if (elem.posRect.contains(mousePos.back() - areaPtr->scroll - origin)) {
							origin += elem.posRect.position + areaPtr->scroll;
							areaPtr = &elem;
							goto L_continue;
						}
					}
					break;
					L_continue:;
				}
				//debug;
				overFocus[attr::gui::AreaPath] = path;
				for (auto& elem : areaPtr->buttonData) {
					if (elem.posRect.contains(mousePos.back() - areaPtr->scroll - origin)) {
						overFocus[attr::gui::ButtonId] = elem.id;
						goto L_return;
					}
				}
				for (auto& elem : areaPtr->inputData) {
					if (elem.posRect.contains(mousePos.back() - areaPtr->scroll - origin)) {
						overFocus[attr::gui::InputId] = elem.id;
						goto L_return;
					}
				}
				L_return:;
				return areaPtr;
			}
			bool update(const optional<sf::Event>& sfEvent) {
				if (windowData.size() == 0)
					return false;
				//update mouse press statu
				if (sfEvent->is<sf::Event::MouseButtonPressed>())
					mousePressed = true;
				if (sfEvent->is<sf::Event::MouseButtonReleased>())
					mousePressed = false;
				//sf::Event::MouseButtonPressed
				//update focus
				//after updating focus ,varible 'focus' & 'overFocus' must have a value
				string path = "";
				AreaObj* topWindow = &windowData[windowData.size() - 1];
				AreaObj* areaOverPtr;
				if (sfEvent->is<sf::Event::MouseButtonPressed>()) {
					mousePos.back() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseButtonPressed>()->position);//update mousePos
					areaOverPtr = updateOver(topWindow,true);
					if (focus.count(attr::gui::InputId)&&!overFocus.contain(attr::gui::AreaPath,focus[attr::gui::AreaPath].cast<string>(), attr::gui::InputId, focus[attr::gui::InputId].cast<string>()))
						eventList.push(game::Event(attr::gui::InputLoseFocus, { attr::gui::InputPath,focus[attr::gui::AreaPath].cast<string>() + '_' + focus[attr::gui::InputId].cast<string>() }));
					if (overFocus.count(attr::gui::InputId) && !focus.contain(attr::gui::AreaPath, overFocus[attr::gui::AreaPath].cast<string>(), attr::gui::InputId, overFocus[attr::gui::InputId].cast<string>()))
						eventList.push(game::Event(attr::gui::InputGainFocus, { attr::gui::InputPath,overFocus[attr::gui::AreaPath].cast<string>() + '_' + overFocus[attr::gui::InputId].cast<string>() }));

					if (focus.count(attr::gui::ButtonId))
						area(focus[attr::gui::AreaPath].cast<string>())
							.button(focus[attr::gui::ButtonId].cast<string>()).loseFocus();
					else if (focus.count(attr::gui::InputId))
						area(focus[attr::gui::AreaPath].cast<string>())
							.input(focus[attr::gui::InputId].cast<string>()).loseFocus();
					else if (focus.count(attr::gui::AreaPath))
						area(focus[attr::gui::AreaPath].cast<string>()).loseFocus();

					if (overFocus.count(attr::gui::ButtonId))
						areaOverPtr->button(overFocus[attr::gui::ButtonId].cast<string>()).currentStatu = attr::gui::Statu::focus;
					else if (overFocus.count(attr::gui::InputId))
						areaOverPtr->input(overFocus[attr::gui::InputId].cast<string>()).currentStatu = attr::gui::Statu::focus;
					//update inertial scroll stop
					else if (overFocus.count(attr::gui::AreaPath))
						areaOverPtr->currentStatu = attr::gui::Statu::focus;

					focus = overFocus;
					return true;
				}
				//sf::Event::MouseMoved
				if (sfEvent->is<sf::Event::MouseMoved>()) {
					mousePos.back() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseMoved>()->position);//update mousePos
					areaOverPtr=updateOver(topWindow);
					//update scroll
					if (mousePressed) {
						AreaObj& areaPtr = area(focus[attr::gui::AreaPath].cast<string>());
						if (areaPtr.scrollable != sf::Vector2f()) {
							//It seems that do not cancel the focus when scrolling makes the GUI easier to use
							/*
							if (focus.count(attr::gui::ButtonId)) {
								area(focus[attr::gui::AreaPath].cast<string>())
									.button(focus[attr::gui::ButtonId].cast<string>()).currentStatu = attr::gui::Statu::over;
								focus.erase(attr::gui::ButtonId);
							}
							if (focus.count(attr::gui::InputId)) {
								area(focus[attr::gui::AreaPath].cast<string>())
									.input(focus[attr::gui::InputId].cast<string>()).currentStatu = attr::gui::Statu::over;
								focus.erase(attr::gui::InputId);
							}
							*/
							//tar.scroll += mouseVelocity();//wrong
							//consider the case of two continuous sf::Event::MouseMoved event
							if (mousePos.Size()>1)
							areaPtr.scroll += (mousePos.back()-mousePos[mousePos.backPos()-1]).componentWiseMul(areaPtr.scrollable);
						}
						if (focus.count(attr::gui::ButtonId)) {
							if (overFocus.contain(attr::gui::AreaPath, focus[attr::gui::AreaPath].cast<string>(), attr::gui::ButtonId, focus[attr::gui::ButtonId].cast<string>()))
								area(focus[attr::gui::AreaPath].cast<string>()).button(focus[attr::gui::ButtonId].cast<string>()).currentStatu = attr::gui::Statu::focus;
							else area(focus[attr::gui::AreaPath].cast<string>()).button(focus[attr::gui::ButtonId].cast<string>()).currentStatu = attr::gui::Statu::over;
						}
					}
					//update over
					//after updating over ,varible 'focus' will not be changed
					else {
						if (overFocus.count(attr::gui::ButtonId))
							areaOverPtr->button(overFocus[attr::gui::ButtonId].cast<string>()).gainOver();
						else if (overFocus.count(attr::gui::InputId))
							areaOverPtr->input(overFocus[attr::gui::InputId].cast<string>()).gainOver();
						else if (overFocus.count(attr::gui::AreaPath))
							areaOverPtr->gainOver();
					}
					return true;
				}
				//sf::Event::MouseButtonReleased
				//update release
				//after updating release ,varible 'focus' will not be changed
				if (sfEvent->is<sf::Event::MouseButtonReleased>()) {
					mousePos.back() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseButtonReleased>()->position);//update mousePos
					areaOverPtr = updateOver(topWindow);

					if (focus.count(attr::gui::ButtonId)) {
						if (overFocus.contain(attr::gui::AreaPath, focus[attr::gui::AreaPath].cast<string>(), attr::gui::ButtonId, focus[attr::gui::ButtonId].cast<string>())) {
							area(focus[attr::gui::AreaPath].cast<string>()).button(focus[attr::gui::ButtonId].cast<string>()).currentStatu = attr::gui::Statu::over;
							eventList.push(game::Event(attr::gui::ButtonPressed, { attr::gui::ButtonPath,focus[attr::gui::AreaPath].cast<string>() + '_' + focus[attr::gui::ButtonId].cast<string>() }));
						}
						else area(focus[attr::gui::AreaPath].cast<string>()).button(focus[attr::gui::ButtonId].cast<string>()).currentStatu = attr::gui::Statu::normal;
					}

					//update inertial scroll start
					if (focus.count(attr::gui::AreaPath)) {
						if (area(focus[attr::gui::AreaPath].cast<string>()).scrollable != sf::Vector2f())
							area(focus[attr::gui::AreaPath].cast<string>()).scrollVelocity = mouseVelocity().componentWiseMul(area(focus[attr::gui::AreaPath].cast<string>()).scrollable);
					}
					return true;
				}
				if (sfEvent->is<sf::Event::TextEntered>()) {
					if (focus.count(attr::gui::InputId)) {
						char32_t ch = sfEvent->getIf<sf::Event::TextEntered>()->unicode;
						InputObj& InputTar = area(focus[attr::gui::AreaPath].cast<string>()).input(focus[attr::gui::InputId].cast<string>());
						if (ch == 8)//backspace key
							InputTar.erase(true);
						else if (ch == 13)//enter key
							InputTar.insert('\n');
						else if (ch == 22)//ctrl+v
							InputTar.insert(sf::Clipboard::getString());
						else InputTar.insert(ch);//other keys
						cursorBlinkTick = 0;
					}
					return true;
				}
				if (sfEvent->is<sf::Event::KeyPressed>()) {
					if (focus.count(attr::gui::InputId)) {
						InputObj& InputTar = area(focus[attr::gui::AreaPath].cast<string>()).input(focus[attr::gui::InputId].cast<string>());
						if (sfEvent->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Left)//left arrow
							InputTar.moveCursor(true);
						if (sfEvent->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Right)//right arrow
							InputTar.moveCursor(false);
						if (sfEvent->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Delete)//delete key
							InputTar.erase(false);
						cursorBlinkTick = 0;
					}
					return true;
				}
				return false;
			}
			//draw all objects to drawTarget
			//update cursor tick
			//update inertial scroll
			void draw(sf::RenderTarget& drawTarget) {
				if (focus.count(attr::gui::InputId)) {
					cursorBlinkTick++;
					cursorBlinkTick %= cursorBlinkRate;
				}
				else cursorBlinkTick = 0;
				mousePos.copy_back();//update mousePos
				for (auto& elem : windowData) {
					elem.updateScroll(*this);
					elem.draw(drawTarget, sf::FloatRect(sf::Vector2f(), static_cast<sf::Vector2f>(drawTarget.getSize())), *this);
				}
			}
		};
	}
}
#ifdef LET_ME_SEE_SEE
#undef private
#undef protected
#endif