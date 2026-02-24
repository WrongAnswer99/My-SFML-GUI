#pragma once
#include <set>
#include "Attribute.hpp"
#include "Event.hpp"
#include "Skippable.hpp"
#include "BinaryFileStreamSFML.hpp"
#include "SFMLBase.hpp"
#include "VarianTmap.hpp"
#include "RollArray.hpp"
//if exist then check condition
#define ensure(existCondition,condition) (!(existCondition)||((existCondition)&&(condition)))
namespace gui {
	class WindowManager;
	class Style {
	public:
		sf::Color backgroundColor = sf::Color(0, 0, 0, 0), outlineColor = sf::Color(0, 0, 0, 0);
		float outlineThickness = 1;
		void set(Skippable<sf::Color> _backgroundColor, Skippable<sf::Color>_outlineColor, Skippable<float>_outlineThickness) {
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
	class UIBase {
		friend class AreaObject;
		friend class WindowManager;
	protected:
		sf::FloatRect posRect = sf::FloatRect(sf::Vector2f(), sf::Vector2f(1.f, 1.f));
		Style styles[3];
		int currentStatu = attr::gui::Statu::normal;
		bool isShow = true;
		virtual void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager);
		//std::set<std::string> linkList;
	public:
		UIBase& setPosition(sf::Vector2f _pos) {
			posRect.position = _pos;
			return *this;
		}
		//use this setter
		//after : setPosition() , setSize()
		//* this setter is always the last one
		UIBase& setCenter() {
			posRect.position -= posRect.size / 2.f;
			return *this;
		}
		UIBase& setSize(sf::Vector2f _size) {
			posRect.size = _size;
			return *this;
		}
		sf::Vector2f getPosition() const {
			return posRect.position;
		}
		sf::Vector2f getSize() const {
			return posRect.size;
		}
		sf::FloatRect getPosRect() const {
			return posRect;
		}
		UIBase& setStyle(Skippable<Style>_normalStyle, Skippable<Style>_overStyle, Skippable<Style>_focusStyle) {
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
		bool getShow() const {
			return isShow;
		}
		UIBase& setShow(bool _isShow){
			isShow = _isShow;
			return *this;
		}
		UIBase& toggleShow() {
			isShow = !isShow;
			return *this;
		}
	protected:
		void setStatu(int statu, bool force = false) {
			if (force || currentStatu != attr::gui::Statu::focus) {
				currentStatu = statu;
			}
		}
	public:
		friend inline BinaryFStream& operator>>(BinaryFStream& bf, UIBase& x) {
			return bf.structIn(x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus], x.isShow);
		}
		friend inline BinaryFStream& operator<<(BinaryFStream& bf, const UIBase& x) {
			return bf.structOut(x.posRect, x.styles[attr::gui::Statu::normal], x.styles[attr::gui::Statu::over], x.styles[attr::gui::Statu::focus], x.isShow);
		}
	};
	class ImageObject :public UIBase {
		friend class WindowManager;
	public:
		ImageObject() {}
		//use this setter
		//before : setCenter()
		//after : setImage() , setScale() , setScaleTo()
		ImageObject& setSizeAuto() {
			posRect.size = static_cast<sf::Vector2f>(imageManager[imageId].getSize()).componentWiseMul(scale);
			return *this;
		}
	protected:
		std::string imageId;
		sf::Vector2i justification = { attr::gui::Mid,attr::gui::Mid };
		sf::Vector2f scale = sf::Vector2f(1, 1);
		sf::Color imageColors[3] = { sf::Color::White,sf::Color::White ,sf::Color::White };
		ImageObject& setImageColor(Skippable<sf::Color>_normalColor, Skippable<sf::Color>_overColor, Skippable<sf::Color>_focusColor) {
			_normalColor.assignTo(imageColors[attr::gui::Statu::normal]);
			_overColor.assignTo(imageColors[attr::gui::Statu::over]);
			_focusColor.assignTo(imageColors[attr::gui::Statu::focus]);
			return *this;
		}
		sf::Color& imageColor(int id) {
			return imageColors[id];
		}
		void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager);
	public:
		ImageObject& setJustification(Skippable<attr::gui::TextJustification> xJus, Skippable<attr::gui::TextJustification> yJus) {
			xJus.assignTo(justification.x);
			yJus.assignTo(justification.y);
			return *this;
		}
		ImageObject& setScale(sf::Vector2f _scale) {
			scale = _scale;
			return *this;
		}
		//use this setter
		//after : setImage()
		ImageObject& setScaleTo(sf::Vector2f _size) {
			scale = _size.componentWiseDiv(static_cast<sf::Vector2f>(imageManager[imageId].getSize()));
			return *this;
		}
		ImageObject& setImageId(const std::string& _imageId) {
			imageId = _imageId;
			return *this;
		}
		friend inline BinaryFStream& operator>>(BinaryFStream& bf, ImageObject& x) {
			bf >> static_cast<UIBase&>(x);
			bf.structIn(x.imageId, x.scale, x.justification, x.imageColors[attr::gui::Statu::normal], x.imageColors[attr::gui::Statu::over], x.imageColors[attr::gui::Statu::focus]);
			return bf;
		}
		friend inline BinaryFStream& operator<<(BinaryFStream& bf, const ImageObject& x) {
			bf << static_cast<const UIBase&>(x);
			bf.structOut(x.imageId, x.scale, x.justification, x.imageColors[attr::gui::Statu::normal], x.imageColors[attr::gui::Statu::over], x.imageColors[attr::gui::Statu::focus]);
			return bf;
		}
	};
	class TextObject :public UIBase {
		friend class WindowManager;
	public:
		TextObject() {
			textStyles[attr::gui::Statu::normal].set(sf::Color::Black, sf::Color::Black, Skip);
			textStyles[attr::gui::Statu::over].set(sf::Color::Black, sf::Color::Black, Skip);
			textStyles[attr::gui::Statu::focus].set(sf::Color::Black, sf::Color::Black, Skip);
		}
		TextObject& setTextStyle(Skippable<Style>_normalStyle, Skippable<Style>_overStyle, Skippable<Style>_focusStyle) {
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
		TextObject& setSizeAuto() {
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
		std::string font = "";
		unsigned int characterSize = 30;
		float letterSpacing = 1, lineSpacing = 1;
		sf::String text = "";
		sf::Text textRender{ fontManager[font] };
		sf::FloatRect textRect;
		sf::Vector2i justification = { attr::gui::Mid,attr::gui::Mid };
		Style textStyles[3];
		void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager);
	public:
		TextObject& setFont(const std::string& _font) {
			font = _font;
			textRender.setFont(fontManager[font]);
			return *this;
		}
		TextObject& setCharacterSize(int _characterSize) {
			characterSize = _characterSize;
			return *this;
		}
		TextObject& setSpacing(Skippable<int> _letterSpacing, Skippable<int> _lineSpacing) {
			_letterSpacing.assignTo(letterSpacing);
			_lineSpacing.assignTo(lineSpacing);
			return *this;
		}
		TextObject& setJustification(Skippable<attr::gui::TextJustification> xJus, Skippable<attr::gui::TextJustification> yJus) {
			xJus.assignTo(justification.x);
			yJus.assignTo(justification.y);
			return *this;
		}
		TextObject& setText(sf::String _text) {
			text = _text;
			return *this;
		}
		sf::String& getText() {
			return text;
		}
		friend inline BinaryFStream& operator>>(BinaryFStream& bf, TextObject& x) {
			bf >> static_cast<UIBase&>(x);
			bf.structIn(x.textStyles[attr::gui::Statu::normal], x.textStyles[attr::gui::Statu::over], x.textStyles[attr::gui::Statu::focus],
				x.font, x.characterSize, x.justification, x.letterSpacing, x.lineSpacing, x.text);
			x.textRender.setFont(fontManager[x.font]);
			return bf;
		}
		friend inline BinaryFStream& operator<<(BinaryFStream& bf, const TextObject& x) {
			bf << static_cast<const UIBase&>(x);
			bf.structOut(x.textStyles[attr::gui::Statu::normal], x.textStyles[attr::gui::Statu::over], x.textStyles[attr::gui::Statu::focus],
				x.font, x.characterSize, x.justification, x.letterSpacing, x.lineSpacing, x.text);
			return bf;
		}
	};
	class ButtonObject :public TextObject {
		friend class WindowManager;
	public:
		ButtonObject() {
			styles[attr::gui::Statu::normal].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2);
			styles[attr::gui::Statu::over].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2);
			styles[attr::gui::Statu::focus].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2);
		}
	};
	class OptionObject :public ButtonObject {
		friend class WindowManager;
	};
	class InputObject :public TextObject {
		friend class WindowManager;
	public:
		InputObject() {
			styles[attr::gui::Statu::normal].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2);
			styles[attr::gui::Statu::over].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2);
			styles[attr::gui::Statu::focus].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2);
		}
	protected:
		int typeLimit = attr::gui::InputTypeLimit::String;
		size_t sizeLimit = INT_MAX;
		class InputLimit {
			bool isAllowList = false;
			std::vector<char32_t>single;
			std::vector<std::pair<char32_t, char32_t>>range;
		public:
			void set(bool _isAllowList, std::initializer_list<char32_t> _single, std::initializer_list<std::pair<char32_t, char32_t>> _range) {
				isAllowList = _isAllowList;
				single = _single;
				range = _range;
				for (auto& elem : range)
					if (elem.first > elem.second)
						std::swap(elem.first, elem.second);
			}
			bool isLegal(char32_t ch) {
				for (auto& elem : single)
					if (ch == elem)return isAllowList;
				for (auto& elem : range)
					if (ch >= elem.first && ch <= elem.second)return isAllowList;
				return !isAllowList;
			}
			size_t legalize(sf::String& st) {
				size_t pos = 0;
				for (size_t i = 0; i < st.getSize(); i++) {
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
		void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager);
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
					(ch >= '0' && ch <= '9' && ensure(text.getSize() > 0, ensure(text[0] == '-', cursor != 0))) ||
					(ch == '.' && ensure(text.getSize() > 0, ensure(text[0] == '-', cursor != 0)) && text.find('.') == sf::String::InvalidPos)) {
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
					text.insert(cursor, st.substring(0, std::min(remainSize, realSize)));
					cursor += std::min(remainSize, realSize);
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
		InputObject& setSizeLimit(int _sizeLimit) {
			sizeLimit = _sizeLimit;
			return *this;
		}
		InputObject& setTypeLimit(attr::gui::InputTypeLimit _typeLimit) {
			typeLimit = _typeLimit;
			return *this;
		}
		InputObject& setStringTypeLimit(bool _isAllowList, std::initializer_list<char32_t> _single, std::initializer_list<std::pair<char32_t, char32_t>> _range) {
			typeLimit = attr::gui::InputTypeLimit::String;
			inputLimit.set(_isAllowList, _single, _range);
			return *this;
		}
		InputObject& setText(sf::String _text) {
			text = _text;
			cursor = text.getSize();
			return *this;
		}
		friend inline BinaryFStream& operator>>(BinaryFStream& bf, InputObject& x) {
			bf >> static_cast<TextObject&>(x);
			bf.structIn(x.sizeLimit, x.typeLimit, x.inputLimit);
			x.cursor = x.text.getSize();
			return bf;
		}
		friend inline BinaryFStream& operator<<(BinaryFStream& bf, const InputObject& x) {
			bf << static_cast<const TextObject&>(x);
			bf.structOut(x.sizeLimit, x.typeLimit, x.inputLimit);
			return bf;
		}
	};
	class AreaObject :public UIBase {
		friend class WindowManager;
	protected:
		sf::Vector2f scroll;
		sf::Vector2i mouseDragScrollable, mouseWheelScrollable;
		sf::Vector2f scrollVelocity;
		sf::FloatRect scrollLimit;
		std::string option;
	public:
		AreaObject() {
			styles[attr::gui::Statu::normal].set(sf::Color::White, sf::Color(200, 200, 200), 2);
			styles[attr::gui::Statu::over].set(sf::Color::White, sf::Color(200, 200, 200), 2);
			styles[attr::gui::Statu::focus].set(sf::Color::White, sf::Color(200, 200, 200), 2);
		}
		VarianTmap<UIBase>sub;
		AreaObject& setScrollable(Skippable<sf::Vector2i> _mouseDragScrollable, Skippable<sf::Vector2i> _mouseWheelScrollable) {
			_mouseDragScrollable.assignTo(mouseDragScrollable);
			_mouseWheelScrollable.assignTo(mouseWheelScrollable);
			return *this;
		}
	protected:
		inline static void addPoint(sf::FloatRect& rect, sf::Vector2f point) {
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
		AreaObject& setScrollLimit(sf::FloatRect rect) {
			scrollLimit = sf::FloatRect(sf::Vector2f(), posRect.size);
			addPoint(scrollLimit, rect.position);
			addPoint(scrollLimit, rect.position + rect.size);
			scrollLimit.size -= posRect.size;
			return *this;
		}
		AreaObject& setScrollLimitAuto() {
			scrollLimit = sf::FloatRect(sf::Vector2f(), posRect.size);
			for (auto& elem : sub) {
				addPoint(scrollLimit, elem->posRect.position);
				addPoint(scrollLimit, elem->posRect.position + elem->posRect.size);
			}
			scrollLimit.size -= posRect.size;
			return *this;
		}
		friend inline BinaryFStream& operator>>(BinaryFStream& bf, AreaObject& x) {
			bf >> static_cast<UIBase&>(x);
			x.sub.read<AreaObject, ImageObject, TextObject, InputObject, ButtonObject, OptionObject>(bf);
			bf.structIn(x.mouseDragScrollable, x.mouseWheelScrollable, x.scrollLimit, x.option);
			return bf;
		}
		friend inline BinaryFStream& operator<<(BinaryFStream& bf, const AreaObject& x) {
			bf << static_cast<const UIBase&>(x);
			x.sub.write<AreaObject, ImageObject, TextObject, InputObject, ButtonObject, OptionObject>(bf);
			bf.structOut(x.mouseDragScrollable, x.mouseWheelScrollable, x.scrollLimit, x.option);
			return bf;
		}
		AreaObject& setOption(const std::string& key) {
			if (option!="") {
				if (auto ptr = sub.find_named<OptionObject>(option))
					ptr->setStatu(attr::gui::Statu::normal, true);
				else option = "";
			}
			option = key;
			if (auto ptr = sub.find_named<OptionObject>(key)) {
				ptr->setStatu(attr::gui::Statu::focus);
			}
			return *this;
		}
		AreaObject& setOptionNull() {
			if (option != "") {
				if (auto ptr = sub.find_named<OptionObject>(option))
					ptr->setStatu(attr::gui::Statu::normal, true);
				else option = "";
			}
			option = "";
			return *this;
		}
		std::string getOption() {
			return option;
		}
	protected:
		void updateOption() {
			if (option!="") {
				if (auto ptr = sub.find_named<OptionObject>(option))
					ptr->setStatu(attr::gui::Statu::focus);
				else option = "";
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
		void updateScroll(WindowManager& windowManager);
		void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager);
	public:
		template<typename T>
		T& path_get(const std::string& path) {
			AreaObject* areaPtr = this;
			std::string temp = "";
			for (int i = 0; i < path.size(); i++) {
				if (path[i] == '_' || path[i] == '.') {
					areaPtr = &(areaPtr->sub.get<AreaObject>(temp));
					temp = "";
				}
				else temp.push_back(path[i]);
			}
			return areaPtr->sub.get<T>(temp);
		}
		template<typename T>
		T& path_at(const std::string& path) {
			AreaObject* areaPtr = this;
			std::string temp = "";
			for (int i = 0; i < path.size(); i++) {
				if (path[i] == '_' || path[i] == '.') {
					areaPtr = &(areaPtr->sub.at<AreaObject>(temp));
					temp = "";
				}
				else temp.push_back(path[i]);
			}
			return areaPtr->sub.at<T>(temp);
		}
	};
	class WindowManager :public EventManager {
		friend class InputObject;
		friend class AreaObject;
	public:
		WindowManager() {}
		//noncopyable
		WindowManager(const WindowManager&) = delete;
		WindowManager& operator=(const WindowManager&) = delete;
	private:
		//设置

		//cursorBlinkRate : how many ticks the cursor blinks
		unsigned int cursorBlinkRate = 30;
		float scrollResistance = 3.f;
		//scroll sensitivity = 6 tick = 0.1 s (60 FPS)
		static constexpr int scrollSensitivity = 6;
		float mouseWheelScrollRate = 50.f;
		float scrollThreshold = 20;

		Statu focus, overFocus;
		bool isDragScrolling = false;
		bool mousePressed = false;
		unsigned int cursorBlinkTick = 0;
		RollArray<sf::Vector2f, scrollSensitivity>mousePos;
		sf::Vector2f mouseLastPressPos;
		sf::Vector2f mouseVelocity() {
			return mousePos.back() - mousePos.front();
		}
		VarianTmap<AreaObject> layer;
	public:
		void clear() {
			while (!eventList.empty())
				eventList.pop();
			focus = Statu();
			layer.clear();
		}
		void open(const std::string& id) {
			if (layer.find_named<AreaObject>(id)) {
				std::cerr << "[WindowManager::open] Window ID already exists 窗口ID重复\n  id: " << id << "\n";
				throw std::runtime_error("[WindowManager::open] Window ID already exists 窗口ID重复\n  id: " + id + "\n");
			}
			layer.emplace_named<AreaObject>(layer.end(),id);
		}
		void open(const std::string& id,const AreaObject& Window) {
			if (layer.find_named<AreaObject>(id)) {
				std::cerr << "[WindowManager::open] Window ID already exists 窗口ID重复\n  id: " << id << "\n";
				throw std::runtime_error("[WindowManager::open] Window ID already exists 窗口ID重复\n  id: " + id + "\n");
			}
			auto ptr=layer.push_back_named(id, Window);
			ptr->updateOption();
		}
		void open(const std::string& id, AreaObject&& Window) {
			if (layer.find_named<AreaObject>(id)) {
				std::cerr << "[WindowManager::open] Window ID already exists 窗口ID重复\n  id: " << id << "\n";
				throw std::runtime_error("[WindowManager::open] Window ID already exists 窗口ID重复\n  id: " + id + "\n");
			}
			auto ptr = layer.push_back_named(id, Window);
			ptr->updateOption();
		}
		size_t close(const std::string& until) {
			size_t count = 0;
			while (layer.find_key(std::prev(layer.end())) != until) {
				layer.erase(std::prev(layer.end()));
				count++;
			}
			return count;
		}
		AreaObject& window(const std::string& id) {
			if (auto ptr = layer.find_named<AreaObject>(id))
				return *ptr;
			else {
				std::cerr << "[WindowManager::window] Window ID not found 未找到指定窗口ID\n  id: " << id << "\n";
				throw std::runtime_error("[WindowManager::window] Window ID not found 未找到指定窗口ID\n  id: " + id + "\n");
			}
		}
		template<typename T>
		T& path_get(const std::string& path){
			AreaObject* areaPtr = nullptr;
			std::string temp = "";
			for (int i = 0; i < path.size(); i++) {
				if (path[i] == '_' || path[i] == '.') {
					if (areaPtr == nullptr)
						areaPtr = &window(temp);
					else areaPtr = &(areaPtr->sub.get<AreaObject>(temp));
					temp = "";
				}
				else temp.push_back(path[i]);
			}
			if constexpr (std::is_same_v<T, AreaObject>) {
				if (areaPtr == nullptr)
					return window(temp);
				else return areaPtr->sub.get<T>(temp);
			}
			else return areaPtr->sub.get<T>(temp);
		}
		template<typename T>
		T& path_at(const std::string& path) {
			AreaObject* areaPtr = nullptr;
			std::string temp = "";
			for (int i = 0; i < path.size(); i++) {
				if (path[i] == '_'|| path[i] == '.') {
					if (areaPtr == nullptr)
						areaPtr = &window(temp);
					else areaPtr = &(areaPtr->sub.at<AreaObject>(temp));
					temp = "";
				}
				else temp.push_back(path[i]);
			}
			if constexpr (std::is_same_v<T, AreaObject>) {
				if (areaPtr == nullptr)
					return window(temp);
				else return areaPtr->sub.get<T>(temp);
			}
			else return areaPtr->sub.get<T>(temp);
		}
	private:
		inline AreaObject* updateOver(bool stopScroll = false) {
			if (statuVisit(overFocus) != nullptr)
				statuVisit(overFocus)->setStatu(attr::gui::Statu::normal);
			AreaObject* areaPtr = *std::prev(layer.end());
			sf::Vector2f origin = areaPtr->posRect.position;
			overFocus = {};
			std::string path = layer.find_key(areaPtr);
			while (true) {
				if (stopScroll)
					areaPtr->scrollVelocity = sf::Vector2f();
				for (auto& elem : areaPtr->sub.riterate()) {
					if (elem->isShow && elem->posRect.contains(mousePos.back() - areaPtr->scroll - origin)) {
						if (auto ptr = areaPtr->sub.find<AreaObject>(elem)) {
							origin += elem->posRect.position + areaPtr->scroll;
							path = path + '_' + areaPtr->sub.find_key(elem);
							areaPtr = ptr;
							goto searchcontinue;
						}
						else {
							if (areaPtr == nullptr) {
								return nullptr;
							}
							overFocus[attr::gui::AreaPath] = path;
							if (areaPtr->sub.find<ButtonObject>(elem)) {
								overFocus[attr::gui::ButtonId] = areaPtr->sub.find_key(elem);
								return areaPtr;
							}
							if (areaPtr->sub.find<OptionObject>(elem)) {
								overFocus[attr::gui::OptionId] = areaPtr->sub.find_key(elem);
								return areaPtr;
							}
							if (areaPtr->sub.find<InputObject>(elem)) {
								overFocus[attr::gui::InputId] = areaPtr->sub.find_key(elem);
								return areaPtr;
							}
						}
					}
				}
				break;
			searchcontinue:;
			}
			overFocus[attr::gui::AreaPath] = path;
			return areaPtr;
		}
		inline UIBase* statuVisit(Statu& statu, AreaObject* areaPtr = nullptr) {
			if (areaPtr == nullptr) {
				std::string temp = "";
				if (!statu.count(attr::gui::AreaPath))
					return nullptr;
				std::string& path = statu[attr::gui::AreaPath].raw();
				for (int i = 0; i < path.size(); i++) {
					if (path[i] == '_' || path[i] == '.') {
						if (areaPtr == nullptr) {
							if (auto ptr = layer.find_named<AreaObject>(temp))
								areaPtr = ptr;
							else return nullptr;
						}
						else {
							if (auto ptr = areaPtr->sub.find_named<AreaObject>(temp))
								areaPtr = ptr;
							else return nullptr;
						}
						temp = "";
					}
					else temp.push_back(path[i]);
				}
				if (areaPtr == nullptr) {
					if (auto ptr = layer.find_named<AreaObject>(temp))
						areaPtr = ptr;
					else return nullptr;
				}
				else {
					if (auto ptr = areaPtr->sub.find_named<AreaObject>(temp))
						areaPtr = ptr;
					else return nullptr;
				}
			}
			if (statu.count(attr::gui::ButtonId)) {
				return areaPtr->sub.find_named<ButtonObject>(statu[attr::gui::ButtonId].cast<std::string>());
			}
			else if (statu.count(attr::gui::OptionId)) {
				return areaPtr->sub.find_named<OptionObject>(statu[attr::gui::OptionId].cast<std::string>());
			}
			else if (statu.count(attr::gui::InputId)) {
				return areaPtr->sub.find_named<InputObject>(statu[attr::gui::InputId].cast<std::string>());
			}
			else if (statu.count(attr::gui::AreaPath))
				return areaPtr;
			else return nullptr;
		}
		inline void updateSimpleMove(AreaObject* areaFocusPtr, AreaObject* areaOverPtr) {
			//update scroll
			if (areaFocusPtr != nullptr && mousePressed) {
				if (areaFocusPtr->mouseDragScrollable != sf::Vector2i()) {
					if (ensure(focus.count(attr::gui::ButtonId) || focus.count(attr::gui::OptionId), (mouseLastPressPos - mousePos.back()).lengthSquared() >= scrollThreshold * scrollThreshold))
						isDragScrolling = true;
					if ((focus.count(attr::gui::ButtonId) || focus.count(attr::gui::OptionId)) && isDragScrolling) {
						if (overFocus == focus)
							statuVisit(focus, areaFocusPtr)->setStatu(attr::gui::Statu::over, true);
						else statuVisit(focus, areaFocusPtr)->setStatu(attr::gui::Statu::normal, true);
						if (focus.count(attr::gui::OptionId))
							areaFocusPtr->updateOption();
					}
				}
				else {
					if (areaFocusPtr != nullptr && (focus.count(attr::gui::ButtonId) || focus.count(attr::gui::OptionId))) {
						if (overFocus == focus)
							statuVisit(focus, areaFocusPtr)->setStatu(attr::gui::Statu::focus);
						else statuVisit(focus, areaFocusPtr)->setStatu(attr::gui::Statu::over, true);
						if (focus.count(attr::gui::OptionId))
							areaFocusPtr->updateOption();
					}
				}
			}
			//update over
			//after updating over ,varible 'focus' will not be changed
			else {
				if (statuVisit(overFocus, areaOverPtr) != nullptr)
					statuVisit(overFocus, areaOverPtr)->setStatu(attr::gui::Statu::over);
			}
		}
	public:
		bool update(const std::optional<sf::Event>& sfEvent) {
			if (layer.size() == 0)
				return false;
			//update mouse press statu
			if (sfEvent->is<sf::Event::MouseButtonPressed>()) {
				mousePressed = true;
				mouseLastPressPos = static_cast<sf::Vector2f>(sfEvent->getIf<sf::Event::MouseButtonPressed>()->position);
			}
			if (sfEvent->is<sf::Event::MouseButtonReleased>())
				mousePressed = false;
			if (statuVisit(focus) == nullptr)
				focus = {};
			AreaObject* areaOverPtr = nullptr, * areaFocusPtr = nullptr;
			if (focus.count(attr::gui::AreaPath))
				areaFocusPtr = &path_at<AreaObject>(focus[attr::gui::AreaPath].cast<std::string>());
			//sf::Event::MouseButtonPressed
			//update focus
			//after updating focus ,varible 'focus' & 'overFocus' must have a value
			if (sfEvent->is<sf::Event::MouseButtonPressed>()) {
				mousePos.back() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseButtonPressed>()->position);//update mousePos

				//update inertial scroll stop
				areaOverPtr = updateOver(true);

				if (focus.count(attr::gui::InputId) && overFocus != focus)
					eventList.push(Event(attr::gui::InputLoseFocus, { attr::gui::InputPath,focus[attr::gui::AreaPath].cast<std::string>() + '_' + focus[attr::gui::InputId].cast<std::string>() }));
				if (overFocus.count(attr::gui::InputId) && overFocus != focus)
					eventList.push(Event(attr::gui::InputGainFocus, { attr::gui::InputPath,overFocus[attr::gui::AreaPath].cast<std::string>() + '_' + overFocus[attr::gui::InputId].cast<std::string>() }));

				if (statuVisit(focus, areaFocusPtr) != nullptr)
					statuVisit(focus, areaFocusPtr)->setStatu(attr::gui::Statu::normal, true);
				if (areaFocusPtr != nullptr)
					areaFocusPtr->updateOption();
				if (statuVisit(overFocus, areaOverPtr) != nullptr)
					statuVisit(overFocus, areaOverPtr)->setStatu(attr::gui::Statu::focus);

				focus = overFocus;
				return true;
			}
			//sf::Event::MouseMoved
			if (sfEvent->is<sf::Event::MouseMoved>()) {
				sf::Vector2f mousePosDelta = sf::Vector2f(sfEvent->getIf<sf::Event::MouseMoved>()->position) - mousePos.back();
				mousePos.back() = sf::Vector2f(sfEvent->getIf<sf::Event::MouseMoved>()->position);//update mousePos
				areaOverPtr = updateOver();
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
				areaOverPtr = updateOver();

				if (areaFocusPtr != nullptr && focus.count(attr::gui::ButtonId)) {
					if (overFocus == focus) {
						if (!isDragScrolling)
							eventList.push(Event(attr::gui::ButtonPressed, { attr::gui::ButtonPath,focus[attr::gui::AreaPath].cast<std::string>() + '_' + focus[attr::gui::ButtonId].cast<std::string>() }));
						areaFocusPtr->sub.at<ButtonObject>(focus[attr::gui::ButtonId].cast<std::string>()).setStatu(attr::gui::Statu::over, true);
					}
					else areaFocusPtr->sub.at<ButtonObject>(focus[attr::gui::ButtonId].cast<std::string>()).setStatu(attr::gui::Statu::normal, true);
				}
				if (areaFocusPtr != nullptr && focus.count(attr::gui::OptionId)) {
					if (areaFocusPtr->option!="")
						areaFocusPtr->sub.at<OptionObject>(areaFocusPtr->option).setStatu(attr::gui::Statu::normal, true);
					if (overFocus == focus) {
						if (!isDragScrolling) {
							areaFocusPtr->option = focus[attr::gui::OptionId].cast<std::string>();
							eventList.push(Event(attr::gui::OptionChosen, { attr::gui::OptionPath,focus[attr::gui::AreaPath].cast<std::string>() + '_' + focus[attr::gui::OptionId].cast<std::string>() }));
						}
					}
					else areaFocusPtr->sub.at<OptionObject>(focus[attr::gui::OptionId].cast<std::string>()).setStatu(attr::gui::Statu::normal, true);
					areaFocusPtr->updateOption();
				}
				isDragScrolling = false;

				//update inertial scroll start
				if (areaFocusPtr != nullptr && focus.count(attr::gui::AreaPath)) {
					if (areaFocusPtr->mouseDragScrollable != sf::Vector2i())
						areaFocusPtr->scrollVelocity = mouseVelocity()
						.componentWiseMul(static_cast<sf::Vector2f>(areaFocusPtr->mouseDragScrollable))
						.componentWiseDiv(sf::Vector2f(scrollSensitivity, scrollSensitivity));
				}
				return true;
			}
			if (sfEvent->is<sf::Event::TextEntered>()) {
				if (areaFocusPtr != nullptr && focus.count(attr::gui::InputId)) {
					char32_t ch = sfEvent->getIf<sf::Event::TextEntered>()->unicode;
					InputObject& InputTar = areaFocusPtr->sub.at<InputObject>(focus[attr::gui::InputId].cast<std::string>());
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
					InputObject& InputTar = areaFocusPtr->sub.at<InputObject>(focus[attr::gui::InputId].cast<std::string>());
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
				areaOverPtr = updateOver();
				float delta = sfEvent->getIf<sf::Event::MouseWheelScrolled>()->delta;
				if (areaOverPtr->mouseWheelScrollable == sf::Vector2i(1, 0))
					areaOverPtr->scrollVelocity = sf::Vector2f(delta * mouseWheelScrollRate, 0);
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
			if (layer.size() >= 1) {
				if (statuVisit(focus) == nullptr)
					focus = {};
				updateSimpleMove(focus.count(attr::gui::AreaPath) ? &path_at<AreaObject>(focus[attr::gui::AreaPath].cast<std::string>()) : nullptr, updateOver());
			}
			if (focus.count(attr::gui::InputId)) {
				cursorBlinkTick++;
				cursorBlinkTick %= cursorBlinkRate;
			}
			else cursorBlinkTick = 0;
			mousePos.copy_back();//update mousePos
			for (auto& elem : layer) {
				elem->updateScroll(*this);
				elem->draw(drawTarget, sf::FloatRect(sf::Vector2f(), static_cast<sf::Vector2f>(drawTarget.getSize())), *this);
			}
		}
	};
}
