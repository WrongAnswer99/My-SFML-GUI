//Author : WrongAnswer99
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <any>
#include <vector>
#include <variant>
#include <stdarg.h>
#include <unordered_map>
#include <queue>
#include "mainBase.cpp"

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
inline static BinaryFStream& operator>>(BinaryFStream& fin, unordered_map<T, U>& x) {
	size_t size;
	fin >> size;
	T t{}; U u{};
	for (int i = 0; i < size; i++) {
		fin >> t >> u;
		x.emplace(t, u);
	}
	return fin;
}
template<typename T, typename U>
inline static BinaryFStream& operator<<(BinaryFStream& fout, const unordered_map<T, U> x) {
	fout << x.size();
	for (auto& elem : x) {
		fout << elem.first << elem.second;
	}
	return fout;
}
inline static BinaryFStream& operator>>(BinaryFStream& fin,sf::Color& x) {
	fin >> x.r >> x.g >> x.b >> x.a;
	return fin;
}
inline static BinaryFStream& operator<<(BinaryFStream& fout, const sf::Color& x) {
	fout << x.r << x.g << x.b << x.a;
	return fout;
}
template<typename T>
inline static BinaryFStream& operator>>(BinaryFStream& fin, sf::Vector2<T>& x) {
	fin >> x.x >> x.y;
	return fin;
}
template<typename T>
inline static BinaryFStream& operator<<(BinaryFStream& fout, const sf::Vector2<T>& x) {
	fout << x.x << x.y;
	return fout;
}
template<typename T>
inline static BinaryFStream& operator>>(BinaryFStream& fin, sf::Rect<T>& x) {
	fin >> x.position >> x.size;
	return fin;
}
template<typename T>
inline static BinaryFStream& operator<<(BinaryFStream& fout, const sf::Rect<T>& x) {
	fout << x.position << x.size;
	return fout;
}
inline static BinaryFStream& operator>>(BinaryFStream& fin, sf::String& x) {
	string s;
	fin >> s;
	x = sf::String::fromUtf8(s.begin(), s.end());
	return fin;
}
inline static BinaryFStream& operator<<(BinaryFStream& fout, const sf::String& x) {
	fout << x.toUtf8().size();
	for (uint8_t& elem : x.toUtf8())
		fout << elem;
	return fout;
}
namespace game {
	namespace gui {
		class WindowManager:public EventManager {
		private:
			Statu focus,overFocus;
			bool mousePressed=false;
			unsigned int cursorBlinkTick = 0;
			//cursorBlinkRate : how many ticks the cursor blinks
			const unsigned int cursorBlinkRate = 30;
			const float scrollResistance=30.f;
			template<typename T,int size>
			class RollArray {
				T array[size];
				int realSize = 0;
				int backPos = 0;
			public:
				inline void emplace_back() {
					if (realSize < size) {
						realSize++;
						backPos = realSize - 1;
					}
					else {
						backPos = (backPos + 1) % size;
					}
				}
				inline T& front() {
					if (realSize < size) {
						return array[0];
					}
					else {
						return array[(backPos + 1) % size];
					}
				}
				inline T& back() {
					return array[backPos];
				}
				inline bool copy_back() {
					emplace_back();
					array[backPos] = array[(backPos - 1 + size) % size];
					return true;
				}
			};
			//scroll sensitivity = 6 tick = 0.1 s (60 FPS)
			RollArray<sf::Vector2f,6>mousePos;
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
				sf::Color textColor = sf::Color(0, 0, 0, 0);
				string font="";
				unsigned int characterSize = 30;
				float letterSpacing = 1, lineSpacing = 1;
			public:
				Style() {}
				void set(Skipable<sf::Color> _backgroundColor, Skipable<sf::Color>_outlineColor, Skipable<float>_outlineThickness, Skipable<sf::Color>_textColor, Skipable<string>_font,Skipable<unsigned int>_characterSize, Skipable<float>_letterSpacing, Skipable<float>_lineSpacing) {
					_backgroundColor.assignTo(backgroundColor);
					_outlineColor.assignTo(outlineColor);
					_outlineThickness.assignTo(outlineThickness);
					_textColor.assignTo(textColor);
					_font.assignTo(font);
					_characterSize.assignTo(characterSize);
					_letterSpacing.assignTo(letterSpacing);
					_lineSpacing.assignTo(lineSpacing);
				}
				friend inline BinaryFStream& operator>>(BinaryFStream& fin, Style& x) {
					return fin.structIn(x.backgroundColor, x.outlineColor, x.outlineThickness, x.textColor, x.font, x.characterSize, x.letterSpacing, x.lineSpacing);
				}
				friend inline BinaryFStream& operator<<(BinaryFStream& fout,const Style& x) {
					return fout.structOut(x.backgroundColor, x.outlineColor, x.outlineThickness, x.textColor, x.font, x.characterSize, x.letterSpacing, x.lineSpacing);
				}
			};
		private:
			class ObjBase {
				friend class WindowManager;
			protected:
				string id;
				sf::FloatRect posRect=sf::FloatRect(sf::Vector2f(),sf::Vector2f(1.f,1.f));
				Style styles[3];
				int currentStatu=attr::gui::Statu::normal;
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
				//use this setter
				//before setGeneralStyle()
				ObjBase& setStyle(Skipable<Style>_normalStyle, Skipable<Style>_overStyle, Skipable<Style>_focusStyle) {
					_normalStyle.assignTo(styles[attr::gui::Statu::normal]);
					_overStyle.assignTo(styles[attr::gui::Statu::over]);
					_focusStyle.assignTo(styles[attr::gui::Statu::focus]);
					return *this;
				}
				Style& style(int id) {
					return styles[id];
				}
				//use this setter
				//after setStyle()
				ObjBase& setGeneralStyle(Skipable<sf::Color> _backgroundColor, Skipable<sf::Color>_outlineColor, Skipable<float>_outlineThickness, Skipable<sf::Color>_textColor, Skipable<string>_font, Skipable<unsigned int>_characterSize, Skipable<float>_letterSpacing, Skipable<float>_lineSpacing) {
					_backgroundColor.assignTo(styles[attr::gui::Statu::normal].backgroundColor);
					_outlineColor.assignTo(styles[attr::gui::Statu::normal].outlineColor);
					_outlineThickness.assignTo(styles[attr::gui::Statu::normal].outlineThickness);
					_textColor.assignTo(styles[attr::gui::Statu::normal].textColor);
					_font.assignTo(styles[attr::gui::Statu::normal].font);
					_characterSize.assignTo(styles[attr::gui::Statu::normal].characterSize);
					_letterSpacing.assignTo(styles[attr::gui::Statu::normal].letterSpacing);
					_lineSpacing.assignTo(styles[attr::gui::Statu::normal].lineSpacing);

					_backgroundColor.assignTo(styles[attr::gui::Statu::over].backgroundColor);
					_outlineColor.assignTo(styles[attr::gui::Statu::over].outlineColor);
					_outlineThickness.assignTo(styles[attr::gui::Statu::over].outlineThickness);
					_textColor.assignTo(styles[attr::gui::Statu::over].textColor);
					_font.assignTo(styles[attr::gui::Statu::over].font);
					_characterSize.assignTo(styles[attr::gui::Statu::over].characterSize);
					_letterSpacing.assignTo(styles[attr::gui::Statu::over].letterSpacing);
					_lineSpacing.assignTo(styles[attr::gui::Statu::over].lineSpacing);

					_backgroundColor.assignTo(styles[attr::gui::Statu::focus].backgroundColor);
					_outlineColor.assignTo(styles[attr::gui::Statu::focus].outlineColor);
					_outlineThickness.assignTo(styles[attr::gui::Statu::focus].outlineThickness);
					_textColor.assignTo(styles[attr::gui::Statu::focus].textColor);
					_font.assignTo(styles[attr::gui::Statu::focus].font);
					_characterSize.assignTo(styles[attr::gui::Statu::focus].characterSize);
					_letterSpacing.assignTo(styles[attr::gui::Statu::focus].letterSpacing);
					_lineSpacing.assignTo(styles[attr::gui::Statu::focus].lineSpacing);
					return *this;
				}
				int getStatu() const {
					return currentStatu;
				}
			};
		public:
			class TextObj:public ObjBase{
				friend class WindowManager;
			public:
				TextObj() {
					styles[attr::gui::Statu::normal].set(Skip, Skip, Skip, sf::Color::Black, Skip, Skip, Skip, Skip);
					styles[attr::gui::Statu::over].set(Skip, Skip, Skip, sf::Color::Black, Skip, Skip, Skip, Skip);
					styles[attr::gui::Statu::focus].set(Skip, Skip, Skip, sf::Color::Black, Skip, Skip, Skip, Skip);
				}
				//use this setter
				//before : setCenter()
				//after : setStyle() , setGeneralStyle() , setText()
				TextObj& setSizeSyncText() {
					textObj.setFont(game::fontManager[styles[currentStatu].font]);
					textObj.setCharacterSize(styles[currentStatu].characterSize);
					textObj.setLineSpacing(styles[currentStatu].lineSpacing);
					textObj.setLetterSpacing(styles[currentStatu].letterSpacing);
					textObj.setString(text);
					posRect.size.x = 0;
					for (int i = 0; i <= text.getSize(); i++) {
						if (textObj.findCharacterPos(i).x > posRect.size.x)
							posRect.size.x = textObj.findCharacterPos(i).x;
					}
					posRect.size.y = textObj.findCharacterPos(textObj.getString().getSize()).y + styles[currentStatu].characterSize;
					return *this;
				}
			protected:
				sf::String text="";
				sf::Text textObj{ fontManager[styles[attr::gui::Statu::normal].font] };
				sf::FloatRect textRect;
				sf::Vector2i justification = { attr::gui::Mid,attr::gui::Mid };
				void draw(sf::RenderTarget& r, sf::FloatRect displayArea) {
					drawBase(r, displayArea);

					//render text
					textObj.setString("_");
					textObj.setFont(game::fontManager[styles[currentStatu].font]);
					textObj.setCharacterSize(styles[currentStatu].characterSize);
					textObj.setLineSpacing(styles[currentStatu].lineSpacing);
					textObj.setLetterSpacing(styles[currentStatu].letterSpacing);
					textObj.setFillColor(styles[currentStatu].textColor);
					textObj.setPosition({ 0,0 });
					//fix position offset
					sf::Vector2f offsetFix;
					offsetFix.x = textObj.getGlobalBounds().position.x;
					offsetFix.y = textObj.getGlobalBounds().position.y + textObj.getGlobalBounds().size.y - styles[currentStatu].characterSize;
					textObj.setString(text);
					textRect.size.x = 0;
					for (int i = 0; i <= text.getSize(); i++) {
						if (textObj.findCharacterPos(i).x > textRect.size.x)
							textRect.size.x = textObj.findCharacterPos(i).x;
					}
					textRect.size.y = textObj.findCharacterPos(textObj.getString().getSize()).y + styles[currentStatu].characterSize;
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
				/*
				friend inline BinaryFStream& operator>>(BinaryFStream& fin, TextObj& x) {
					fin >> x.text;
					return fin.structIn(x.id,x.posRect,x.styles, x.text,x.justification);
				}
				friend inline BinaryFStream& operator<<(BinaryFStream& fout, const TextObj& x) {
					return fout.structOut();
				}*/
			public:
				TextObj& setJustification(Skipable<int> xJus, Skipable<int> yJus) {
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
			};
			class ButtonObj :public TextObj {
				friend class WindowManager;
			public:
				ButtonObj() {
					styles[attr::gui::Statu::normal].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2, sf::Color::Black, Skip, Skip, Skip, Skip);
					styles[attr::gui::Statu::over].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2, sf::Color::Black, Skip, Skip, Skip, Skip);
					styles[attr::gui::Statu::focus].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2, sf::Color::Black, Skip, Skip, Skip, Skip);
				}
			private:
				void updatePress(string& path, WindowManager& windowManager) {
					//按下按钮控件
					currentStatu = attr::gui::Statu::focus;
					windowManager.focus = { attr::gui::AreaPath, path,
											attr::gui::ButtonId, id };
					windowManager.overFocus = { attr::gui::AreaPath, path,
												attr::gui::ButtonId, id };
				}
				void updateOverMouseUnpressed(string& path, WindowManager& windowManager) {
					//未按下鼠标时移动到按钮控件
					if (currentStatu != attr::gui::Statu::focus) {
						currentStatu = attr::gui::Statu::over;
						windowManager.overFocus = { attr::gui::AreaPath, path,
													attr::gui::ButtonId, id };
					}
				}
				void updateOverMousePressed(string& path, WindowManager& windowManager) {
					//窗口不滚动且移动到按钮控件
					if (windowManager.focus.count(attr::gui::ButtonId) &&
						windowManager.focus[attr::gui::AreaPath].cast<string>() == path &&
						windowManager.focus[attr::gui::ButtonId].cast<string>() == id) {
						currentStatu = attr::gui::Statu::focus;
					}
					else {
						if (windowManager.focus.count(attr::gui::ButtonId)) {
							windowManager.operator[](windowManager.focus[attr::gui::AreaPath].cast<string>())
								.button(windowManager.focus[attr::gui::ButtonId].cast<string>()).currentStatu = attr::gui::Statu::over;
						}
						if (windowManager.focus.count(attr::gui::InputId)) {
							windowManager.operator[](windowManager.focus[attr::gui::AreaPath].cast<string>())
								.input(windowManager.focus[attr::gui::InputId].cast<string>()).currentStatu = attr::gui::Statu::over;
						}
					}
				}
				void updateRelease(string& path, WindowManager& windowManager) {
					//释放按钮控件
					currentStatu = attr::gui::Statu::over;
					windowManager.overFocus = { attr::gui::AreaPath, path,
												attr::gui::ButtonId, id };
					if (windowManager.focus.count(attr::gui::ButtonId) && windowManager.focus[attr::gui::AreaPath].cast<string>() == path && windowManager.focus[attr::gui::ButtonId].cast<string>() == id) {
						windowManager.eventList.push(game::Event(attr::gui::ButtonPressed, { attr::gui::ButtonPath,path + '_' + id }));
					}
				}
			};
			class InputObj :public TextObj{
				friend class WindowManager;
			public:
				InputObj() {
					styles[attr::gui::Statu::normal].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2, sf::Color::Black, Skip, Skip, Skip, Skip);
					styles[attr::gui::Statu::over].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2, sf::Color::Black, Skip, Skip, Skip, Skip);
					styles[attr::gui::Statu::focus].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2, sf::Color::Black, Skip, Skip, Skip, Skip);
				}
			protected:
				bool oneLineLimit = false;
				size_t sizeLimit = INT_MAX;
				size_t cursor = 0;
				sf::Vector2f scroll;
				void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager) {
					if (posRect.findIntersection(displayArea)) {
						drawBase(r, displayArea);
						sf::FloatRect displayAreaCur(-scroll, posRect.size);
						sf::RenderTexture rCur(static_cast<sf::Vector2u>(posRect.size));
						//draw text

						//render text
						textObj.setString("_");
						textObj.setFont(game::fontManager[styles[currentStatu].font]);
						textObj.setCharacterSize(styles[currentStatu].characterSize);
						textObj.setLineSpacing(styles[currentStatu].lineSpacing);
						textObj.setLetterSpacing(styles[currentStatu].letterSpacing);
						textObj.setFillColor(styles[currentStatu].textColor);
						textObj.setPosition({ 0,0 });
						//fix position offset
						sf::Vector2f offsetFix;
						offsetFix.x = textObj.getGlobalBounds().position.x;
						offsetFix.y = textObj.getGlobalBounds().position.y + textObj.getGlobalBounds().size.y - styles[currentStatu].characterSize;
						textObj.setString(text);
						textRect.size.x = 0;
						for (int i = 0; i <= text.getSize(); i++) {
							if (textObj.findCharacterPos(i).x > textRect.size.x)
								textRect.size.x = textObj.findCharacterPos(i).x;
						}
						textRect.size.y = textObj.findCharacterPos(textObj.getString().getSize()).y + styles[currentStatu].characterSize;
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
							else if (cursorPos.y + styles[currentStatu].characterSize > posRect.size.y)
								scroll.y += posRect.size.y - (cursorPos.y + styles[currentStatu].characterSize);
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
								cursorPos + sf::Vector2f(0, (float)styles[currentStatu].characterSize),
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
			private:
				void updatePress(string& path, WindowManager& windowManager) {
					//按下输入框控件
					currentStatu = attr::gui::Statu::focus;
					windowManager.focus = { attr::gui::AreaPath, path,
											attr::gui::InputId, id };
					windowManager.overFocus = { attr::gui::AreaPath, path,
												attr::gui::InputId, id };
				}
				void updateOverMouseUnpressed(string& path, WindowManager& windowManager) {
					//未按下鼠标时移动到输入框控件
					if (currentStatu != attr::gui::Statu::focus) {
						currentStatu = attr::gui::Statu::over;
						windowManager.overFocus = { attr::gui::AreaPath, path,
													attr::gui::InputId, id };
					}
				}
			public:
				InputObj& setLimit(Skipable<bool> _oneLineLimit, Skipable<int> _sizeLimit) {
					_oneLineLimit.assignTo(oneLineLimit);
					_sizeLimit.assignTo(sizeLimit);
					return *this;
				}
				InputObj& setText(sf::String _text) {
					text = _text;
					cursor = text.getSize();
					return *this;
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
					styles[attr::gui::Statu::normal].set(sf::Color::White, sf::Color(200, 200, 200), 2, Skip, Skip, Skip, Skip, Skip);
					styles[attr::gui::Statu::over].set(sf::Color::White, sf::Color(200, 200, 200), 2, Skip, Skip, Skip, Skip, Skip);
					styles[attr::gui::Statu::focus].set(sf::Color::White, sf::Color(200, 200, 200), 2, Skip, Skip, Skip, Skip, Skip);
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
				AreaObj& setScrollable(bool scrollableX,bool scrollableY) {
					scrollable = sf::Vector2f(scrollableX, scrollableY);
					return *this;
				}
			private:
				inline void addPoint(sf::FloatRect &rect,sf::Vector2f point) {
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
						addPoint(scrollLimit,elem.posRect.position);
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
				void updatePress(string& path, WindowManager& windowManager, sf::Vector2f origin) {
					if (path == "")
						path = id;
					else path =path+ '_' + id;

					//update inertial scroll stop
					scrollVelocity = sf::Vector2f();

					for (auto& elem : areaData) {
						if (elem.posRect.contains(windowManager.mousePos.back() - scroll - origin)) {
							elem.updatePress(path, windowManager, origin + elem.posRect.position + scroll);
							return;
						}
					}
					for (auto& elem : buttonData) {
						if (elem.posRect.contains(windowManager.mousePos.back() - scroll - origin)) {
							elem.updatePress(path, windowManager);
							return;
						}
					}
					for (auto& elem : inputData) {
						if (elem.posRect.contains(windowManager.mousePos.back() - scroll - origin)) {
							elem.updatePress(path, windowManager);
							return;
						}
					}
					//在当前的区域，但是不在控件上
					currentStatu = attr::gui::Statu::focus;
					windowManager.focus = { attr::gui::AreaPath, path};
					windowManager.overFocus = { attr::gui::AreaPath, path };
				}
				void updateOverMouseUnpressed(string& path, WindowManager& windowManager, sf::Vector2f origin) {
					if (path == "")
						path = id;
					else path = path + '_' + id;
					for (auto& elem : areaData) {
						if (elem.posRect.contains(windowManager.mousePos.back() - scroll - origin)) {
							elem.updateOverMouseUnpressed(path, windowManager, origin + elem.posRect.position + scroll);
							return;
						}
					}
					for (auto& elem : buttonData) {
						if (elem.posRect.contains(windowManager.mousePos.back() - scroll - origin)) {
							elem.updateOverMouseUnpressed(path, windowManager);
							return;
						}
					}
					for (auto& elem : inputData) {
						if (elem.posRect.contains(windowManager.mousePos.back() - scroll - origin)) {
							elem.updateOverMouseUnpressed(path, windowManager);
							return;
						}
					}
					//在当前的区域，但是不在控件上
					if (currentStatu != attr::gui::Statu::focus) {
						currentStatu = attr::gui::Statu::over;
						windowManager.overFocus = { attr::gui::AreaPath, path };
					}
				}
				void updateOverMousePressed(string& path, WindowManager& windowManager, sf::Vector2f origin) {
					if (path == "")
						path = id;
					else path = path + '_' + id;
					for (auto& elem : areaData) {
						if (elem.posRect.contains(windowManager.mousePos.back() - scroll - origin)) {
							elem.updateOverMousePressed(path, windowManager, origin + elem.posRect.position + scroll);
							return;
						}
					}
					for (auto& elem : buttonData) {
						if (elem.posRect.contains(windowManager.mousePos.back() - scroll - origin)) {
							elem.updateOverMousePressed(path, windowManager);
							return;
						}
					}
					//在当前的区域，但是不在控件上
					if (windowManager.focus.count(attr::gui::ButtonId)) {
						windowManager.operator[](windowManager.focus[attr::gui::AreaPath].cast<string>())
							.button(windowManager.focus[attr::gui::ButtonId].cast<string>()).currentStatu = attr::gui::Statu::over;
					}

				}
				void updateRelease(string& path, WindowManager& windowManager, sf::Vector2f origin) {
					if (path == "")
						path = id;
					else path = path + '_' + id;
					for (auto& elem : areaData) {
						if (elem.posRect.contains(windowManager.mousePos.back() - scroll - origin)) {
							elem.updateRelease(path, windowManager, origin + elem.posRect.position + scroll);
							return;
						}
					}
					for (auto& elem : buttonData) {
						if (elem.posRect.contains(windowManager.mousePos.back() - scroll - origin)) {
							elem.updateRelease(path, windowManager);
							return;
						}
					}
					//在当前的区域，但是不在控件上
					currentStatu = attr::gui::Statu::over;
					windowManager.overFocus = { attr::gui::AreaPath, path };
				}
				void updateScroll(WindowManager& windowManager) {
					if (scrollable!=sf::Vector2f()) {
						if (scrollVelocity.lengthSquared() < windowManager.scrollResistance* windowManager.scrollResistance) {
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
				void draw(sf::RenderTarget& r,sf::FloatRect displayArea, WindowManager& windowManager) {
					if (!posRect.findIntersection(displayArea))
						return;
					drawBase(r, displayArea);
					sf::FloatRect displayAreaCur(-scroll, posRect.size);
					sf::RenderTexture rCur(static_cast<sf::Vector2u>(posRect.size));
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
					s.setPosition(posRect.position- displayArea.position);
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
			bool newWindow(string id,AreaObj& w) {
				if (windowId.count(id)) {
					//窗口名称重复
					return false;
				}
				windowData.push_back(w);
				windowData[windowData.size() - 1].id = id;
				windowId[id] = (int)windowData.size()-1;
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
			AreaObj& operator[](string path) {
				AreaObj* res=nullptr;
				string temp = "";
				for (int i = 0; i < path.size(); i++) {
					if (path[i] == '_') {
						if (res==nullptr)
							res = &window(temp);
						else res = &(res->area(temp));
						temp = "";
					}
					else temp.push_back(path[i]);
				}
				if (res == nullptr)
					res = &window(temp);
				else res = &(res->area(temp));
				return *res;
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
				string path="";
				AreaObj& topWindow = windowData[windowData.size() - 1];
				if (sfEvent->is<sf::Event::MouseButtonPressed>()) {
					if (focus.count(attr::gui::ButtonId)) {
						operator[](focus[attr::gui::AreaPath].cast<string>())
							.button(focus[attr::gui::ButtonId].cast<string>()).currentStatu = attr::gui::Statu::normal;
						focus.erase(attr::gui::ButtonId);
					}
					if (focus.count(attr::gui::InputId)) {
						operator[](focus[attr::gui::AreaPath].cast<string>())
							.input(focus[attr::gui::InputId].cast<string>()).currentStatu = attr::gui::Statu::normal;
						focus.erase(attr::gui::InputId);
					}
					mousePos.back() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseButtonPressed>()->position);//update mousePos
					topWindow.updatePress(path, *this,topWindow.posRect.position);
					return true;
				}
				//sf::Event::MouseMoved
				if (sfEvent->is<sf::Event::MouseMoved>()) {
					//update scroll
					if (mousePressed) {
						AreaObj& tar = operator[](focus[attr::gui::AreaPath].cast<string>());
						if (tar.scrollable != sf::Vector2f()) {
							//It seems that do not cancel the focus when scrolling makes the GUI easier to use
							/*
							if (focus.count(attr::gui::ButtonId)) {
								operator[](focus[attr::gui::AreaPath].cast<string>())
									.button(focus[attr::gui::ButtonId].cast<string>()).currentStatu = attr::gui::Statu::over;
								focus.erase(attr::gui::ButtonId);
							}
							if (focus.count(attr::gui::InputId)) {
								operator[](focus[attr::gui::AreaPath].cast<string>())
									.input(focus[attr::gui::InputId].cast<string>()).currentStatu = attr::gui::Statu::over;
								focus.erase(attr::gui::InputId);
							}
							*/
							//tar.scroll += mouseVelocity();//wrong
							//consider the case of two continuous sf::Event::MouseMoved event
							tar.scroll += (sf::Vector2f(sfEvent->getIf<sf::Event::MouseMoved>()->position) - mousePos.back()).componentWiseMul(tar.scrollable);
						}
						mousePos.back() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseMoved>()->position);//update mousePos
						topWindow.updateOverMousePressed(path, *this, topWindow.posRect.position);
					}
					//update over
					//after updating over ,varible 'focus' will not be changed
					else {
						if (overFocus.count(attr::gui::ButtonId)) {
							operator[](overFocus[attr::gui::AreaPath].cast<string>())
								.button(overFocus[attr::gui::ButtonId].cast<string>()).currentStatu = attr::gui::Statu::normal;
						}
						else if (overFocus.count(attr::gui::InputId)) {
							operator[](overFocus[attr::gui::AreaPath].cast<string>())
								.input(overFocus[attr::gui::InputId].cast<string>()).currentStatu = attr::gui::Statu::normal;
						}
						else if (overFocus.count(attr::gui::AreaPath)) {
							operator[](overFocus[attr::gui::AreaPath].cast<string>()).currentStatu = attr::gui::Statu::normal;
						}
						mousePos.back() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseMoved>()->position);//update mousePos
						topWindow.updateOverMouseUnpressed(path, *this, topWindow.posRect.position);
					}
					return true;
				}
				//sf::Event::MouseButtonReleased
				//update release
				//after updating release ,varible 'focus' will not be changed
				if (sfEvent->is<sf::Event::MouseButtonReleased>()) {
					//re-update over before update release
					if (overFocus.count(attr::gui::ButtonId)) {
						operator[](overFocus[attr::gui::AreaPath].cast<string>())
							.button(overFocus[attr::gui::ButtonId].cast<string>()).currentStatu = attr::gui::Statu::normal;
					}
					else if (overFocus.count(attr::gui::AreaPath)) {
						operator[](overFocus[attr::gui::AreaPath].cast<string>()).currentStatu = attr::gui::Statu::normal;
					}

					mousePos.back() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseButtonReleased>()->position);//update mousePos
					topWindow.updateRelease(path, *this, topWindow.posRect.position);
					//update inertial scroll start
					if (focus.count(attr::gui::AreaPath)) {
						if (operator[](focus[attr::gui::AreaPath].cast<string>()).scrollable != sf::Vector2f())
							operator[](focus[attr::gui::AreaPath].cast<string>()).scrollVelocity = mouseVelocity().componentWiseMul(operator[](focus[attr::gui::AreaPath].cast<string>()).scrollable);
					}
					return true;
				}
				if (sfEvent->is<sf::Event::TextEntered>()) {
					if (focus.count(attr::gui::InputId)) {
						char32_t ch = sfEvent->getIf<sf::Event::TextEntered>()->unicode;
						InputObj& InputTar = operator[](focus[attr::gui::AreaPath].cast<string>()).input(focus[attr::gui::InputId].cast<string>());
						sf::String& text = InputTar.text;
						//backspace key
						if (ch == 8) {
							if (text.getSize() > 0 && InputTar.cursor > 0) {
								text.erase(InputTar.cursor - 1);
								InputTar.cursor--;
							}
						}
						//ctrl+v
						else if (ch == 22) {
							sf::String clipboardTemp = sf::Clipboard::getString();
							if (InputTar.oneLineLimit) {
								clipboardTemp.replace("\n", "");
							}
							clipboardTemp.replace("\r", "");
							if (InputTar.sizeLimit - text.getSize() > 0) {
								size_t remainSize = InputTar.sizeLimit - text.getSize();
								text.insert(InputTar.cursor, clipboardTemp.substring(0, remainSize));
								InputTar.cursor += min(remainSize, clipboardTemp.getSize());
							}
						}
						//enter key
						else if (ch == 13) {
							if (!InputTar.oneLineLimit) {
								text.insert(InputTar.cursor, '\n');
								InputTar.cursor++;
							}
						}
						//other keys
						else {
							if (text.getSize() < InputTar.sizeLimit) {
								text.insert(InputTar.cursor, ch);
								InputTar.cursor++;
							}
						}
						cursorBlinkTick = 0;
					}
					return true;
				}
				if (sfEvent->is<sf::Event::KeyPressed>()) {
					if (focus.count(attr::gui::InputId)) {
						InputObj& InputTar = operator[](focus[attr::gui::AreaPath].cast<string>()).input(focus[attr::gui::InputId].cast<string>());
						//left arrow
						if (sfEvent->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Left) {
							if (InputTar.cursor > 0) {
								InputTar.cursor--;
							}
						}
						//right arrow
						if (sfEvent->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Right) {
							sf::String& text = InputTar.text;
							if (InputTar.cursor < text.getSize()) {
								InputTar.cursor++;
							}
						}
						//delete key
						if (sfEvent->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Delete) {
							sf::String& text = InputTar.text;
							if (text.getSize() > 0 && InputTar.cursor < text.getSize()) {
								text.erase(InputTar.cursor);
							}
						}
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
					elem.draw(drawTarget, sf::FloatRect(sf::Vector2f(), static_cast<sf::Vector2f>(drawTarget.getSize())),*this);
				}
			}
		};
	}
}
unordered_map<string, game::gui::WindowManager::AreaObj>window;
unordered_map<string, game::gui::WindowManager::Style>style;

int windowWidth = 800, windowHeight = 600;
static void init() {
	game::fontManager.loadFont("ht", "FZHTJW.TTF");

	style["stda1"].set(sf::Color::White, sf::Color(200, 200, 200), 2, Skip, Skip, Skip, Skip, Skip);
	style["stda2"].set(sf::Color(240,240,240), sf::Color(200, 200, 200), 2, Skip, Skip, Skip, Skip, Skip);
	style["stdbn"].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2, sf::Color::Black, "ht", 50, Skip, Skip);
	style["stdbo"].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2, sf::Color::Black, "ht", 50, Skip, Skip);
	style["stdbf"].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2, sf::Color::Black, "ht", 50, Skip, Skip);

	window["main"]
		.setScrollable(false,false)
		.setStyle(style["stda1"], style["stda1"], style["stda1"])
		.setPosition(sf::Vector2f(0, 0))
		.setSize(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
	
	window["main"].button("button")
		.setText(L"按钮")
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(650, 500))
		.setSize(sf::Vector2f(100, 50));
	
	window["main"].text("text")
		.setText(L"这是外层窗口，被设置为不可拖动")
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(400, 25))
		.setCenter();

	window["main"].area("area").text("text2")
		//.setText(L"↓↓↓向下滑↓↓↓\n当按下按钮时拖动，\n操作将转化为拖动，\n且不会触发按钮\n(文本框同理)")
		.setText(L"↓↓↓向下滑↓↓↓\n当按下按钮时拖动，\n将保持按下按钮的状态拖动\n(文本框同理)")
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(350, 500))
		.setSize(sf::Vector2f(700, 260))
		.setCenter();

	window["main"].area("area")
		.setScrollable(false,true)
		.setStyle(style["stda2"], style["stda2"], style["stda2"])
		.setPosition(sf::Vector2f(50, 50))
		.setSize(sf::Vector2f(700, 450));
	
	window["main"].area("area").text("top")
		.setText(L"↑顶部，无法继续上拉")
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(350, 25))
		.setCenter();

	window["main"].area("area").text("text")
		.setText(L"这是内层窗口，被设置为可拖动\n现已支持惯性滑动")
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(350, 100))
		.setCenter();
	
	window["main"].area("area").input("input")
		.setText(L"这是一个文本框")
		.setJustification(attr::gui::Mid, attr::gui::Mid)
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(350, 225))
		.setSize(sf::Vector2f(400, 50));
	window["main"].area("area").input("input")
		.setSizeSyncText()
		.setCenter();

	window["main"].area("area").button("button")
		.setText(L"按钮").setJustification(attr::gui::Mid, attr::gui::Mid)
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(350, 275))
		.setSize(sf::Vector2f(100, 50))
		.setCenter();

	window["main"].area("area").text("bottom")
		.setText(L"↓底部")
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(350, 1000))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();

	window["main"].area("area")
		.setScrollLimitAuto();
}
game::gui::WindowManager windowManager;
game::Event evt;
int main() {
	init();
	windowManager.newWindow("main",window["main"]);
	game::window.create(sf::VideoMode(sf::Vector2u(windowWidth,windowHeight)), L"测试", sf::Style::Close, sf::State::Windowed);
	game::window.setFramerateLimit(60);
	while (true) {
		while (const optional sfEvt=game::window.pollEvent()) {
			if (sfEvt->is<sf::Event::Closed>()) {
				exit(0);
			}
			else {
				windowManager.update(sfEvt);
			}
		}
		while (windowManager.pollEvent(evt)) {
			if (evt.eventId == attr::gui::ButtonPressed) {
				cout << evt[attr::gui::ButtonPath].cast<string>()<<endl;
			}
		}
		game::window.clear();
		windowManager.draw(game::window);
		game::window.display();
	}
	return 0;
}