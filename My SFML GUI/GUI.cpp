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
inline static BinaryFStream& operator>>(BinaryFStream& bf, sf::Image& x) {
	string s;
	bf >> s;
	if (!x.loadFromMemory(s.data(), s.size()))
		cerr << "[BinaryFStream] Image Read Failed 图片读取失败\n" << "\n";
	return bf;
}
inline static BinaryFStream& operator<<(BinaryFStream& bf, const sf::Image& x) {
	bf<<*x.saveToMemory("png");
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
#ifdef DEBUG
#define private public
#define protected public
#endif
namespace game {
	namespace gui {
		class WindowManager :public EventManager {
		private:
			Statu focus, overFocus;
			bool isDragScrolling = false;
			bool mousePressed = false;
			unsigned int cursorBlinkTick = 0;
			//cursorBlinkRate : how many ticks the cursor blinks
			const unsigned int cursorBlinkRate = 30;
			const float scrollResistance = 3.f;
			//scroll sensitivity = 6 tick = 0.1 s (60 FPS)
			static constexpr int scrollSensitivity = 6;
			const float mouseWheelScrollRate = 50.f;
			const float scrollThreshold = 20;
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
			RollArray<sf::Vector2f, scrollSensitivity>mousePos;
			sf::Vector2f mouseLastPressPos;
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
			class Style {
				friend class WindowManager;
			private:
				sf::Color backgroundColor = sf::Color(0, 0, 0, 0), outlineColor = sf::Color(0, 0, 0, 0);
				float outlineThickness = 1;
			public:
				Style() {}
				void set(Skipable<sf::Color> _backgroundColor, Skipable<sf::Color>_outlineColor, Skipable<float>_outlineThickness) {
					_backgroundColor.assignTo(backgroundColor);
					_outlineColor.assignTo(outlineColor);
					_outlineThickness.assignTo(outlineThickness);
				}
				friend inline BinaryFStream& operator>>(BinaryFStream& bf, Style& x) {
					return bf.structIn(x.backgroundColor, x.outlineColor, x.outlineThickness);
				}
				friend inline BinaryFStream& operator<<(BinaryFStream& bf, const Style& x) {
					return bf.structOut(x.backgroundColor, x.outlineColor, x.outlineThickness);
				}
			};
			class ObjBase {
				friend class WindowManager;
				template <typename T>
				friend class OrderedHashMap;
			protected:
				string id;
				sf::FloatRect posRect = sf::FloatRect(sf::Vector2f(), sf::Vector2f(1.f, 1.f));
				Style styles[3];
				int currentStatu = attr::gui::Statu::normal;
				void draw(sf::RenderTarget& r, sf::FloatRect displayArea) {
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
				void setStatu(int statu,bool force) {
					if (force || statu > currentStatu) {
						currentStatu = statu;
					}
				}
				ObjBase& loseOver() {
					if (currentStatu == attr::gui::over)
						currentStatu = attr::gui::normal;
					return *this;
				}
				ObjBase& gainOver() {
					if (currentStatu == attr::gui::normal)
						currentStatu = attr::gui::over;
					return *this;
				}
				ObjBase& loseFocus() {
					currentStatu = attr::gui::normal;
					return *this;
				}
				ObjBase& gainFocus() {
					currentStatu = attr::gui::focus;
					return *this;
				}
			};
			class ImageObj :public ObjBase {
				friend class WindowManager;
			public:
				ImageObj() {}
				//use this setter
				//before : setCenter()
				//after : setImage() , setScale() , setScaleTo()
				ImageObj& setSizeAuto() {
					posRect.size = static_cast<sf::Vector2f>(image.getSize()).componentWiseMul(scale);
					return *this;
				}
			protected:
				sf::Texture image;
				sf::Vector2i justification = { attr::gui::Mid,attr::gui::Mid };
				sf::Vector2f scale = sf::Vector2f(1, 1);
				sf::Color imageMaskColors[3] = { sf::Color::White,sf::Color::White ,sf::Color::White };
				ImageObj& setImageMaskColor(Skipable<sf::Color>_normalMaskColor, Skipable<sf::Color>_overMaskColor, Skipable<sf::Color>_focusMaskColor) {
					_normalMaskColor.assignTo(imageMaskColors[attr::gui::Statu::normal]);
					_overMaskColor.assignTo(imageMaskColors[attr::gui::Statu::over]);
					_focusMaskColor.assignTo(imageMaskColors[attr::gui::Statu::focus]);
					return *this;
				}
				sf::Color& imageMaskColor(int id) {
					return imageMaskColors[id];
				}
				void draw(sf::RenderTarget& r, sf::FloatRect displayArea) {
					ObjBase::draw(r, displayArea);
					if (posRect.findIntersection(displayArea)) {
						sf::Sprite imageRender(image);
						imageRender.setPosition(posRect.position + ((posRect.size - static_cast<sf::Vector2f>(image.getSize()).componentWiseMul(scale)) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(justification)) - displayArea.position);
						imageRender.setScale(scale);
						imageRender.setColor(imageMaskColors[currentStatu]);
						r.draw(imageRender);
					}
				}
			public:
				ImageObj& setJustification(Skipable<attr::gui::TextJustification> xJus, Skipable<attr::gui::TextJustification> yJus) {
					xJus.assignTo(justification.x);
					yJus.assignTo(justification.y);
					return *this;
				}
				ImageObj& setScale(sf::Vector2f _scale) {
					scale = _scale;
					return *this;
				}
				//use this setter
				//after : setImage()
				ImageObj& setScaleTo(sf::Vector2f _size) {
					scale = _size.componentWiseDiv(static_cast<sf::Vector2f>(image.getSize()));
					return *this;
				}
				ImageObj& setImage(const sf::Image& _image) {
					bool loadSuccess = image.loadFromImage(_image);
					if (!loadSuccess)
						cerr << "[ImageObj::setImage] Image load failed 图像加载失败" << endl << "  id: " << id << endl;
					return *this;
				}
				friend inline BinaryFStream& operator>>(BinaryFStream& bf, ImageObj& x) {
					bf.structIn(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structIn(x.image, x.scale, x.justification, x.imageMaskColors[attr::gui::Statu::normal], x.imageMaskColors[attr::gui::Statu::over], x.imageMaskColors[attr::gui::Statu::focus]);
					return bf;
				}
				friend inline BinaryFStream& operator<<(BinaryFStream& bf, const ImageObj& x) {
					bf.structOut(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structOut(x.image, x.scale, x.justification, x.imageMaskColors[attr::gui::Statu::normal], x.imageMaskColors[attr::gui::Statu::over], x.imageMaskColors[attr::gui::Statu::focus]);
					return bf;
				}
			};
			class TextObj :public ObjBase {
				friend class WindowManager;
			public:
				TextObj() {
					textStyles[attr::gui::Statu::normal].set(sf::Color::Black, sf::Color::Black, Skip);
					textStyles[attr::gui::Statu::over].set(sf::Color::Black, sf::Color::Black, Skip);
					textStyles[attr::gui::Statu::focus].set(sf::Color::Black, sf::Color::Black, Skip);
				}
				TextObj& setTextStyle(Skipable<Style>_normalStyle, Skipable<Style>_overStyle, Skipable<Style>_focusStyle) {
					_normalStyle.assignTo(textStyles[attr::gui::Statu::normal]);
					_overStyle.assignTo(textStyles[attr::gui::Statu::over]);
					_focusStyle.assignTo(textStyles[attr::gui::Statu::focus]);
					return *this;
				}
				Style& textStyle(int id) {
					return textStyles[id];
				}
				//use this setter
				//before : setCenter()
				//after : setFont() , setText() , setCharacterSize()
				TextObj& setSizeAuto() {
					textRender.setFont(fontManager[font]);
					textRender.setCharacterSize(characterSize);
					textRender.setLineSpacing(lineSpacing);
					textRender.setLetterSpacing(letterSpacing);
					textRender.setString(text);
					posRect.size.x = 0;
					for (int i = 0; i <= text.getSize(); i++) {
						if (textRender.findCharacterPos(i).x > posRect.size.x)
							posRect.size.x = textRender.findCharacterPos(i).x;
					}
					posRect.size.y = textRender.findCharacterPos(textRender.getString().getSize()).y + characterSize;
					return *this;
				}
			protected:
				string font = "";
				unsigned int characterSize = 30;
				float letterSpacing = 1, lineSpacing = 1;
				sf::String text = "";
				sf::Text textRender{ fontManager[font] };
				sf::FloatRect textRect;
				sf::Vector2i justification = { attr::gui::Mid,attr::gui::Mid };
				Style textStyles[3];
				void draw(sf::RenderTarget& r, sf::FloatRect displayArea) {
					ObjBase::draw(r, displayArea);

					//render text
					textRender.setString("_");
					textRender.setCharacterSize(characterSize);
					textRender.setLineSpacing(lineSpacing);
					textRender.setLetterSpacing(letterSpacing);
					textRender.setFillColor(textStyles[currentStatu].backgroundColor);
					textRender.setOutlineColor(textStyles[currentStatu].outlineColor);
					textRender.setPosition({ 0,0 });
					//fix position offset
					sf::Vector2f offsetFix;
					offsetFix.x = textRender.getGlobalBounds().position.x;
					offsetFix.y = textRender.getGlobalBounds().position.y + textRender.getGlobalBounds().size.y - characterSize;
					textRender.setString(text);
					textRect.size.x = 0;
					for (int i = 0; i <= text.getSize(); i++) {
						if (textRender.findCharacterPos(i).x > textRect.size.x)
							textRect.size.x = textRender.findCharacterPos(i).x;
					}
					textRect.size.y = textRender.findCharacterPos(textRender.getString().getSize()).y + characterSize;
					textRender.setPosition(posRect.position - offsetFix + ((posRect.size - textRect.size) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(justification)) - displayArea.position);
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
						r.draw(textRender);
					}
				}
			public:
				TextObj& setFont(string _font) {
					font = _font;
					textRender.setFont(fontManager[font]);
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
				sf::String& getText() {
					return text;
				}
				friend inline BinaryFStream& operator>>(BinaryFStream& bf, TextObj& x) {
					bf.structIn(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structIn(x.textStyles[attr::gui::Statu::normal], x.textStyles[attr::gui::Statu::over], x.textStyles[attr::gui::Statu::focus],
								x.font, x.characterSize, x.justification, x.letterSpacing, x.lineSpacing, x.text);
					x.textRender.setFont(fontManager[x.font]);
					return bf;
				}
				friend inline BinaryFStream& operator<<(BinaryFStream& bf, const TextObj& x) {
					bf.structOut(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structOut(x.textStyles[attr::gui::Statu::normal], x.textStyles[attr::gui::Statu::over], x.textStyles[attr::gui::Statu::focus],
								 x.font, x.characterSize, x.justification, x.letterSpacing, x.lineSpacing, x.text);
					return bf;
				}
			};
			class ButtonObj :public TextObj {
				friend class WindowManager;
			public:
				ButtonObj() {
					styles[attr::gui::Statu::normal].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2);
					styles[attr::gui::Statu::over].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2);
					styles[attr::gui::Statu::focus].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2);
				}
			};
			class OptionObj :public ButtonObj {
				friend class WindowManager;
			};
			class InputObj :public TextObj {
				friend class WindowManager;
			public:
				InputObj() {
					styles[attr::gui::Statu::normal].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2);
					styles[attr::gui::Statu::over].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2);
					styles[attr::gui::Statu::focus].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2);
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
						ObjBase::draw(r, displayArea);
						sf::FloatRect displayAreaCur(-scroll, posRect.size);
						sf::RenderTexture rCur(static_cast<sf::Vector2u>(posRect.size));
						rCur.clear(sf::Color::Transparent);
						//draw text

						//render text
						textRender.setString("_");
						textRender.setCharacterSize(characterSize);
						textRender.setLineSpacing(lineSpacing);
						textRender.setLetterSpacing(letterSpacing);
						textRender.setFillColor(textStyles[currentStatu].backgroundColor);
						textRender.setOutlineColor(textStyles[currentStatu].outlineColor);
						textRender.setPosition({ 0,0 });
						//fix position offset
						sf::Vector2f offsetFix;
						offsetFix.x = textRender.getGlobalBounds().position.x;
						offsetFix.y = textRender.getGlobalBounds().position.y + textRender.getGlobalBounds().size.y - characterSize;
						textRender.setString(text);
						textRect.size.x = 0;
						for (int i = 0; i <= text.getSize(); i++) {
							if (textRender.findCharacterPos(i).x > textRect.size.x)
								textRect.size.x = textRender.findCharacterPos(i).x;
						}
						textRect.size.y = textRender.findCharacterPos(textRender.getString().getSize()).y + characterSize;
						textRender.setPosition(-offsetFix + ((posRect.size - textRect.size) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(justification)) - displayAreaCur.position);
						textRect.position = ((posRect.size - textRect.size) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(justification));
						sf::Vector2f cursorPos = textRender.findCharacterPos(cursor);
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
						rCur.draw(textRender);

						//draw cursor
						if (currentStatu == attr::gui::Statu::focus && (windowManager.cursorBlinkTick >= 0 && windowManager.cursorBlinkTick < windowManager.cursorBlinkRate / 2)) {
							Display::Draw::Line(
								rCur,
								cursorPos,
								cursorPos + sf::Vector2f(0, (float)characterSize),
								textStyles[currentStatu].backgroundColor,
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
					bf.structIn(x.textStyles[attr::gui::Statu::normal], x.textStyles[attr::gui::Statu::over], x.textStyles[attr::gui::Statu::focus],
								x.font, x.characterSize, x.justification, x.letterSpacing, x.lineSpacing, x.text);
					bf.structIn(x.sizeLimit,x.typeLimit, x.inputLimit);
					x.textRender.setFont(fontManager[x.font]);
					x.cursor = x.text.getSize();
					return bf;
				}
				friend inline BinaryFStream& operator<<(BinaryFStream& bf, const InputObj& x) {
					bf.structOut(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structOut(x.textStyles[attr::gui::Statu::normal], x.textStyles[attr::gui::Statu::over], x.textStyles[attr::gui::Statu::focus],
								 x.font, x.characterSize, x.justification, x.letterSpacing, x.lineSpacing, x.text);
					bf.structOut(x.sizeLimit, x.typeLimit, x.inputLimit);
					return bf;
				}
			};
			class AreaObj :public ObjBase {
				friend class WindowManager;
			protected:
				sf::Vector2f scroll;
				sf::Vector2i mouseDragScrollable, mouseWheelScrollable;
				sf::Vector2f scrollVelocity;
				sf::FloatRect scrollLimit;
				Statu optionFocus;
			public:
				AreaObj() {
					styles[attr::gui::Statu::normal].set(sf::Color::White, sf::Color(200, 200, 200), 2);
					styles[attr::gui::Statu::over].set(sf::Color::White, sf::Color(200, 200, 200), 2);
					styles[attr::gui::Statu::focus].set(sf::Color::White, sf::Color(200, 200, 200), 2);
				}
				OrderedHashMap<AreaObj>area;
				OrderedHashMap<InputObj>input;
				OrderedHashMap<ButtonObj>button;
				OrderedHashMap<OptionObj>option;
				OrderedHashMap<ImageObj>image;
				OrderedHashMap<TextObj>text;
				AreaObj& setScrollable(Skipable<sf::Vector2i> _mouseDragScrollable, Skipable<sf::Vector2i> _mouseWheelScrollable) {
					_mouseDragScrollable.assignTo(mouseDragScrollable);
					_mouseWheelScrollable.assignTo(mouseWheelScrollable);
					return *this;
				}
			protected:
				static inline void addPoint(sf::FloatRect& rect, sf::Vector2f point) {
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
					for (auto& elem : area.iterate()) {
						addPoint(scrollLimit, elem.posRect.position);
						addPoint(scrollLimit, elem.posRect.position + elem.posRect.size);
					}
					for (auto& elem : input.iterate()) {
						addPoint(scrollLimit, elem.posRect.position);
						addPoint(scrollLimit, elem.posRect.position + elem.posRect.size);
					}
					for (auto& elem : button.iterate()) {
						addPoint(scrollLimit, elem.posRect.position);
						addPoint(scrollLimit, elem.posRect.position + elem.posRect.size);
					}
					for (auto& elem : option.iterate()) {
						addPoint(scrollLimit, elem.posRect.position);
						addPoint(scrollLimit, elem.posRect.position + elem.posRect.size);
					}
					for (auto& elem : image.iterate()) {
						addPoint(scrollLimit, elem.posRect.position);
						addPoint(scrollLimit, elem.posRect.position + elem.posRect.size);
					}
					for (auto& elem : text.iterate()) {
						addPoint(scrollLimit, elem.posRect.position);
						addPoint(scrollLimit, elem.posRect.position + elem.posRect.size);
					}
					scrollLimit.size -= posRect.size;
					return *this;
				}
				friend inline BinaryFStream& operator>>(BinaryFStream& bf, AreaObj& x) {
					bf.structIn(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structIn(x.area, x.image, x.text, x.input, x.button,x.option);
					bf.structIn(x.mouseDragScrollable, x.mouseWheelScrollable, x.scrollLimit);
					return bf;
				}
				friend inline BinaryFStream& operator<<(BinaryFStream& bf, const AreaObj& x) {
					bf.structOut(x.id, x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus]);
					bf.structOut(x.area, x.image, x.text, x.input, x.button, x.option);
					bf.structOut(x.mouseDragScrollable, x.mouseWheelScrollable, x.scrollLimit);
					return bf;
				}
				AreaObj& setOption(string key) {
					if (optionFocus.count(attr::gui::OptionId)) {
						if (!option.count(optionFocus[attr::gui::OptionId].cast<string>()))
							optionFocus = {};
						else option[optionFocus[attr::gui::OptionId].cast<string>()].loseFocus();
					}
					if (option.count(key)) {
						optionFocus = { attr::gui::OptionId,key };
						option[key].gainFocus();
					}
					return *this;
				}
				AreaObj& setOptionNull() {
					if (option.count(optionFocus[attr::gui::OptionId].cast<string>()))
						option[optionFocus[attr::gui::OptionId].cast<string>()].loseFocus();
					optionFocus = {};
					return *this;
				}
			protected:
				void updateOptionFocus() {
					if (optionFocus.count(attr::gui::OptionId)) {
						if (!option.count(optionFocus[attr::gui::OptionId].cast<string>()))
							optionFocus = {};
						else option[optionFocus[attr::gui::OptionId].cast<string>()].gainFocus();
					}
				}
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
					if (scrollVelocity != sf::Vector2f()) {
						if (scrollVelocity.lengthSquared() < windowManager.scrollResistance * windowManager.scrollResistance) {
							scrollVelocity = sf::Vector2f();
						}
						else {
							scrollVelocity -= scrollVelocity
								.componentWiseDiv(sf::Vector2f(scrollVelocity.length(), scrollVelocity.length()))
								.componentWiseMul(sf::Vector2f(windowManager.scrollResistance, windowManager.scrollResistance));
						}
						scroll += scrollVelocity;
					}
					if (mouseDragScrollable != sf::Vector2i() || mouseWheelScrollable != sf::Vector2i())
						ensureScrollLimit();
					for (auto& elem : area.iterate()) {
						elem.updateScroll(windowManager);
					}
				}
				void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager) {
					if (!posRect.findIntersection(displayArea))
						return;
					ObjBase::draw(r, displayArea);
					sf::FloatRect displayAreaCur(-scroll, posRect.size);
					sf::RenderTexture rCur(static_cast<sf::Vector2u>(posRect.size));
					rCur.clear(sf::Color::Transparent);
					//clear
					for (auto& elem : area.iterate()) {
						elem.draw(rCur, displayAreaCur, windowManager);
					}
					for (auto& elem : input.iterate()) {
						elem.draw(rCur, displayAreaCur, windowManager);
					}
					for (auto& elem : button.iterate()) {
						elem.draw(rCur, displayAreaCur);
					}
					for (auto& elem : option.iterate()) {
						elem.draw(rCur, displayAreaCur);
					}
					for (auto& elem : image.iterate()) {
						elem.draw(rCur, displayAreaCur);
					}
					for (auto& elem : text.iterate()) {
						elem.draw(rCur, displayAreaCur);
					}
					rCur.display();
					sf::Sprite s(rCur.getTexture());
					s.setPosition(posRect.position - displayArea.position);
					r.draw(s);

				}
			};
		private:
			unordered_map<string, int>windowId;
			vector<AreaObj>windowData;
		public:
			OrderedHashMap<AreaObj>preset;
			void readPreset(BinaryFStream & bf) {
				bf >> preset;
			}
			void newWindow(string id) {
				if (windowId.count(id)) {
					cerr << "[WindowManager::newWindow] Window ID already exists 窗口ID重复\n  id: " << id << "\n";
					throw runtime_error("[WindowManager::newWindow] Window ID already exists 窗口ID重复\n  id: " + id + "\n");
				}
				if (preset.count(id))
					windowData.push_back(preset[id]);
				else windowData.emplace_back();
				windowData[windowData.size() - 1].id = id;
				windowId[id] = (int)windowData.size() - 1;
			}
			bool topWindowIs(string id) {
				return windowData.size() > 0 && windowData.back().id == id;
			}
			void closeTopWindow() {
				if (windowData.size() > 0) {
					windowId.erase(windowData.back().id);
					windowData.pop_back();
				}
				else cerr << "[WindowManager::closeTopWindow] No window to close 无窗口，无法关闭\n";
			}
			AreaObj& window(string id) {
				if (windowId.count(id))
					return windowData[windowId[id]];
				else {
					cerr << "[WindowManager::window] Window ID not found 未找到指定窗口ID\n  id: " << id << "\n";
					throw std::runtime_error("[WindowManager::window] Window ID not found 未找到指定窗口ID\n  id: " + id + "\n");
				}
			}
		private:
			pair<AreaObj*,string> visit(string path) {
				AreaObj* areaPtr = nullptr;
				string temp = "";
				for (int i = 0; i < path.size(); i++) {
					if (path[i] == '_') {
						if (areaPtr == nullptr)
							areaPtr = &window(temp);
						else areaPtr = &(areaPtr->area[temp]);
						temp = "";
					}
					else temp.push_back(path[i]);
				}
				return make_pair(areaPtr, temp);
			}
		public:
			//通过路径访问area 
			AreaObj& area(string path) {
				auto temp = visit(path);
				if (temp.first == nullptr)
					return window(temp.second);
				else return temp.first->area[temp.second];
			}
			//通过路径访问input
			InputObj& input(string path) {
				auto temp = visit(path);
				return temp.first->input[temp.second];
			}
			//通过路径访问button
			ButtonObj& button(string path) {
				auto temp = visit(path);
				return temp.first->button[temp.second];
			}
			//通过路径访问option
			OptionObj& option(string path) {
				auto temp = visit(path);
				return temp.first->option[temp.second];
			}
			//通过路径访问image
			ImageObj& image(string path) {
				auto temp = visit(path);
				return temp.first->image[temp.second];
			}
			//通过路径访问text
			TextObj& text(string path) {
				auto temp = visit(path);
				return temp.first->text[temp.second];
			}
		private:
			inline AreaObj* updateOver(AreaObj* areaPtr,bool stopScroll=false) {
				if (statuExist(overFocus)) {
					if (overFocus.count(attr::gui::ButtonId)) {
						area(overFocus[attr::gui::AreaPath].cast<string>())
							.button[overFocus[attr::gui::ButtonId].cast<string>()].loseOver();
					}
					if (overFocus.count(attr::gui::OptionId)) {
						area(overFocus[attr::gui::AreaPath].cast<string>())
							.option[overFocus[attr::gui::OptionId].cast<string>()].loseOver();
					}
					else if (overFocus.count(attr::gui::InputId)) {
						area(overFocus[attr::gui::AreaPath].cast<string>())
							.input[overFocus[attr::gui::InputId].cast<string>()].loseOver();
					}
					else if (overFocus.count(attr::gui::AreaPath)) {
						area(overFocus[attr::gui::AreaPath].cast<string>()).loseOver();
					}
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
					for (auto& elem : areaPtr->area.iterate()) {
						if (elem.posRect.contains(mousePos.back() - areaPtr->scroll - origin)) {
							origin += elem.posRect.position + areaPtr->scroll;
							areaPtr = &elem;
							goto LabelFindSuccess;
						}
					}
					break;
				LabelFindSuccess:;
				}
				overFocus[attr::gui::AreaPath] = path;
				for (auto& elem : areaPtr->button.iterate()) {
					if (elem.posRect.contains(mousePos.back() - areaPtr->scroll - origin)) {
						overFocus[attr::gui::ButtonId] = elem.id;
						goto LabelReturn;
					}
				}
				for (auto& elem : areaPtr->option.iterate()) {
					if (elem.posRect.contains(mousePos.back() - areaPtr->scroll - origin)) {
						overFocus[attr::gui::OptionId] = elem.id;
						goto LabelReturn;
					}
				}
				for (auto& elem : areaPtr->input.iterate()) {
					if (elem.posRect.contains(mousePos.back() - areaPtr->scroll - origin)) {
						overFocus[attr::gui::InputId] = elem.id;
						goto LabelReturn;
					}
				}
			LabelReturn:;
				return areaPtr;
			}
			inline bool statuExist(Statu& statu) {
				AreaObj* areaPtr = nullptr;
				string temp = "";
				if (!statu.count(attr::gui::AreaPath)) {
					return false;
				}
				string& path = statu[attr::gui::AreaPath].cast<string>();
				for (int i = 0; i < path.size(); i++) {
					if (path[i] == '_') {
						if (areaPtr == nullptr) {
							if (!windowId.count(temp))return false;
							areaPtr = &window(temp);
						}
						else {
							if (!areaPtr->area.count(temp))return false;
							areaPtr = &(areaPtr->area[temp]);
						}
						temp = "";
					}
					else temp.push_back(path[i]);
				}
				if (areaPtr == nullptr) {
					if (!windowId.count(temp))return false;
					areaPtr = &window(temp);
				}
				else {
					if (!areaPtr->area.count(temp))return false;
					areaPtr = &(areaPtr->area[temp]);
				}
				if (statu.count(attr::gui::ButtonId))
					return areaPtr->button.count(statu[attr::gui::ButtonId].cast<string>());
				if (statu.count(attr::gui::OptionId))
					return areaPtr->option.count(statu[attr::gui::OptionId].cast<string>());
				if (statu.count(attr::gui::InputId))
					return areaPtr->input.count(statu[attr::gui::InputId].cast<string>());
				return true;
			}
			inline ObjBase* statuVisit(Statu& statu, AreaObj* areaPtr = nullptr) {
				if (statu.count(attr::gui::AreaPath) && areaPtr == nullptr)
					areaPtr = &area(statu[attr::gui::AreaPath].cast<string>());
				if (statu.count(attr::gui::ButtonId))
					return &areaPtr->button[statu[attr::gui::ButtonId].cast<string>()];
				else if (statu.count(attr::gui::OptionId))
					return &areaPtr->option[statu[attr::gui::OptionId].cast<string>()];
				else if (statu.count(attr::gui::InputId))
					return &areaPtr->input[statu[attr::gui::InputId].cast<string>()];
				else if (statu.count(attr::gui::AreaPath))
					return areaPtr;
				else return nullptr;
			}
			inline void updateSimpleMove(AreaObj* areaFocusPtr,AreaObj* areaOverPtr) {
				//update scroll
				if (areaFocusPtr != nullptr && mousePressed) {
					if (areaFocusPtr->mouseDragScrollable != sf::Vector2i()) {
						if (ensure(focus.count(attr::gui::ButtonId) || focus.count(attr::gui::OptionId), (mouseLastPressPos - mousePos.back()).lengthSquared() >= scrollThreshold * scrollThreshold))
							isDragScrolling = true;
						if (focus.count(attr::gui::ButtonId) && isDragScrolling) {
							if (overFocus.contain(attr::gui::AreaPath, focus[attr::gui::AreaPath].cast<string>(), attr::gui::ButtonId, focus[attr::gui::ButtonId].cast<string>()))
								areaFocusPtr->button[focus[attr::gui::ButtonId].cast<string>()].loseFocus().gainOver();
							else areaFocusPtr->button[focus[attr::gui::ButtonId].cast<string>()].loseFocus();
						}
						if (focus.count(attr::gui::OptionId) && isDragScrolling) {
							if (overFocus.contain(attr::gui::AreaPath, focus[attr::gui::AreaPath].cast<string>(), attr::gui::OptionId, focus[attr::gui::OptionId].cast<string>()))
								areaFocusPtr->option[focus[attr::gui::OptionId].cast<string>()].loseFocus().gainOver();
							else areaFocusPtr->option[focus[attr::gui::OptionId].cast<string>()].loseFocus();
							areaFocusPtr->updateOptionFocus();
						}
					}
					else {
						if (areaFocusPtr != nullptr && focus.count(attr::gui::ButtonId)) {
							if (overFocus.contain(attr::gui::AreaPath, focus[attr::gui::AreaPath].cast<string>(), attr::gui::ButtonId, focus[attr::gui::ButtonId].cast<string>()))
								areaFocusPtr->button[focus[attr::gui::ButtonId].cast<string>()].gainFocus();
							else areaFocusPtr->button[focus[attr::gui::ButtonId].cast<string>()].loseFocus().gainOver();
						}
						if (areaFocusPtr != nullptr && focus.count(attr::gui::OptionId)) {
							if (overFocus.contain(attr::gui::AreaPath, focus[attr::gui::AreaPath].cast<string>(), attr::gui::OptionId, focus[attr::gui::OptionId].cast<string>()))
								areaFocusPtr->option[focus[attr::gui::OptionId].cast<string>()].gainFocus();
							else areaFocusPtr->option[focus[attr::gui::OptionId].cast<string>()].loseFocus().gainOver();
							areaFocusPtr->updateOptionFocus();
						}
					}
				}
				//update over
				//after updating over ,varible 'focus' will not be changed
				else {
					if (statuVisit(overFocus, areaOverPtr) != nullptr)
						statuVisit(overFocus, areaOverPtr)->gainOver();
				}
			}
		public:
			bool update(const optional<sf::Event>& sfEvent) {
				if (windowData.size() == 0)
					return false;
				//update mouse press statu
				if (sfEvent->is<sf::Event::MouseButtonPressed>()) {
					mousePressed = true;
					mouseLastPressPos = static_cast<sf::Vector2f>(sfEvent->getIf<sf::Event::MouseButtonPressed>()->position);
				}
				if (sfEvent->is<sf::Event::MouseButtonReleased>())
					mousePressed = false;
				string path = "";
				AreaObj* topWindow = &windowData.back();
				if (!statuExist(focus))
					focus = {};
				AreaObj* areaOverPtr = nullptr, * areaFocusPtr = nullptr;
				if (focus.count(attr::gui::AreaPath))
					areaFocusPtr = &area(focus[attr::gui::AreaPath].cast<string>());
				//sf::Event::MouseButtonPressed
				//update focus
				//after updating focus ,varible 'focus' & 'overFocus' must have a value
				if (sfEvent->is<sf::Event::MouseButtonPressed>()) {
					mousePos.back() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseButtonPressed>()->position);//update mousePos

					//update inertial scroll stop
					areaOverPtr=updateOver(topWindow,true);

					if (focus.count(attr::gui::InputId)&&!overFocus.contain(attr::gui::AreaPath,focus[attr::gui::AreaPath].cast<string>(), attr::gui::InputId, focus[attr::gui::InputId].cast<string>()))
						eventList.push(Event(attr::gui::InputLoseFocus, { attr::gui::InputPath,focus[attr::gui::AreaPath].cast<string>() + '_' + focus[attr::gui::InputId].cast<string>() }));
					if (overFocus.count(attr::gui::InputId) && !focus.contain(attr::gui::AreaPath, overFocus[attr::gui::AreaPath].cast<string>(), attr::gui::InputId, overFocus[attr::gui::InputId].cast<string>()))
						eventList.push(Event(attr::gui::InputGainFocus, { attr::gui::InputPath,overFocus[attr::gui::AreaPath].cast<string>() + '_' + overFocus[attr::gui::InputId].cast<string>() }));

					if (statuVisit(focus)!=nullptr)
						statuVisit(focus)->loseFocus();
					if (areaFocusPtr != nullptr)
						areaFocusPtr->updateOptionFocus();
					if (statuVisit(overFocus,areaOverPtr) != nullptr)
						statuVisit(overFocus,areaOverPtr)->gainFocus();

					focus = overFocus;
					return true;
				}
				//sf::Event::MouseMoved
				if (sfEvent->is<sf::Event::MouseMoved>()) {
					sf::Vector2f mousePosDelta = sf::Vector2f(sfEvent->getIf<sf::Event::MouseMoved>()->position) - mousePos.back();
					mousePos.back() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseMoved>()->position);//update mousePos
					areaOverPtr=updateOver(topWindow);
					if (areaFocusPtr != nullptr && mousePressed && areaFocusPtr->mouseDragScrollable != sf::Vector2i() && isDragScrolling) {
						//consider the case of two continuous sf::Event::MouseMoved event
						areaFocusPtr->scroll += mousePosDelta.componentWiseMul(static_cast<sf::Vector2f>(areaFocusPtr->mouseDragScrollable));
					}
					updateSimpleMove(areaFocusPtr, areaOverPtr);
					return true;
				}
				//sf::Event::MouseButtonReleased 
				//update release
				//after updating release ,varible 'focus' will not be changed
				if (sfEvent->is<sf::Event::MouseButtonReleased>()) {
					mousePos.back() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseButtonReleased>()->position);//update mousePos
					areaOverPtr = updateOver(topWindow);

					if (areaFocusPtr != nullptr && focus.count(attr::gui::ButtonId)) {
						if (overFocus.contain(attr::gui::AreaPath, focus[attr::gui::AreaPath].cast<string>(), attr::gui::ButtonId, focus[attr::gui::ButtonId].cast<string>())) {
							if (!isDragScrolling)
								eventList.push(Event(attr::gui::ButtonPressed, { attr::gui::ButtonPath,focus[attr::gui::AreaPath].cast<string>() + '_' + focus[attr::gui::ButtonId].cast<string>() }));
							areaFocusPtr->button[focus[attr::gui::ButtonId].cast<string>()].loseFocus().gainOver();
						}
						else areaFocusPtr->button[focus[attr::gui::ButtonId].cast<string>()].loseFocus();
					}
					if (areaFocusPtr != nullptr && focus.count(attr::gui::OptionId)) {
						if (areaFocusPtr->optionFocus.count(attr::gui::OptionId))
							areaFocusPtr->option[areaFocusPtr->optionFocus[attr::gui::OptionId].cast<string>()].loseFocus();
						if (overFocus.contain(attr::gui::AreaPath, focus[attr::gui::AreaPath].cast<string>(), attr::gui::OptionId, focus[attr::gui::OptionId].cast<string>())) {
							if (!isDragScrolling) {
								areaFocusPtr->optionFocus = { attr::gui::OptionId,focus[attr::gui::OptionId].cast<string>() };
								eventList.push(Event(attr::gui::OptionChosen, { attr::gui::OptionPath,focus[attr::gui::AreaPath].cast<string>() + '_' + focus[attr::gui::OptionId].cast<string>() }));
							}
						}
						else areaFocusPtr->option[focus[attr::gui::OptionId].cast<string>()].loseFocus();
						areaFocusPtr->updateOptionFocus();
					}
					isDragScrolling = false;

					//update inertial scroll start
					if (areaFocusPtr != nullptr && focus.count(attr::gui::AreaPath)) {
						if (areaFocusPtr->mouseDragScrollable != sf::Vector2i())
							areaFocusPtr->scrollVelocity = mouseVelocity()
								.componentWiseMul(static_cast<sf::Vector2f>(areaFocusPtr->mouseDragScrollable))
								.componentWiseDiv(sf::Vector2f(scrollSensitivity,scrollSensitivity));
					}
					return true;
				}
				if (sfEvent->is<sf::Event::TextEntered>()) {
					if (areaFocusPtr != nullptr && focus.count(attr::gui::InputId)) {
						char32_t ch = sfEvent->getIf<sf::Event::TextEntered>()->unicode;
						InputObj& InputTar = areaFocusPtr->input[focus[attr::gui::InputId].cast<string>()];
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
					if (areaFocusPtr != nullptr && focus.count(attr::gui::InputId)) {
						InputObj& InputTar = areaFocusPtr->input[focus[attr::gui::InputId].cast<string>()];
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
				if (sfEvent->is<sf::Event::MouseWheelScrolled>()) {
					areaOverPtr = updateOver(topWindow);
					float delta = sfEvent->getIf<sf::Event::MouseWheelScrolled>()->delta;
					if (areaOverPtr->mouseWheelScrollable == sf::Vector2i(1, 0))
						areaOverPtr->scrollVelocity = sf::Vector2f(delta*mouseWheelScrollRate,0);
					if (areaOverPtr->mouseWheelScrollable == sf::Vector2i(0, 1))
						areaOverPtr->scrollVelocity = sf::Vector2f(0, delta * mouseWheelScrollRate);
					if (areaOverPtr->mouseWheelScrollable == sf::Vector2i(1, 1)) {
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RShift))
							areaOverPtr->scrollVelocity = sf::Vector2f(delta * mouseWheelScrollRate, 0);
						else areaOverPtr->scrollVelocity = sf::Vector2f(0, delta * mouseWheelScrollRate);
					}
					updateSimpleMove(areaFocusPtr, areaOverPtr);
					return true;
				}
				return false;
			}
			//draw all objects to drawTarget
			//update cursor tick
			//update inertial scroll
			void draw(sf::RenderTarget& drawTarget) {
				if (windowData.size() >= 1) {
					if (!statuExist(focus))
						focus = {};
					updateSimpleMove(focus.count(attr::gui::AreaPath) ? &area(focus[attr::gui::AreaPath].cast<string>()) : nullptr, updateOver(&windowData.back()));
				}
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
#ifdef DEBUG
#undef private
#undef protected
#endif