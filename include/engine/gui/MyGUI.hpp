#pragma once
#include <set>
#include "engine/event/Event.hpp"
#include "engine/serialization/BinaryFileStream.hpp"
#include "engine/serialization/JsonExtensions.hpp"
#include "engine/resource/ResourceManager.hpp"
#include "engine/data/VarianTmap.hpp"
#include "engine/data/RollArray.hpp"
//if exist then check condition
#define ensure(existCondition,condition) (!(existCondition)||((existCondition)&&(condition)))
namespace gui {
	inline ImageManager UIimageManager;
	namespace Events{
		struct UIEventBase : public EventBase {
			std::string path;
			std::string name;
			const std::string wholePath() const {
				return path + '_' + name;
			}
		};
		struct ButtonPressed : public UIEventBase {};
		struct OptionDeselected : public UIEventBase {};
		struct OptionSelected : public UIEventBase {};
		struct InputSelected : public UIEventBase {};
		struct InputDeselected : public UIEventBase {};
	}
	//快速绘制简图
	namespace _builtinGUIdraw {
		inline void Rect(sf::RenderTarget& r, const sf::Vector2f& point1, const sf::Vector2f& point2, sf::Color fillcolor, sf::Color linecolor = sf::Color::Transparent, float thickness = 0) {
			sf::RectangleShape rect;
			rect.setOrigin((point2 - point1) / 2.0f);
			rect.setPosition((point1 + point2) / 2.f);
			rect.setSize(point2 - point1);
			rect.setFillColor(fillcolor);
			rect.setOutlineColor(linecolor);
			rect.setOutlineThickness(thickness);
			r.draw(rect);
		}
		inline void Line(sf::RenderTarget& r, const sf::Vector2f& point1, const sf::Vector2f& point2, sf::Color color, float thickness) {
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
	class WindowManager;
	class Style {
	public:
		sf::Color backgroundColor = sf::Color(0, 0, 0, 0), outlineColor = sf::Color(0, 0, 0, 0);
		float outlineThickness = 1;
		void set(sf::Color _backgroundColor, sf::Color _outlineColor, float _outlineThickness) {
			backgroundColor = _backgroundColor;
			outlineColor = _outlineColor;
			outlineThickness = _outlineThickness;
		}
		friend inline void read(BinaryFileStream& bf, Style& x) {
			bf.readStruct(x.backgroundColor, x.outlineColor, x.outlineThickness);
		}
		friend inline void write(BinaryFileStream& bf, const Style& x) {
			bf.writeStruct(x.backgroundColor, x.outlineColor, x.outlineThickness);
		}
		friend inline void to_json(nlohmann::json& j, const Style& x) {
			j = nlohmann::json{ {"backgroundColor", x.backgroundColor}, {"outlineColor", x.outlineColor}, {"outlineThickness", x.outlineThickness} };
		}
		friend inline void from_json(const nlohmann::json& j, Style& x) {
			j.at("backgroundColor").get_to(x.backgroundColor);
			j.at("outlineColor").get_to(x.outlineColor);
			j.at("outlineThickness").get_to(x.outlineThickness);
		}
	};
	class UIBase {
		friend class AreaObject;
		friend class WindowManager;
	public:
		enum class Anchor {
			Left = 0, Right = 2, Width = 3,
			Top = 0, Bottom = 2, Height = 3,
			Mid = 1, Size = 3
		};
		enum class Relative { LeftEdge = 0, RightEdge = 2, TopEdge = 0, BottomEdge = 2 ,MidLine = 1 };
		enum class Align { Left = 0, Right = 2, Top = 0, Bottom = 2, Mid = 1 };
		class DynamicPosition{
			friend class UIBase;
			float calcRelative(float fatherSize) const{
				return (type & 0b000011) * fatherSize / 2.0f + value;
			}
		public:
			bool isSize () const{
				return getAnchor() == static_cast<int>(Anchor::Size);
			}
			bool isNormal () const{
				return type == ((static_cast<int>(Anchor::Top) << 2) | static_cast<int>(Relative::TopEdge));
			}
			int getRelative () const{
				return type & 0b000011;
			}
			int getAnchor () const{
				return type >> 2;
			}
			int type = 0;
			float value = 0;
			DynamicPosition() = default;
			DynamicPosition(Anchor _anchor, float _value = 0){
				type = static_cast<int>(_anchor) << 2;
				value = _value;
			}
			DynamicPosition(Anchor _anchor, Relative _relative, float _value = 0){
				type = (static_cast<int>(_anchor) << 2) | static_cast<int>(_relative);
				value = _value;
			}
			friend inline void read(BinaryFileStream& bf, DynamicPosition& x) {
				bf.read(x.type, x.value);
			}
			friend inline void write(BinaryFileStream& bf, const DynamicPosition& x) {
				bf.write(x.type, x.value);
			}
			friend inline void to_json(nlohmann::json& j, const DynamicPosition& x) {
				j = nlohmann::json{ {"type", x.type}, {"value", x.value} };
			}
			friend inline void from_json(const nlohmann::json& j, DynamicPosition& x) {
				j.at("type").get_to(x.type);
				j.at("value").get_to(x.value);
			}
		};
	protected:
		sf::FloatRect posRect = sf::FloatRect(sf::Vector2f(), sf::Vector2f());
		sf::Vector2<std::pair<DynamicPosition, DynamicPosition>> relativePosition={
			{{UIBase::Anchor::Left, UIBase::Relative::LeftEdge, 0}, {UIBase::Anchor::Width, 0}},
			{{UIBase::Anchor::Top, UIBase::Relative::TopEdge, 0}, {UIBase::Anchor::Height, 0}}
		};
		Style styles[3];
		int currentStatu = gui::UIBase::Normal;
		bool isShow = true;
		virtual void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager);
		//std::set<std::string> linkList;
	public:
		enum Statu { Normal = 0, Over = 1, Focus = 2 };
		UIBase& setPosition(sf::Vector2f _position) {
			relativePosition.x.first = {UIBase::Anchor::Left, UIBase::Relative::LeftEdge, _position.x};
			relativePosition.y.first = {UIBase::Anchor::Top, UIBase::Relative::TopEdge, _position.y};
			return *this;
		}
		UIBase& setPosition(sf::Vector2f _position, sf::Vector2<Anchor> _anchor) {
			relativePosition.x.first = {_anchor.x, UIBase::Relative::LeftEdge, _position.x};
			relativePosition.y.first = {_anchor.y, UIBase::Relative::TopEdge, _position.y};
			return *this;
		}
		UIBase& setPosition(sf::Vector2f _position, sf::Vector2<Anchor> _anchor, sf::Vector2<Relative> _relative) {
			relativePosition.x.first = {_anchor.x, _relative.x, _position.x};
			relativePosition.y.first = {_anchor.y, _relative.y, _position.y};
			return *this;
		}
		UIBase& setSize(sf::Vector2f _size) {
			if (relativePosition.x.second.isSize()) relativePosition.x.second = {UIBase::Anchor::Size, _size.x};
			if (relativePosition.y.second.isSize()) relativePosition.y.second = {UIBase::Anchor::Size, _size.y};
			return *this;
		}
		UIBase& setPositionRelative(std::initializer_list<UIBase::DynamicPosition> xRelative, std::initializer_list<UIBase::DynamicPosition> yRelative) {
			if (xRelative.size() == 1){
				auto& first = *xRelative.begin();
				if (first.isSize()) goto setRelativeIllegal;
				relativePosition.x.first = first;
			}
			else{
				auto & first = *xRelative.begin(),&second = *std::next(xRelative.begin());
				if (first.getAnchor() == second.getAnchor()) goto setRelativeIllegal;
				if (first.getAnchor() < second.getAnchor()){
					relativePosition.x.first = first;relativePosition.x.second = second;
				}
				else{
					relativePosition.x.first = second;relativePosition.x.second = first;
				}
			}
			if (yRelative.size() == 1){
				auto& first = *yRelative.begin();
				if (first.isSize()) goto setRelativeIllegal;
				relativePosition.y.first = first;
			}
			else{
				auto & first = *yRelative.begin(),&second = *std::next(yRelative.begin());
				if (first.getAnchor() == second.getAnchor()) goto setRelativeIllegal;
				if (first.getAnchor() < second.getAnchor()){
					relativePosition.y.first = first;relativePosition.y.second = second;
				}
				else{
					relativePosition.y.first = second;relativePosition.y.second = first;
				}
			}
			return *this;
			setRelativeIllegal:;
			throw std::runtime_error("[UIBase::setRelative] Illegal relative position\n");
		}
		//static getter
		sf::Vector2<std::pair<DynamicPosition, DynamicPosition>> getDynamicPosition() const {
			return relativePosition;
		}
		sf::Vector2<std::optional<float>> getPosition() const {
			return {
				relativePosition.x.first.isNormal() ? std::optional<float>(relativePosition.x.first.value) : std::nullopt,
				relativePosition.y.first.isNormal() ? std::optional<float>(relativePosition.y.first.value) : std::nullopt
			};
		}
		sf::Vector2<std::optional<float>> getSize() const {
			return {
				relativePosition.x.second.isSize() ? std::optional<float>(relativePosition.x.second.value) : std::nullopt,
				relativePosition.y.second.isSize() ? std::optional<float>(relativePosition.y.second.value) : std::nullopt
			};
		}
		//dynamic getter
		sf::FloatRect getPosRect() const {
			return posRect;
		}
		UIBase& setStyle(const Style& _normalStyle, const Style& _overStyle, const Style& _focusStyle) {
			styles[gui::UIBase::Normal] = _normalStyle;
			styles[gui::UIBase::Over] = _overStyle;
			styles[gui::UIBase::Focus] = _focusStyle;
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
		void updatePosRect(sf::Vector2f fatherSize) {
			if (relativePosition.x.second.isSize())
				posRect.size.x = relativePosition.x.second.value;
			else posRect.size.x = (relativePosition.x.second.calcRelative(fatherSize.x)-relativePosition.x.first.calcRelative(fatherSize.x)) * 2 / (relativePosition.x.second.getAnchor() - relativePosition.x.first.getAnchor());
			if (relativePosition.y.second.isSize())
				posRect.size.y = relativePosition.y.second.value;
			else posRect.size.y = (relativePosition.y.second.calcRelative(fatherSize.y)-relativePosition.y.first.calcRelative(fatherSize.y)) * 2 / (relativePosition.y.second.getAnchor() - relativePosition.y.first.getAnchor());
			posRect.position.x = relativePosition.x.first.calcRelative(fatherSize.x) - (relativePosition.x.first.getAnchor()) / 2.f * posRect.size.x;
			posRect.position.y = relativePosition.y.first.calcRelative(fatherSize.y) - (relativePosition.y.first.getAnchor()) / 2.f * posRect.size.y;
		}
		void setStatu(int statu, bool force = false) {
			if (force || currentStatu != gui::UIBase::Focus) {
				currentStatu = statu;
			}
		}
	public:
		friend inline void read(BinaryFileStream& bf, UIBase& x) {
			bf.readStruct(x.relativePosition, x.styles, x.isShow);
		}
		friend inline void write(BinaryFileStream& bf, const UIBase& x) {
			bf.writeStruct(x.relativePosition, x.styles, x.isShow);
		}
		friend inline void to_json(nlohmann::json& j, const UIBase& x) {
			j = nlohmann::json{ {"position", x.relativePosition}, {"styles", x.styles}, {"isShow", x.isShow} };
		}
		friend inline void from_json(const nlohmann::json& j, UIBase& x) {
			j.at("position").get_to(x.relativePosition);
			j.at("styles").get_to(x.styles);
			j.at("isShow").get_to(x.isShow);
		}
	};
	class ImageObject :public UIBase {
		friend class WindowManager;
	public:
		ImageObject() {}
		//use this setter
		//after : setImage() , setScale() , setScaleTo()
		ImageObject& setSizeAuto() {
			setSize(static_cast<sf::Vector2f>(UIimageManager[imageId].getSize()).componentWiseMul(scale));
			return *this;
		}
	protected:
		std::string imageId;
		sf::Vector2i align = { static_cast<int>(gui::UIBase::Align::Mid), static_cast<int>(gui::UIBase::Align::Mid) };
		sf::Vector2f scale = sf::Vector2f(1, 1);
		sf::Color imageColors[3] = { sf::Color::White,sf::Color::White ,sf::Color::White };
		void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager);
	public:
		ImageObject& setImageColor(const sf::Color& _normalColor, const sf::Color& _overColor, const sf::Color& _focusColor) {
			imageColors[gui::UIBase::Normal] = _normalColor;
			imageColors[gui::UIBase::Over] = _overColor;
			imageColors[gui::UIBase::Focus] = _focusColor;
			return *this;
		}
		sf::Color& imageColor(int id) {
			return imageColors[id];
		}
		ImageObject& setAlign(gui::UIBase::Align xAlign, gui::UIBase::Align yAlign) {
			align.x = static_cast<int>(xAlign);
			align.y = static_cast<int>(yAlign);
			return *this;
		}
		ImageObject& setScale(sf::Vector2f _scale) {
			scale = _scale;
			return *this;
		}
		//use this setter
		//after : setImage()
		ImageObject& setScaleTo(sf::Vector2f _size) {
			scale = _size.componentWiseDiv(static_cast<sf::Vector2f>(UIimageManager[imageId].getSize()));
			return *this;
		}
		ImageObject& setImageId(const std::string& _imageId) {
			imageId = _imageId;
			return *this;
		}
		const std::string& getImageId() const { return imageId; }
		sf::Vector2f getScale() const { return scale; }
		sf::Vector2<Align> getAlign() const {
			return static_cast<sf::Vector2<Align>>(align);
		}
		const sf::Color& getImageColor(int id) const { return imageColors[id]; }
		friend inline void read(BinaryFileStream& bf, ImageObject& x) {
			bf.readStruct(static_cast<UIBase&>(x), x.imageId, x.scale, x.align, x.imageColors);
		}
		friend inline void write(BinaryFileStream& bf, const ImageObject& x) {
			bf.writeStruct(static_cast<const UIBase&>(x), x.imageId, x.scale, x.align, x.imageColors);
		}
		friend inline void to_json(nlohmann::json& j, const ImageObject& x) {
			nlohmann::json base = static_cast<const UIBase&>(x);
			j = nlohmann::json{ {"UIBase", base}, {"imageId", x.imageId}, {"scale", x.scale}, {"align", x.align}, {"imageColors", x.imageColors} };
		}
		friend inline void from_json(const nlohmann::json& j, ImageObject& x) {
			nlohmann::json base;
			j.at("UIBase").get_to(base);
			base.get_to(static_cast<UIBase&>(x));
			j.at("imageId").get_to(x.imageId);
			j.at("scale").get_to(x.scale);
			j.at("align").get_to(x.align);
			j.at("imageColors").get_to(x.imageColors);
		}
	};
	class TextStyle{
	public:
		sf::Color fillColor;
		sf::Color outlineColor;
		void set(sf::Color _fillColor, sf::Color _outlineColor) {
			fillColor = _fillColor;
			outlineColor = _outlineColor;
		}
		friend inline void read(BinaryFileStream& bf, TextStyle& x) {
			bf.readStruct(x.fillColor, x.outlineColor);
		}
		friend inline void write(BinaryFileStream& bf, const TextStyle& x) {
			bf.writeStruct(x.fillColor, x.outlineColor);
		}
		friend inline void to_json(nlohmann::json& j, const TextStyle& x) {
			j = nlohmann::json{ {"fillColor", x.fillColor}, {"outlineColor", x.outlineColor} };
		}
		friend inline void from_json(const nlohmann::json& j, TextStyle& x) {
			j.at("fillColor").get_to(x.fillColor);
			j.at("outlineColor").get_to(x.outlineColor);
		}
	};
	class TextObject :public UIBase {
		friend class WindowManager;
	public:
		TextObject() {
			textStyles[gui::UIBase::Normal].set(sf::Color::Black, sf::Color::Black);
			textStyles[gui::UIBase::Over].set(sf::Color::Black, sf::Color::Black);
			textStyles[gui::UIBase::Focus].set(sf::Color::Black, sf::Color::Black);
		}
		TextObject& setTextStyle(const TextStyle& _normalStyle, const TextStyle& _overStyle, const TextStyle& _focusStyle) {
			textStyles[gui::UIBase::Normal] = _normalStyle;
			textStyles[gui::UIBase::Over] = _overStyle;
			textStyles[gui::UIBase::Focus] = _focusStyle;
			return *this;
		}
		TextStyle& textStyle(int id) {
			return textStyles[id];
		}
		//use this setter
		//after : setFont() , setText() , setCharacterSize()
		TextObject& setSizeAuto() {
			textRender.setCharacterSize(characterSize);
			textRender.setLineSpacing(lineSpacing);
			textRender.setLetterSpacing(letterSpacing);
			textRender.setString(text);
			textRender.setPosition({ 0, 0 });
			sf::Vector2f size;
			size.x = 0;
			for (int i = 0; i <= text.getSize(); i++) {
				if (textRender.findCharacterPos(i).x > size.x)
					size.x = textRender.findCharacterPos(i).x;
			}
			size.y = textRender.findCharacterPos(textRender.getString().getSize()).y + characterSize;
			setSize(size);
			return *this;
		}
	protected:
		std::string font = "";
		unsigned int characterSize = 30;
		float letterSpacing = 1, lineSpacing = 1;
		sf::String text = "";
		sf::Text textRender{ fontManager[font] };
		sf::FloatRect textRect;
		sf::Vector2f textRenderOffsetFix;
		sf::Vector2i align = { static_cast<int>(gui::UIBase::Align::Mid), static_cast<int>(gui::UIBase::Align::Mid) };
		TextStyle textStyles[3];
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
		TextObject& setSpacing(float _letterSpacing, float _lineSpacing) {
			letterSpacing = _letterSpacing;
			lineSpacing = _lineSpacing;
			return *this;
		}
		TextObject& setAlign(gui::UIBase::Align xAlign, gui::UIBase::Align yAlign) {
			align.x = static_cast<int>(xAlign);
			align.y = static_cast<int>(yAlign);
			return *this;
		}
		TextObject& setText(sf::String _text) {
			text = _text;
			return *this;
		}
		sf::String& getText() {
			return text;
		}
		const sf::String& getText() const {
			return text;
		}
		const std::string& getFont() const { return font; }
		unsigned int getCharacterSize() const { return characterSize; }
		float getLetterSpacing() const { return letterSpacing; }
		float getLineSpacing() const { return lineSpacing; }
		sf::Vector2i getAlign() const { return align; }
		friend inline void read(BinaryFileStream& bf, TextObject& x) {
			bf.readStruct(static_cast<UIBase&>(x), x.textStyles, x.font, x.characterSize, x.align, x.letterSpacing, x.lineSpacing, x.text);
			x.textRender.setFont(fontManager[x.font]);
		}
		friend inline void write(BinaryFileStream& bf, const TextObject& x) {
			bf.writeStruct(static_cast<const UIBase&>(x), x.textStyles, x.font, x.characterSize, x.align, x.letterSpacing, x.lineSpacing, x.text);
		}
		friend inline void to_json(nlohmann::json& j, const TextObject& x) {
			nlohmann::json base = static_cast<const UIBase&>(x);
			j = nlohmann::json{ {"UIBase", base}, {"textStyles", x.textStyles}, {"font", x.font}, {"characterSize", x.characterSize}, {"align", x.align}, {"letterSpacing", x.letterSpacing}, {"lineSpacing", x.lineSpacing}, {"text", x.text} };
		}
		friend inline void from_json(const nlohmann::json& j, TextObject& x) {
			nlohmann::json base;
			j.at("UIBase").get_to(base);
			base.get_to(static_cast<UIBase&>(x));
			j.at("textStyles").get_to(x.textStyles);
			j.at("font").get_to(x.font);
			j.at("characterSize").get_to(x.characterSize);
			j.at("align").get_to(x.align);
			j.at("letterSpacing").get_to(x.letterSpacing);
			j.at("lineSpacing").get_to(x.lineSpacing);
			j.at("text").get_to(x.text);
			x.textRender.setFont(fontManager[x.font]);
		}
	};
	class ButtonObject :public TextObject {
		friend class WindowManager;
	public:
		ButtonObject() {
			styles[gui::UIBase::Normal].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2);
			styles[gui::UIBase::Over].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2);
			styles[gui::UIBase::Focus].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2);
		}
	};
	class OptionObject :public ButtonObject {
		friend class WindowManager;
	};
	class InputObject :public TextObject {
		friend class WindowManager;
	public:
		InputObject() {
			styles[gui::UIBase::Normal].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2);
			styles[gui::UIBase::Over].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2);
			styles[gui::UIBase::Focus].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2);
		}
	protected:
		int typeLimit = gui::InputObject::String;
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
			friend inline void read(BinaryFileStream& bf, InputLimit& x) {
				bf.readStruct(x.isAllowList, x.single, x.range);
			}
			friend inline void write(BinaryFileStream& bf, const InputLimit& x) {
				bf.writeStruct(x.isAllowList, x.single, x.range);
			}
			friend inline void to_json(nlohmann::json& j, const InputLimit& x) {
				j = nlohmann::json{ {"isAllowList", x.isAllowList}, {"single", x.single}, {"range", x.range} };
			}
			friend inline void from_json(const nlohmann::json& j, InputLimit& x) {
				j.at("isAllowList").get_to(x.isAllowList);
				j.at("single").get_to(x.single);
				j.at("range").get_to(x.range);
			}
		}inputLimit;
		size_t cursor = 0;
		sf::Vector2f scroll;
		void draw(sf::RenderTarget& r, sf::FloatRect displayArea, WindowManager& windowManager);
		inline void insert(char32_t ch) {
			if (text.getSize() >= sizeLimit)return;
			if (typeLimit == gui::InputObject::Int) {
				if ((ch == '-' && cursor == 0 && ensure(text.getSize() > 0, text[0] != '-')) ||
					(ch >= '0' && ch <= '9' && ensure(text.getSize() > 0, ensure(text[0] == '-', cursor != 0)))) {
					text.insert(cursor, ch);
					cursor++;
				}
			}
			else if (typeLimit == gui::InputObject::Float) {
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
			if (typeLimit == gui::InputObject::Int || typeLimit == gui::InputObject::Float) {
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
		void updateCursorByMousePos(sf::Vector2f mousePos) {
			size_t bestCursor = 0;
			float minDist = std::numeric_limits<float>::max();
			for (size_t i = 0; i <= text.getSize(); i++) {
				sf::Vector2f charPos = textRender.findCharacterPos(i);
				charPos += textRenderOffsetFix;
				float distX = std::abs(charPos.x - mousePos.x);
				float distY = std::abs((charPos.y + characterSize / 2.f) - mousePos.y);
				float dist = distX * distX + distY * distY;
				if (dist < minDist) {
					minDist = dist;
					bestCursor = i;
				}
			}
			cursor = bestCursor;
		}
	public:
		enum InputType { String = 0, Int = 1, Float = 2 };
		InputObject& setSizeLimit(int _sizeLimit) {
			sizeLimit = _sizeLimit;
			return *this;
		}
		InputObject& setTypeLimit(gui::InputObject::InputType _typeLimit) {
			typeLimit = _typeLimit;
			return *this;
		}
		InputObject& setStringTypeLimit(bool _isAllowList, std::initializer_list<char32_t> _single, std::initializer_list<std::pair<char32_t, char32_t>> _range) {
			typeLimit = gui::InputObject::String;
			inputLimit.set(_isAllowList, _single, _range);
			return *this;
		}
		InputObject& setText(sf::String _text) {
			text = _text;
			cursor = text.getSize();
			return *this;
		}
		int getTypeLimit() const { return typeLimit; }
		size_t getSizeLimit() const { return sizeLimit; }
		friend inline void read(BinaryFileStream& bf, InputObject& x) {
			bf.readStruct(static_cast<TextObject&>(x), x.sizeLimit, x.typeLimit, x.inputLimit);
			x.cursor = x.text.getSize();
		}
		friend inline void write(BinaryFileStream& bf, const InputObject& x) {
			bf.writeStruct(static_cast<const TextObject&>(x), x.sizeLimit, x.typeLimit, x.inputLimit);
		}
		friend inline void to_json(nlohmann::json& j, const InputObject& x) {
			nlohmann::json base = static_cast<const TextObject&>(x);
			j = nlohmann::json{ {"TextObject", base}, {"sizeLimit", x.sizeLimit}, {"typeLimit", x.typeLimit}, {"inputLimit", x.inputLimit} };
		}
		friend inline void from_json(const nlohmann::json& j, InputObject& x) {
			nlohmann::json base;
			j.at("TextObject").get_to(base);
			base.get_to(static_cast<TextObject&>(x));
			j.at("sizeLimit").get_to(x.sizeLimit);
			j.at("typeLimit").get_to(x.typeLimit);
			j.at("inputLimit").get_to(x.inputLimit);
			x.cursor = x.text.getSize();
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
			styles[gui::UIBase::Normal].set(sf::Color::White, sf::Color(200, 200, 200), 2);
			styles[gui::UIBase::Over].set(sf::Color::White, sf::Color(200, 200, 200), 2);
			styles[gui::UIBase::Focus].set(sf::Color::White, sf::Color(200, 200, 200), 2);
		}
		VarianTmap<UIBase>sub;
		AreaObject& setScrollable(sf::Vector2i _mouseDragScrollable, sf::Vector2i _mouseWheelScrollable) {
			mouseDragScrollable = _mouseDragScrollable;
			mouseWheelScrollable = _mouseWheelScrollable;
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
		AreaObject& setScrollLimitAuto() {
			scrollLimit = sf::FloatRect(sf::Vector2f(), posRect.size);
			for (auto& elem : sub) {
				addPoint(scrollLimit, elem->posRect.position);
				addPoint(scrollLimit, elem->posRect.position + elem->posRect.size);
			}
			scrollLimit.size -= posRect.size;
			return *this;
		}
	public:
		friend inline void read(BinaryFileStream& bf, AreaObject& x) {
			bf.readStruct(static_cast<UIBase&>(x), VarianTmapSerializerWrapper<UIBase, AreaObject, ImageObject, TextObject, InputObject, ButtonObject, OptionObject>{x.sub}, x.mouseDragScrollable, x.mouseWheelScrollable, x.option);
		}
		friend inline void write(BinaryFileStream& bf, const AreaObject& x) {
			bf.writeStruct(static_cast<const UIBase&>(x), VarianTmapSerializerWrapper<UIBase, AreaObject, ImageObject, TextObject, InputObject, ButtonObject, OptionObject>{x.sub}, x.mouseDragScrollable, x.mouseWheelScrollable, x.option);
		}
		friend inline void to_json(nlohmann::json& j, const AreaObject& x) {
			nlohmann::json base = static_cast<const UIBase&>(x);
			j = nlohmann::json{ {"UIBase", base}, {"sub", VarianTmapJsonSerializerWrapper<UIBase, AreaObject, ImageObject, TextObject, InputObject, ButtonObject, OptionObject>{const_cast<VarianTmap<UIBase>&>(x.sub)}}, {"mouseDragScrollable", x.mouseDragScrollable}, {"mouseWheelScrollable", x.mouseWheelScrollable}, {"option", x.option} };
		}
		friend inline void from_json(const nlohmann::json& j, AreaObject& x) {
			nlohmann::json base;
			j.at("UIBase").get_to(base);
			base.get_to(static_cast<UIBase&>(x));
			auto subWrapper = VarianTmapJsonSerializerWrapper<UIBase, AreaObject, ImageObject, TextObject, InputObject, ButtonObject, OptionObject>{x.sub};
			j.at("sub").get_to(subWrapper);
			j.at("mouseDragScrollable").get_to(x.mouseDragScrollable);
			j.at("mouseWheelScrollable").get_to(x.mouseWheelScrollable);
			j.at("option").get_to(x.option);
		}
		AreaObject& setOption(const std::string& key) {
			if (option!="") {
				if (auto ptr = sub.find_named<OptionObject>(option))
					ptr->setStatu(gui::UIBase::Normal, true);
				else option = "";
			}
			option = key;
			if (auto ptr = sub.find_named<OptionObject>(key)) {
				ptr->setStatu(gui::UIBase::Focus);
			}
			return *this;
		}
		AreaObject& setOption() {
			if (option != "") {
				if (auto ptr = sub.find_named<OptionObject>(option))
					ptr->setStatu(gui::UIBase::Normal, true);
				else option = "";
			}
			option = "";
			return *this;
		}
		std::string getOption() {
			return option;
		}
		const std::string getOption() const {
			return option;
		}
		sf::Vector2i getMouseDragScrollable() const { return mouseDragScrollable; }
		sf::Vector2i getMouseWheelScrollable() const { return mouseWheelScrollable; }
	protected:
		void updateOption() {
			if (option!="") {
				if (auto ptr = sub.find_named<OptionObject>(option))
					ptr->setStatu(gui::UIBase::Focus);
				else option = "";
			}
		}
		void ensureScrollLimit() {
			setScrollLimitAuto();
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
		const T& path_at(const std::string& path) const {
			const AreaObject* areaPtr = this;
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
		template<typename T>
		T& path_at(const std::string& path) {
			return const_cast<T&>(std::as_const(*this).path_at<T>(path));
		}
		template<typename T>
		T* path_find(const std::string& path) const {
			const AreaObject* areaPtr = this;
			std::string temp = "";
			for (int i = 0; i < path.size(); i++) {
				if (path[i] == '_' || path[i] == '.') {
					areaPtr = areaPtr->sub.find_named<AreaObject>(temp);
					temp = "";
					if (areaPtr == nullptr)return nullptr;
				}
				else temp.push_back(path[i]);
			}
			return areaPtr->sub.find_named<T>(temp);
		}
		template<typename T>
		T* path_find(const std::string& path) {
			return const_cast<T*>(std::as_const(*this).path_find<T>(path));
		}
	};
	class WindowManager {
		friend class InputObject;
		friend class AreaObject;
	public:
		WindowManager() {}
		//noncopyable
		WindowManager(const WindowManager&) = delete;
		WindowManager& operator=(const WindowManager&) = delete;
	private:
		//窗口管理

		//cursorBlinkRate : how many ticks the cursor blinks
		unsigned int cursorBlinkRate = 30;
		float scrollResistance = 3.f;
		//scroll sensitivity = 6 tick = 0.1 s (60 FPS)
		static constexpr int scrollSensitivity = 6;
		float mouseWheelScrollRate = 50.f;
		float scrollThreshold = 20;

		EventQueue event;
		struct ObjectPath {
			std::optional<std::type_index> type;
			std::string path;
			std::string name;
			template<typename T>
			void setType() {
				type = std::type_index(typeid(T));
			}
			template<typename T>
			bool is() const {
				return std::type_index(typeid(T)) == type;
			}
			void clear() {
				type = std::nullopt;
				path.clear();
				name.clear();
			}
			bool operator==(const ObjectPath& other) const = default;
		}focus, over;
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
			event.clear();
			focus.clear(); over.clear();
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
		size_t close(const std::string& id) {
			if (!layer.empty() && layer.find_key(std::prev(layer.end())) == id) {
				layer.erase(std::prev(layer.end()));
				return 1;
			}
			return 0;
		}
		size_t closeUntil(const std::string& id, bool include = false) {
			size_t count = 0;
			while (!layer.empty() && layer.find_key(std::prev(layer.end())) != id) {
				layer.erase(std::prev(layer.end()));
				count++;
			}
			if (include && !layer.empty() && layer.find_key(std::prev(layer.end())) == id) {
				layer.erase(std::prev(layer.end()));
				count++;
			}
			return count;
		}
		AreaObject& window(const std::string& id) {
			if (auto ptr = layer.find_named<AreaObject>(id))
				return *ptr;
			else {
				std::cerr << "[WindowManager::window] Window ID not found 未找到指定的窗口ID\n  id: " << id << "\n";
				throw std::runtime_error("[WindowManager::window] Window ID not found 未找到指定的窗口ID\n  id: " + id + "\n");
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
			else {
				if (areaPtr != nullptr)
					return areaPtr->sub.get<T>(temp);
				else {
					std::cerr << "[WindowManager::path_get] Path error. 路径错误\n  path: " << path << "\n";
					throw std::runtime_error("[WindowManager::path_get] Path error. 路径错误\n  path: " + path + "\n");
				}
			}
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
				else return areaPtr->sub.at<T>(temp);
			}
			else {
				if (areaPtr != nullptr)
					return areaPtr->sub.at<T>(temp);
				else {
					std::cerr << "[WindowManager::path_at] Path error. 路径错误\n  path: " << path << "\n";
					throw std::runtime_error("[WindowManager::path_at] Path error. 路径错误\n  path: " + path + "\n");
				}
			}
		}
		template<typename T>
		T* path_find(const std::string& path) {
			AreaObject* areaPtr = nullptr;
			std::string temp = "";
			for (int i = 0; i < path.size(); i++) {
				if (path[i] == '_' || path[i] == '.') {
					if (areaPtr == nullptr)
						areaPtr = layer.find_named<AreaObject>(temp);
					else areaPtr = areaPtr->sub.find_named<AreaObject>(temp);
					temp = "";
					if (areaPtr == nullptr)return nullptr;
				}
				else temp.push_back(path[i]);
			}
			if constexpr (std::is_same_v<T, AreaObject>) {
				if (areaPtr == nullptr)
					return layer.find_named<AreaObject>(temp);
				else return areaPtr->sub.find_named<T>(temp);
			}
			else return areaPtr->sub.find_named<T>(temp);
		}
		const std::optional<Event> pollEvent() {
			return event.pollEvent();
		}
	private:
		inline UIBase* objectPathVisit(ObjectPath& obj, AreaObject* areaPtr = nullptr) {
			if (!obj.type.has_value())return nullptr;
			if (areaPtr == nullptr) {
				areaPtr = path_find<gui::AreaObject>(obj.path);
				if (areaPtr == nullptr) return nullptr;
			}

			if (obj.is<gui::ButtonObject>())
				return areaPtr->sub.find_named<ButtonObject>(obj.name);
			else if (obj.is<gui::OptionObject>())
				return areaPtr->sub.find_named<OptionObject>(obj.name);
			else if (obj.is<gui::InputObject>())
				return areaPtr->sub.find_named<InputObject>(obj.name);
			else if (obj.is<gui::AreaObject>())
				return areaPtr;
			else return nullptr;
		}
		inline AreaObject* updateOver(bool stopScroll = false) {
			if (objectPathVisit(over) != nullptr)
				objectPathVisit(over)->setStatu(gui::UIBase::Normal);
			AreaObject* areaPtr = *std::prev(layer.end());
			sf::Vector2f origin = areaPtr->posRect.position;
			over.clear();
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
							if (areaPtr == nullptr)
								return nullptr;
							over.path = path;
							if (areaPtr->sub.find<ButtonObject>(elem)) {
								over.setType<ButtonObject>();
								over.name = areaPtr->sub.find_key(elem);
								return areaPtr;
							}
							if (areaPtr->sub.find<OptionObject>(elem)) {
								over.setType<OptionObject>();
								over.name = areaPtr->sub.find_key(elem);
								return areaPtr;
							}
							if (areaPtr->sub.find<InputObject>(elem)) {
								over.setType<InputObject>();
								over.name = areaPtr->sub.find_key(elem);
								return areaPtr;
							}
							over.setType<AreaObject>();
						}
					}
				}
				break;
			searchcontinue:;
			}
			over.setType<AreaObject>();
			over.path = path;
			return areaPtr;
		}
		inline void updateSimpleMove(AreaObject* areaFocusPtr, AreaObject* areaOverPtr) {
			//update scroll
			if (areaFocusPtr != nullptr && mousePressed) {
				if (areaFocusPtr->mouseDragScrollable != sf::Vector2i()) {
					if (ensure(focus.is<ButtonObject>() || focus.is<OptionObject>(), (mouseLastPressPos - mousePos.back()).lengthSquared() >= scrollThreshold * scrollThreshold))
						isDragScrolling = true;
					if ((focus.is<ButtonObject>() || focus.is<OptionObject>()) && isDragScrolling) {
						if (over == focus)
							objectPathVisit(focus, areaFocusPtr)->setStatu(gui::UIBase::Over, true);
						else objectPathVisit(focus, areaFocusPtr)->setStatu(gui::UIBase::Normal, true);
						if (focus.is<OptionObject>())
							areaFocusPtr->updateOption();
					}
				}
				else {
					if (areaFocusPtr != nullptr && (focus.is<ButtonObject>() || focus.is<OptionObject>())) {
						if (over == focus)
							objectPathVisit(focus, areaFocusPtr)->setStatu(gui::UIBase::Focus);
						else objectPathVisit(focus, areaFocusPtr)->setStatu(gui::UIBase::Over, true);
						if (focus.is<OptionObject>())
							areaFocusPtr->updateOption();
					}
				}
			}
			//update over
			//after updating over ,varible 'focus' will not be changed
			else {
				if (objectPathVisit(over, areaOverPtr) != nullptr)
					objectPathVisit(over, areaOverPtr)->setStatu(gui::UIBase::Over);
			}
		}
	public:
		bool update(const std::optional<sf::Event>& sfEvent) {
			if (layer.size() == 0)
				return false;
			//update mouse press statu
			if (auto ptr = sfEvent->getIf<sf::Event::MouseButtonPressed>()) {
				mousePressed = true;
				mouseLastPressPos = static_cast<sf::Vector2f>(ptr->position);
			}
			if (sfEvent->is<sf::Event::MouseButtonReleased>())
				mousePressed = false;
			if (objectPathVisit(focus) == nullptr)
				focus.clear();
			AreaObject* areaOverPtr = nullptr, * areaFocusPtr = nullptr;
			if (focus.type.has_value())
				areaFocusPtr = path_find<AreaObject>(focus.path);
			//sf::Event::MouseButtonPressed
			//update focus
			//after updating focus ,varible 'focus' & 'over' must have a value
			if (auto ptr = sfEvent->getIf<sf::Event::MouseButtonPressed>()) {
				mousePos.back() = sf::Vector2f(ptr->position);//update mousePos

				//update inertial scroll stop
				areaOverPtr = updateOver(true);

				if (focus.is<InputObject>() && over != focus)
					event.push(gui::Events::InputDeselected{ {.path = focus.path,.name = focus.name} });
				if (over.is<InputObject>() && over != focus)
					event.push(gui::Events::InputSelected{ {.path = over.path,.name = over.name} });

				if (auto ptr = objectPathVisit(focus, areaFocusPtr))
					ptr->setStatu(gui::UIBase::Normal, true);
				if (areaFocusPtr != nullptr)
					areaFocusPtr->updateOption();
				if (auto ptr = objectPathVisit(over, areaOverPtr))
					ptr->setStatu(gui::UIBase::Focus);
				if (over.is<InputObject>()) {
					InputObject& input = areaOverPtr->sub.at<InputObject>(over.name);
					sf::Vector2f mouseInLocal = mousePos.back() - areaOverPtr->posRect.position - areaOverPtr->scroll - input.posRect.position;
					input.updateCursorByMousePos(mouseInLocal);
				}

				focus = over;
				return true;
			}
			//sf::Event::MouseMoved
			if (auto ptr = sfEvent->getIf<sf::Event::MouseMoved>()) {
				sf::Vector2f mousePosDelta = sf::Vector2f(ptr->position) - mousePos.back();
				mousePos.back() = sf::Vector2f(ptr->position);//update mousePos
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
			if (auto ptr = sfEvent->getIf<sf::Event::MouseButtonReleased>()) {
				mousePos.back() = sf::Vector2f(ptr->position);//update mousePos
				areaOverPtr = updateOver();

				if (areaFocusPtr != nullptr && focus.is<ButtonObject>()) {
					if (over == focus) {
						if (!isDragScrolling)
							event.push(gui::Events::ButtonPressed{ {.path = focus.path,.name = focus.name} });
						areaFocusPtr->sub.at<ButtonObject>(focus.name).setStatu(gui::UIBase::Over, true);
					}
					else areaFocusPtr->sub.at<ButtonObject>(focus.name).setStatu(gui::UIBase::Normal, true);
				}
				if (areaFocusPtr != nullptr && focus.is<OptionObject>()) {
					if (over == focus) {
						if (!isDragScrolling) {
							if (focus.name != areaFocusPtr->option) {
								if (areaFocusPtr->option != "") {
									areaFocusPtr->sub.at<OptionObject>(areaFocusPtr->option).setStatu(gui::UIBase::Normal, true);
									event.push(gui::Events::OptionDeselected{ {.path = focus.path,.name = areaFocusPtr->option} });
								}
								areaFocusPtr->option = focus.name;
								event.push(gui::Events::OptionSelected{ {.path = focus.path,.name = focus.name} });
							}
						}
					}
					else areaFocusPtr->sub.at<OptionObject>(focus.name).setStatu(gui::UIBase::Normal, true);
					areaFocusPtr->updateOption();
				}
				isDragScrolling = false;

				//update inertial scroll start
				if (areaFocusPtr != nullptr && focus.is<AreaObject>()) {
					if (areaFocusPtr->mouseDragScrollable != sf::Vector2i())
						areaFocusPtr->scrollVelocity = mouseVelocity()
						.componentWiseMul(static_cast<sf::Vector2f>(areaFocusPtr->mouseDragScrollable))
						.componentWiseDiv(sf::Vector2f(scrollSensitivity, scrollSensitivity));
				}
				return true;
			}
			if (auto ptr = sfEvent->getIf<sf::Event::TextEntered>()) {
				if (areaFocusPtr != nullptr && focus.is<InputObject>()) {
					char32_t ch = ptr->unicode;
					InputObject& InputTar = areaFocusPtr->sub.at<InputObject>(focus.name);
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
			if (auto ptr = sfEvent->getIf<sf::Event::KeyPressed>()) {
				if (areaFocusPtr != nullptr && focus.is<InputObject>()) {
					InputObject& InputTar = areaFocusPtr->sub.at<InputObject>(focus.name);
					if (ptr->code == sf::Keyboard::Key::Left)//left arrow
						InputTar.moveCursor(true);
					if (ptr->code == sf::Keyboard::Key::Right)//right arrow
						InputTar.moveCursor(false);
					if (ptr->code == sf::Keyboard::Key::Delete)//delete key
						InputTar.erase(false);
					cursorBlinkTick = 0;
				}
				return true;
			}
			if (auto ptr = sfEvent->getIf<sf::Event::MouseWheelScrolled>()) {
				areaOverPtr = updateOver();
				float delta = ptr->delta;
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
				if (objectPathVisit(focus) == nullptr)
					focus.clear();
				updateSimpleMove(focus.type.has_value() ? path_find<AreaObject>(focus.path) : nullptr, updateOver());
			}
			//更新所有子对象的位置
			for (auto& elem : layer.iterate()) {
				elem->updatePosRect(static_cast<sf::Vector2f>(drawTarget.getSize()));
			}
			if (focus.is<InputObject>()) {
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
