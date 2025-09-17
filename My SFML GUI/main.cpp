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
sf::Rect<T> operator+(sf::Rect<T> rect, sf::Vector2<T> vec) {
	rect.position += vec;
	return rect;
}
template <typename T>
sf::Rect<T> operator-(sf::Rect<T> rect, sf::Vector2<T> vec) {
	rect.position -= vec;
	return rect;
}
template<typename T, typename U>
inline BinaryFStream& operator>>(BinaryFStream& fin, unordered_map<T, U>& x) {
	size_t size;
	fin >> size;
	T t; U u;
	for (int i = 0; i < size; i++) {
		fin >> t >> u;
		x.emplace(t, u);
	}
	return fin;
}
template<typename T, typename U>
inline BinaryFStream& operator<<(BinaryFStream& fout, const unordered_map<T, U> x) {
	fout << x.size();
	for (auto& elem : x) {
		fout << elem.first << elem.second;
	}
	return fout;
}
inline BinaryFStream& operator>>(BinaryFStream& fin,sf::Color& x) {
	fin >> x.r >> x.g >> x.b >> x.a;
	return fin;
}
inline BinaryFStream& operator<<(BinaryFStream& fout, const sf::Color& x) {
	fout << x.r << x.g << x.b << x.a;
	return fout;
}
template<typename T>
inline BinaryFStream& operator>>(BinaryFStream& fin, sf::Vector2<T>& x) {
	fin >> x.x >> x.y;
	return fin;
}
template<typename T>
inline BinaryFStream& operator<<(BinaryFStream& fout, const sf::Vector2<T>& x) {
	fout << x.x << x.y;
	return fout;
}
template<typename T>
inline BinaryFStream& operator>>(BinaryFStream& fin, sf::Rect<T>& x) {
	fin >> x.position >> x.size;
	return fin;
}
template<typename T>
inline BinaryFStream& operator<<(BinaryFStream& fout, const sf::Rect<T>& x) {
	fout << x.position << x.size;
	return fout;
}
BinaryFStream& operator>>(BinaryFStream& fin, sf::String& x) {
	string s;
	fin >> s;
	x = sf::String::fromUtf8(s.begin(), s.end());
	return fin;
}
BinaryFStream& operator<<(BinaryFStream& fout, const sf::String& x) {
	for (uint8_t& elem : x.toUtf8())
		fout << elem;
	return fout;
}
BinaryFStream f("D:\\1.bin");
namespace game {
	namespace gui {
		class WindowManager:public EventManager {
		private:
			Statu focus,overFocus;
			bool mousePressed=false;
			unsigned int tick = 0;
			const unsigned int cursorSpeed = 60;
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
				friend BinaryFStream& operator>>(BinaryFStream& fin, Style& x) {
					return fin.structIn(x.backgroundColor, x.outlineColor, x.outlineThickness, x.textColor, x.font, x.characterSize, x.letterSpacing, x.lineSpacing);
				}
				friend BinaryFStream& operator<<(BinaryFStream& fout,const Style& x) {
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
				int currentStyle=attr::gui::normalStyle;
				void drawBase(sf::RenderTarget& r, sf::FloatRect displayArea) {
					if (!posRect.findIntersection(displayArea))return;
					Display::Draw::Rect(
						r,
						posRect.position - displayArea.position,
						posRect.position - displayArea.position + posRect.size,
						styles[currentStyle].backgroundColor,
						styles[currentStyle].outlineColor,
						styles[currentStyle].outlineThickness
					);
				}
			public:
				ObjBase& setPosition(sf::Vector2f _pos) {
					posRect.position = _pos;
					return *this;
				}
				ObjBase& setCenter() {
					posRect.position -= posRect.size / 2.f;
					return *this;
				}
				ObjBase& setSize(sf::Vector2f _size) {
					posRect.size = _size;
					return *this;
				}
				ObjBase& setStyle(Skipable<Style>_normalStyle, Skipable<Style>_overStyle, Skipable<Style>_focusStyle) {
					_normalStyle.assignTo(styles[0]);
					_overStyle.assignTo(styles[1]);
					_focusStyle.assignTo(styles[2]);
					return *this;
				}
			};
		public:
			class TextObj:public ObjBase{
				friend class WindowManager;
				sf::String text="";
			protected:
				sf::Text textObj{ fontManager[styles[attr::gui::normalStyle].font] };
				sf::FloatRect textRect;
				sf::Vector2i justification = { attr::gui::Mid,attr::gui::Mid };
				float yFix = 0.f;
				void draw(sf::RenderTarget& r, sf::FloatRect displayArea) {
					drawBase(r, displayArea);

					textObj.setString("_");
					textObj.setFont(game::fontManager[styles[currentStyle].font]);
					textObj.setCharacterSize(styles[currentStyle].characterSize);
					textObj.setLineSpacing(styles[currentStyle].lineSpacing);
					textObj.setLetterSpacing(styles[currentStyle].letterSpacing);
					textObj.setFillColor(styles[currentStyle].textColor);
					textObj.setPosition({ 0,0 });
					yFix = textObj.getGlobalBounds().position.y+ textObj.getGlobalBounds().size.y- textObj.getGlobalBounds().position.y;

					textObj.setString(text);
					textRect = textObj.getGlobalBounds();
					textRect.position.y = yFix;
					textRect.size.y = textObj.findCharacterPos(textObj.getString().getSize()).y + styles[currentStyle].characterSize;
					textObj.setPosition(posRect.position - textRect.position + ((posRect.size - textRect.size) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(justification)) - displayArea.position);
					textRect.position = posRect.position + ((posRect.size - textRect.size) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(justification));
					if (!textRect.findIntersection(displayArea))return;

					Display::Draw::Rect(
						r,
						textRect.position - displayArea.position,
						textRect.position - displayArea.position + textRect.size,
						styles[currentStyle].backgroundColor
					);
					r.draw(textObj);
				}
				/*
				friend BinaryFStream& operator>>(BinaryFStream& fin, TextObj& x) {
					fin >> x.text;
					return fin.structIn(x.id,x.posRect,x.styles, x.text,x.justification);
				}
				friend BinaryFStream& operator<<(BinaryFStream& fout, const TextObj& x) {
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
			private:
				void updatePress(const optional<sf::Event>& sfEvent, string& path, WindowManager& windowManager) {
					//按下按钮控件
					currentStyle = attr::gui::Style::focusStyle;
					windowManager.focus = { attr::gui::AreaPath, path,
											attr::gui::ButtonId, id,
											attr::gui::ScrollMousePos, sf::Vector2f(sfEvent->getIf<sf::Event::MouseButtonPressed>()->position) };
					windowManager.overFocus = { attr::gui::AreaPath, path,
												attr::gui::ButtonId, id };
				}
				void updateOver(const optional<sf::Event>& sfEvent, string& path, WindowManager& windowManager) {
					//移动到按钮控件
					if (currentStyle != attr::gui::Style::focusStyle) {
						currentStyle = attr::gui::Style::overStyle;
						windowManager.overFocus = { attr::gui::AreaPath, path,
													attr::gui::ButtonId, id };
					}
				}
				void updateRelease(const optional<sf::Event>& sfEvent, string& path, WindowManager& windowManager) {
					//释放按钮控件
					currentStyle = attr::gui::Style::overStyle;
					windowManager.overFocus = { attr::gui::AreaPath, path,
												attr::gui::ButtonId, id };
					if (windowManager.focus.count(attr::gui::ButtonId) && windowManager.focus[attr::gui::AreaPath].cast<string>() == path&& windowManager.focus[attr::gui::ButtonId].cast<string>()==id)
						windowManager.eventList.push(game::Event(attr::gui::ButtonPressed, { attr::gui::ButtonPath,path + '_' + id }));
				}
			};
			class InputObj :public TextObj{
				friend class WindowManager;
			protected:
				bool oneLineLimit = false;
				size_t sizeLimit=0;
				size_t cursor = 0;
				void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager) {
					TextObj::draw(r, displayArea);
					sf::Vector2f cursorPos = textObj.findCharacterPos(cursor);
					cursorPos.y += yFix;
					if (currentStyle == attr::gui::Style::focusStyle && (windowManager.tick >= 0 && windowManager.tick < windowManager.cursorSpeed/2)) {
						Display::Draw::Line(
							r,
							cursorPos,
							cursorPos + sf::Vector2f(0, (float)styles[currentStyle].characterSize),
							styles[currentStyle].textColor,
							2.0f
						);
					}
				}
			private:
				void updatePress(const optional<sf::Event>& sfEvent, string& path, WindowManager& windowManager) {
					//按下输入框控件
					currentStyle = attr::gui::Style::focusStyle;
					windowManager.focus = { attr::gui::AreaPath, path,
											attr::gui::InputId, id,
											attr::gui::ScrollMousePos, sf::Vector2f(sfEvent->getIf<sf::Event::MouseButtonPressed>()->position) };
					windowManager.overFocus = { attr::gui::AreaPath, path,
												attr::gui::InputId, id };
				}
				void updateOver(const optional<sf::Event>& sfEvent, string& path, WindowManager& windowManager) {
					//移动到输入框控件
					if (currentStyle != attr::gui::Style::focusStyle) {
						currentStyle = attr::gui::Style::overStyle;
						windowManager.overFocus = { attr::gui::AreaPath, path,
													attr::gui::InputId, id };
					}
				}
			public:
				InputObj& setLimit(bool _oneLineLimit = false, int _sizeLimit = 0) {
					oneLineLimit = _oneLineLimit;
					sizeLimit = _sizeLimit;
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
				bool scrollable=true;
				unordered_map<string, int>areaId, textId, buttonId, inputId;
				vector<AreaObj>areaData;
				vector<TextObj>textData;
				vector<ButtonObj>buttonData;
				vector<InputObj>inputData;
			public:
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
				AreaObj& setScrollable(bool _scrollable) {
					scrollable = _scrollable;
					return *this;
				}
			private:
				void updatePress(const optional<sf::Event>& sfEvent, string& path, WindowManager& windowManager, sf::Vector2f origin = { 0,0 }) {
					if (path == "")
						path = id;
					else path =path+ '_' + id;
					sf::Vector2f mousePos = sf::Vector2f(sfEvent->getIf<sf::Event::MouseButtonPressed>()->position);
					for (auto& elem : areaData) {
						if (elem.posRect.contains(mousePos - scroll - origin)) {
							elem.updatePress(sfEvent, path, windowManager, origin + elem.posRect.position + scroll);
							return;
						}
					}
					for (auto& elem : buttonData) {
						if (elem.posRect.contains(mousePos - scroll - origin)) {
							elem.updatePress(sfEvent, path, windowManager);
							return;
						}
					}
					for (auto& elem : inputData) {
						if (elem.posRect.contains(mousePos - scroll - origin)) {
							elem.updatePress(sfEvent, path, windowManager);
							return;
						}
					}
					//在当前的区域，但是不在控件上
					currentStyle = attr::gui::Style::focusStyle;
					windowManager.focus = { attr::gui::AreaPath, path,
											attr::gui::ScrollMousePos, mousePos };
					windowManager.overFocus = { attr::gui::AreaPath, path };
				}
				void updateOver(const optional<sf::Event>& sfEvent, string& path, WindowManager& windowManager, sf::Vector2f origin = { 0,0 }) {
					if (path == "")
						path = id;
					else path = path + '_' + id;
					sf::Vector2f mousePos = sf::Vector2f(sfEvent->getIf<sf::Event::MouseMoved>()->position);
					for (auto& elem : areaData) {
						if (elem.posRect.contains(mousePos - scroll - origin)) {
							elem.updateOver(sfEvent, path, windowManager, origin + elem.posRect.position + scroll);
							return;
						}
					}
					for (auto& elem : buttonData) {
						if (elem.posRect.contains(mousePos - scroll - origin)) {
							elem.updateOver(sfEvent, path, windowManager);
							return;
						}
					}
					for (auto& elem : inputData) {
						if (elem.posRect.contains(mousePos - scroll - origin)) {
							elem.updateOver(sfEvent, path, windowManager);
							return;
						}
					}
					//在当前的区域，但是不在控件上
					if (currentStyle != attr::gui::Style::focusStyle) {
						currentStyle = attr::gui::Style::overStyle;
						windowManager.overFocus = { attr::gui::AreaPath, path };
					}
				}
				void updateRelease(const optional<sf::Event>& sfEvent, string& path, WindowManager& windowManager, sf::Vector2f origin = { 0,0 }) {
					if (path == "")
						path = id;
					else path = path + '_' + id;
					sf::Vector2f mousePos = sf::Vector2f(sfEvent->getIf<sf::Event::MouseButtonReleased>()->position);
					for (auto& elem : areaData) {
						if (elem.posRect.contains(mousePos - scroll - origin)) {
							elem.updateRelease(sfEvent, path, windowManager, origin + elem.posRect.position + scroll);
							return;
						}
					}
					for (auto& elem : buttonData) {
						if (elem.posRect.contains(mousePos - scroll - origin)) {
							elem.updateRelease(sfEvent, path, windowManager);
							return;
						}
					}
					//在当前的区域，但是不在控件上
					currentStyle = attr::gui::Style::overStyle;
					windowManager.overFocus = { attr::gui::AreaPath, path };
				}
				void draw(sf::RenderTarget& r,sf::FloatRect displayArea, WindowManager& windowManager) {
					if (!posRect.findIntersection(displayArea))return;
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
			bool closeTopWindow() {
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
				if (!focus.count(attr::gui::AreaPath) && focus.count(attr::gui::InputId)) {
					cout << "wtf\n";
				}
				if (windowData.size() == 0)
					return false;
				//updateMousePressedStatu
				if (sfEvent->is<sf::Event::MouseButtonPressed>())
					mousePressed = true;
				if (sfEvent->is<sf::Event::MouseButtonReleased>())
					mousePressed = false;
				//sf::Event::MouseButtonPressed
				//updateFocus
				string path="";
				if (sfEvent->is<sf::Event::MouseButtonPressed>()) {
					if (focus.count(attr::gui::ButtonId)) {
						operator[](focus[attr::gui::AreaPath].cast<string>())
							.button(focus[attr::gui::ButtonId].cast<string>()).currentStyle = attr::gui::normalStyle;
						focus.erase(attr::gui::ButtonId);
					}
					if (focus.count(attr::gui::InputId)) {
						operator[](focus[attr::gui::AreaPath].cast<string>())
							.input(focus[attr::gui::InputId].cast<string>()).currentStyle = attr::gui::normalStyle;
						focus.erase(attr::gui::InputId);
					}
					windowData[windowData.size() - 1].updatePress(sfEvent, path, *this);
					return true;
				}
				//sf::Event::MouseMoved
				if (sfEvent->is<sf::Event::MouseMoved>()) {
					//updateScroll
					if (mousePressed) {
						AreaObj& tar = operator[](focus[attr::gui::AreaPath].cast<string>());
						if (tar.scrollable) {
							if (focus.count(attr::gui::ButtonId)) {
								operator[](focus[attr::gui::AreaPath].cast<string>())
									.button(focus[attr::gui::ButtonId].cast<string>()).currentStyle = attr::gui::overStyle;
								focus.erase(attr::gui::ButtonId);
							}
							if (focus.count(attr::gui::InputId)) {
								operator[](focus[attr::gui::AreaPath].cast<string>())
									.input(focus[attr::gui::InputId].cast<string>()).currentStyle = attr::gui::overStyle;
								focus.erase(attr::gui::InputId);
							}
							tar.scroll += sf::Vector2f(sfEvent->getIf<sf::Event::MouseMoved>()->position) - focus[attr::gui::ScrollMousePos].cast<sf::Vector2f>();
							focus[attr::gui::ScrollMousePos].cast<sf::Vector2f>() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseMoved>()->position);
						}
					}
					//updateOver
					else {
						if (overFocus.count(attr::gui::ButtonId)) {
							operator[](overFocus[attr::gui::AreaPath].cast<string>())
								.button(overFocus[attr::gui::ButtonId].cast<string>()).currentStyle = attr::gui::normalStyle;
						}
						else if (overFocus.count(attr::gui::InputId)) {
							operator[](overFocus[attr::gui::AreaPath].cast<string>())
								.input(overFocus[attr::gui::InputId].cast<string>()).currentStyle = attr::gui::normalStyle;
						}
						else if (overFocus.count(attr::gui::AreaPath)) {
							operator[](overFocus[attr::gui::AreaPath].cast<string>()).currentStyle = attr::gui::normalStyle;
						}
						windowData[windowData.size() - 1].updateOver(sfEvent, path, *this);
					}
					return true;
				}
				//sf::Event::MouseButtonReleased
				if (sfEvent->is<sf::Event::MouseButtonReleased>()) {
					windowData[windowData.size() - 1].updateRelease(sfEvent, path, *this);
					return true;
				}
				if (sfEvent->is<sf::Event::TextEntered>()) {
					if (focus.count(attr::gui::InputId)) {
						char32_t ch = sfEvent->getIf<sf::Event::TextEntered>()->unicode;
						InputObj& InputTar = operator[](focus[attr::gui::AreaPath].cast<string>()).input(focus[attr::gui::InputId].cast<string>());
						sf::String& text = InputTar.text;
						if (ch == 8) {
							if (text.getSize() > 0 && InputTar.cursor > 0) {
								text.erase(InputTar.cursor - 1);
								InputTar.cursor--;
							}
						}
						else if (ch == 22) {
							sf::String clipboardTemp = sf::Clipboard::getString();
							if (InputTar.sizeLimit - text.getSize() > 0) {
								text.insert(InputTar.cursor, clipboardTemp.substring(0, InputTar.sizeLimit - text.getSize()));
								InputTar.cursor += min(InputTar.sizeLimit - text.getSize(), clipboardTemp.getSize());
							}
						}
						else if (ch == 13) {
							if (!InputTar.oneLineLimit) {
								text.insert(InputTar.cursor, '\n');
								InputTar.cursor++;
							}
						}
						else {
							if (text.getSize() < InputTar.sizeLimit||InputTar.sizeLimit<=0) {
								text.insert(InputTar.cursor, ch);
								InputTar.cursor++;
							}
						}
						tick = 0;
					}
					return true;
				}
				if (sfEvent->is<sf::Event::KeyPressed>()) {
					if (focus.count(attr::gui::InputId)) {
						InputObj& InputTar = operator[](focus[attr::gui::AreaPath].cast<string>()).input(focus[attr::gui::InputId].cast<string>());
						if (sfEvent->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Left) {
							if (InputTar.cursor > 0) {
								InputTar.cursor--;
							}
						}
						if (sfEvent->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Right) {
							sf::String& text = InputTar.text;
							if (InputTar.cursor < text.getSize()) {
								InputTar.cursor++;
							}
						}
						if (sfEvent->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Delete) {
							sf::String& text = InputTar.text;
							if (text.getSize() > 0 && InputTar.cursor < text.getSize()) {
								text.erase(InputTar.cursor);
							}
						}
						tick = 0;
					}
					return true;
				}
				return false;
			}
			void draw(sf::RenderTarget& drawTarget) {
				if (focus.count(attr::gui::InputId)) {
					tick++;
					tick %= cursorSpeed;
				}
				else tick = 0;
				for (auto& elem : windowData) {
					elem.draw(drawTarget, sf::FloatRect(sf::Vector2f(), static_cast<sf::Vector2f>(drawTarget.getSize())),*this);
				}
			}
		};
	}
}
unordered_map<string, game::gui::WindowManager::AreaObj>window;
unordered_map<string, game::gui::WindowManager::Style>style;

int windowWidth = 800, windowHeight = 600;
void init() {
	game::fontManager.loadFont("ht", "FZHTJW.TTF");

	style["stda1"].set(sf::Color::White, sf::Color(200, 200, 200), 2, Skip, Skip, Skip, Skip, Skip);
	style["stda2"].set(sf::Color(240,240,240), sf::Color(200, 200, 200), 2, Skip, Skip, Skip, Skip, Skip);
	style["stdbn"].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2, sf::Color::Black, "ht", 50, Skip, Skip);
	style["stdbo"].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2, sf::Color::Black, "ht", 50, Skip, Skip);
	style["stdbf"].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2, sf::Color::Black, "ht", 50, Skip, Skip);
	style["stdt"].set(Skip,Skip,Skip, sf::Color::Black, "ht", 50, Skip, Skip);
	
	window["main"].setStyle(style["stda1"], style["stda1"], style["stda1"]).setPosition(sf::Vector2f(0, 0)).setSize(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
	window["main"].button("button").setText(L"按钮").setStyle(style["stdbn"], style["stdbo"], style["stdbf"]).setPosition(sf::Vector2f(650,500)).setSize(sf::Vector2f(100,50));
	window["main"].text("text").setText(L"这是外层窗口，可拖动").setStyle(style["stdt"], style["stdt"], style["stdt"]).setPosition(sf::Vector2f(400, 25)).setCenter();
	window["main"].area("area").text("text").setText(L"这是内层窗口，可拖动").setStyle(style["stdt"], style["stdt"], style["stdt"]).setPosition(sf::Vector2f(350, 25)).setCenter();
	window["main"].area("area").setStyle(style["stda2"], style["stda2"], style["stda2"]).setPosition(sf::Vector2f(50, 50)).setSize(sf::Vector2f(700, 450));
	window["main"].text("text2").setText(L"当按下按钮时拖动，\n操作将转化为拖动，\n且不会触发按钮\n(文本框同理)").setStyle(style["stdt"], style["stdt"], style["stdt"]).setPosition(sf::Vector2f(400, 700)).setCenter();
	window["main"].area("area").input("input").setText(L"这是一个文本框").setJustification(attr::gui::Left,attr::gui::Top)
				  .setStyle(style["stdbn"], style["stdbo"], style["stdbf"]).setPosition(sf::Vector2f(350, 225)).setSize(sf::Vector2f(400, 50)).setCenter();
}
game::gui::WindowManager windowManager;
game::Event evt;
int main() {
	init();
	windowManager.newWindow("main",window["main"]);
	game::window.create(sf::VideoMode(sf::Vector2u(windowWidth,windowHeight)), L"", sf::Style::Close, sf::State::Windowed);
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