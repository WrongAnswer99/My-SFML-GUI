#include "engine/gui/MyGUI.hpp"
#include <fstream>
#include <functional>

std::unordered_map<std::string, gui::Style>style;
std::unordered_map<std::string, gui::TextStyle>textStyle;
namespace attr {
#define def(x) constexpr const char* x=#x;
	namespace designer {
		def(AreaName);
		def(ButtonName);
		def(ImageName);
		def(InputName);
		def(OptionName);
		def(TextName);

		namespace type {
			def((area));
			def((button));
			def((image));
			def((input));
			def((option));
			def((text));
		}
	}
	namespace gtext {
		def(new_isSub);
		def(new_isSubDisabledText);
		def(paste_isSub);
		def(paste_isSubDisabledText);
		//TitleTexts
		def(UIBaseSettingsTitle);
		def(AreaSettingsTitle);
		def(TextSettingsTitle);
		def(ImageSettingsTitle);
		def(InputSettingsTitle);
		def(OptionSettingsTitle);
		//UIBase
		def(main_settings_SetCenterPosition);
		def(main_settings_SetLeftUpPosition);
		def(main_settings_SetSize);
		def(main_settings_SetShow);
		def(main_settings_SetNormalBackgroundColor);
		def(main_settings_SetNormalOutlineColor);
		def(main_settings_SetNormalOutlineThickness);

		def(main_settings_SetOverBackgroundColor);
		def(main_settings_SetOverOutlineColor);
		def(main_settings_SetOverOutlineThickness);

		def(main_settings_SetFocusBackgroundColor);
		def(main_settings_SetFocusOutlineColor);
		def(main_settings_SetFocusOutlineThickness);
		//Area
		def(main_settings_SetOption);
		def(main_settings_SetScrollableDrag);
		def(main_settings_SetScrollableWheel);
		//Text
		def(main_settings_SetText);

		def(main_settings_SetTextOverBackgroundColor);
		def(main_settings_SetTextOverOutlineColor);

		def(main_settings_SetTextFocusBackgroundColor);
		def(main_settings_SetTextFocusOutlineColor);

		def(main_settings_SetFont);
		def(main_settings_SetCharacterSize);
		def(main_settings_SetLetterSpacing);
		def(main_settings_SetLineSpacing);
		def(main_settings_SetJustification);
	}
	namespace gimage {
		def(main_settings_SetImageId);
		def(main_settings_SetScaleX);
		def(main_settings_SetScaleY);
		def(main_settings_SetJustificationX_Left);
		def(main_settings_SetJustificationX_Mid);
		def(main_settings_SetJustificationX_Right);
		def(main_settings_SetJustificationY_Top);
		def(main_settings_SetJustificationY_Mid);
		def(main_settings_SetJustificationY_Bottom);

		def(main_settings_SetImageNormalColorR);
		def(main_settings_SetImageNormalColorG);
		def(main_settings_SetImageNormalColorB);
		def(main_settings_SetImageNormalColorA);

		def(main_settings_SetImageOverColorR);
		def(main_settings_SetImageOverColorG);
		def(main_settings_SetImageOverColorB);
		def(main_settings_SetImageOverColorA);

		def(main_settings_SetImageFocusColorR);
		def(main_settings_SetImageFocusColorG);
		def(main_settings_SetImageFocusColorB);
		def(main_settings_SetImageFocusColorA);
	}
	namespace gbutton {
		def(main_open);
		def(main_merge);
		def(main_save);
		def(main_saveas);
		def(main_split);
		def(main_exportJSON);
		def(main_exportBinary);
		def(main_exportReflection);
		def(main_about);
		def(main_new);
		def(main_delete);
		def(main_moveup);
		def(main_movedown);
		def(main_cut);
		def(main_copy);
		def(main_paste);

		def(new_isSubText);
		def(new_ok);
		def(new_cancel);

		def(open_ok);
		def(open_cancel);

		def(save_ok);
		def(save_cancel);

		def(paste_isSubText);
		def(paste_ok);
		def(paste_cancel);

		def(main_settings_SetText);
		def(main_settings_SetFont);
		def(main_settings_SetCharacterSize);
		def(main_settings_SetLetterSpacing);
		def(main_settings_SetLineSpacing);
		def(main_settings_SetJustificationX_Left);
		def(main_settings_SetJustificationX_Mid);
		def(main_settings_SetJustificationX_Right);
		def(main_settings_SetJustificationY_Top);
		def(main_settings_SetJustificationY_Mid);
		def(main_settings_SetJustificationY_Bottom);
	}
	namespace goption {
		def(new_area);
		def(new_input);
		def(new_button);
		def(new_option);
		def(new_image);
		def(new_text);
		def(new_window);

		def(open_binary);
		def(open_json);

		def(main_settings_SetText);
		def(main_settings_SetFont);
		def(main_settings_SetCharacterSize);
		def(main_settings_SetLetterSpacing);
		def(main_settings_SetLineSpacing);
		def(main_settings_SetJustificationX_Left);
		def(main_settings_SetJustificationX_Mid);
		def(main_settings_SetJustificationX_Right);
		def(main_settings_SetJustificationY_Top);
		def(main_settings_SetJustificationY_Mid);
		def(main_settings_SetJustificationY_Bottom);
	}
	namespace ginput {
		def(new_name);
		def(open_filepath);
		def(save_filepath);
		//UIBase
		def(main_settings_SetLeftUpPositionX);
		def(main_settings_SetLeftUpPositionY);
		def(main_settings_SetCenterPositionX);
		def(main_settings_SetCenterPositionY);
		def(main_settings_SetSizeX);
		def(main_settings_SetSizeY);
		def(main_settings_SetShow);

		def(main_settings_SetNormalBackgroundColorR);
		def(main_settings_SetNormalBackgroundColorG);
		def(main_settings_SetNormalBackgroundColorB);
		def(main_settings_SetNormalBackgroundColorA);
		def(main_settings_SetNormalOutlineColorR);
		def(main_settings_SetNormalOutlineColorG);
		def(main_settings_SetNormalOutlineColorB);
		def(main_settings_SetNormalOutlineColorA);
		def(main_settings_SetNormalOutlineThickness);

		def(main_settings_SetOverBackgroundColorR);
		def(main_settings_SetOverBackgroundColorG);
		def(main_settings_SetOverBackgroundColorB);
		def(main_settings_SetOverBackgroundColorA);
		def(main_settings_SetOverOutlineColorR);
		def(main_settings_SetOverOutlineColorG);
		def(main_settings_SetOverOutlineColorB);
		def(main_settings_SetOverOutlineColorA);
		def(main_settings_SetOverOutlineThickness);

		def(main_settings_SetFocusBackgroundColorR);
		def(main_settings_SetFocusBackgroundColorG);
		def(main_settings_SetFocusBackgroundColorB);
		def(main_settings_SetFocusBackgroundColorA);
		def(main_settings_SetFocusOutlineColorR);
		def(main_settings_SetFocusOutlineColorG);
		def(main_settings_SetFocusOutlineColorB);
		def(main_settings_SetFocusOutlineColorA);
		def(main_settings_SetFocusOutlineThickness);
		//Area
		def(main_settings_SetOption);
		def(main_settings_SetScrollableDragX);
		def(main_settings_SetScrollableDragY);
		def(main_settings_SetScrollableWheelX);
		def(main_settings_SetScrollableWheelY);
		//Text
		def(main_settings_SetText);
		def(main_settings_SetFont);
		def(main_settings_SetCharacterSize);
		def(main_settings_SetLetterSpacing);
		def(main_settings_SetLineSpacing);
		def(main_settings_SetJustificationX_Left);
		def(main_settings_SetJustificationX_Mid);
		def(main_settings_SetJustificationX_Right);
		def(main_settings_SetJustificationY_Top);
		def(main_settings_SetJustificationY_Mid);
		def(main_settings_SetJustificationY_Bottom);
		//Input
		def(main_settings_SetTypeLimit_String);
		def(main_settings_SetTypeLimit_Int);
		def(main_settings_SetTypeLimit_Float);
		def(main_settings_SetSizeLimit);
	}
	namespace garea {
		def(main_list);
		def(main_settings);
	}
#undef def
}
int windowWidth = 1600, windowHeight = 900;
gui::WindowManager menuManager,previewManager;
sf::RenderWindow menu, preview;
gui::AreaObject Main, New, Open, Save, Paste;
namespace settings {
	gui::AreaObject UIBase, Area, Input, Image, Text;
}
namespace Init {
	gui::InputObject& addSimpleInput(gui::AreaObject& area, const std::string& name, gui::InputObject::InputType type, sf::Vector2f pos, sf::Vector2f size, const sf::String& text = L"<null>") {
		area.path_get<gui::InputObject>(name)
			.setTypeLimit(type)
			.setText(text)
			.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
			.setFont("ht")
			.setCharacterSize(40)
			.setPosition(pos)
			.setSize(size);
		return area.path_get<gui::InputObject>(name);
	}
	gui::InputObject& addBoolInput(gui::AreaObject& area, const std::string& name, sf::Vector2f pos, sf::Vector2f size) {
		addSimpleInput(area, name, gui::InputObject::String, pos, size)
			.setStringTypeLimit(true, { '0','1' }, {})
			.setSizeLimit(1);
		return area.path_get<gui::InputObject>(name);
	}
	gui::InputObject& addStringInput(gui::AreaObject& area, const std::string& name, sf::Vector2f pos, sf::Vector2f size) {
		return addSimpleInput(area, name, gui::InputObject::String, pos, size);
	}
	gui::InputObject& addAlphaInput(gui::AreaObject& area, const std::string& name, sf::Vector2f pos, sf::Vector2f size) {
		addSimpleInput(area, name, gui::InputObject::String, pos, size)
			.setStringTypeLimit(true, {}, { {L'A',L'Z'},{L'a',L'z'},{L'0',L'9'} });
		return area.path_get<gui::InputObject>(name);
	}
	gui::TextObject& addSimpleText(gui::AreaObject& area, const std::string& name, const sf::String& text, sf::Vector2f pos, gui::UIBase::Justification hJust = gui::UIBase::Left, gui::UIBase::Justification vJust = gui::UIBase::Mid) {
		area.path_get<gui::TextObject>(name)
			.setText(text)
			.setFont("ht")
			.setCharacterSize(40)
			.setSizeAuto()
			.setJustification(hJust, vJust)
			.setPosition(pos);
		return area.path_get<gui::TextObject>(name);
	}
	gui::TextObject& addTitleText(gui::AreaObject& area, const std::string& name, const sf::String& text, sf::Vector2f pos, sf::Vector2f size) {
		area.path_get<gui::TextObject>(name)
			.setText(text)
			.setFont("ht")
			.setCharacterSize(40)
			.setJustification(gui::UIBase::Mid, gui::UIBase::Top)
			.setPosition(pos)
			.setSize(size);
		return area.path_get<gui::TextObject>(name);
	}
	gui::ButtonObject& addSimpleButton(gui::AreaObject& area, const std::string& name, const sf::String& text, sf::Vector2f pos, sf::Vector2f size) {
		area.path_get<gui::ButtonObject>(name)
			.setText(text)
			.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
			.setFont("ht")
			.setCharacterSize(40)
			.setPosition(pos)
			.setSize(size);
		return area.path_get<gui::ButtonObject>(name);
	}
	gui::ButtonObject& addAutoButton(gui::AreaObject& area, const std::string& name, const sf::String& text, sf::Vector2f pos) {
		area.path_get<gui::ButtonObject>(name)
			.setText(text)
			.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
			.setFont("ht")
			.setCharacterSize(40)
			.setSizeAuto()
			.setPosition(pos);
		return area.path_get<gui::ButtonObject>(name);
	}
	gui::ImageObject& addSimpleImage(gui::AreaObject& area, const std::string& name, const std::string& imageId, sf::Vector2f pos) {
		area.path_get<gui::ImageObject>(name)
			.setImageId(imageId)
			.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
			.setSizeAuto()
			.setPosition(pos);
		return area.path_get<gui::ImageObject>(name);
	}
	gui::OptionObject& addSimpleOption(gui::AreaObject& area, const std::string& name, const sf::String& text, sf::Vector2f pos, sf::Vector2f size) {
		area.path_get<gui::OptionObject>(name)
			.setText(text)
			.setFont("ht")
			.setCharacterSize(40)
			.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
			.setPosition(pos)
			.setSize(size);
		return area.path_get<gui::OptionObject>(name);
	}
	gui::OptionObject& addAutoOption(gui::AreaObject& area, const std::string& name, const sf::String& text, sf::Vector2f pos) {
		area.path_get<gui::OptionObject>(name)
			.setText(text)
			.setFont("ht")
			.setCharacterSize(40)
			.setSizeAuto()
			.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
			.setPosition(pos);
		return area.path_get<gui::OptionObject>(name);
	}
	gui::AreaObject& addSimpleArea(gui::AreaObject& area, const std::string& name, sf::Vector2f pos, sf::Vector2f size, sf::Vector2i drag = {0,0}, sf::Vector2i wheel = {0,0}) {
		area.path_get<gui::AreaObject>(name)
			.setScrollable(drag, wheel)
			.setStyle(style["stda2"], style["stda2"], style["stda2"])
			.setPosition(pos)
			.setSize(size);
		return area.path_get<gui::AreaObject>(name);
	}
	template<typename... Args>
	gui::AreaObject& addSelectBarX(gui::AreaObject& area, const std::string& name, sf::Vector2f pos, sf::Vector2f size, const std::string& defaultOption, Args... options) {
		std::vector<std::string> opts = { options... };
		float count = static_cast<float>(opts.size());
		float eachWidth = size.x / count;
		for (size_t i = 0; i < opts.size(); i++) {
			addSimpleOption(area, name + "." + opts[i], sf::String(opts[i] == "Left" ? L"左" : opts[i] == "Mid" ? L"中" : opts[i] == "Right" ? L"右" : opts[i] == "Top" ? L"上" : opts[i] == "Bottom" ? L"下" : sf::String(opts[i])), sf::Vector2f(eachWidth * static_cast<float>(i), 0), sf::Vector2f(eachWidth, size.y));
		}
		area.path_get<gui::AreaObject>(name)
			.setOption(defaultOption)
			.setScrollable(sf::Vector2i(false, false), sf::Vector2i(false, false))
			.setStyle(style["null"], style["null"], style["null"])
			.setPosition(pos)
			.setSize(size);
		return area.path_get<gui::AreaObject>(name);
	}
	gui::AreaObject& addJustificationBar(gui::AreaObject& area, const std::string& name, sf::Vector2f pos, sf::Vector2f size, const std::string& defaultX, const std::string& defaultY) {
		float eachWidth = size.x / 6;
		addSimpleOption(area, name + "X.Left", L"左", sf::Vector2f(eachWidth * 0, 0), sf::Vector2f(eachWidth, size.y));
		addSimpleOption(area, name + "X.Mid", L"中", sf::Vector2f(eachWidth * 1, 0), sf::Vector2f(eachWidth, size.y));
		addSimpleOption(area, name + "X.Right", L"右", sf::Vector2f(eachWidth * 2, 0), sf::Vector2f(eachWidth, size.y));
		area.path_get<gui::AreaObject>(name + "X")
			.setOption(defaultX)
			.setScrollable(sf::Vector2i(false, false), sf::Vector2i(false, false))
			.setStyle(style["null"], style["null"], style["null"])
			.setPosition(pos)
			.setSize(sf::Vector2f(size.x / 2, size.y));
		addSimpleOption(area, name + "Y.Top", L"上", sf::Vector2f(eachWidth * 0, 0), sf::Vector2f(eachWidth, size.y));
		addSimpleOption(area, name + "Y.Mid", L"中", sf::Vector2f(eachWidth * 1, 0), sf::Vector2f(eachWidth, size.y));
		addSimpleOption(area, name + "Y.Bottom", L"下", sf::Vector2f(eachWidth * 2, 0), sf::Vector2f(eachWidth, size.y));
		area.path_get<gui::AreaObject>(name + "Y")
			.setOption(defaultY)
			.setScrollable(sf::Vector2i(false, false), sf::Vector2i(false, false))
			.setStyle(style["null"], style["null"], style["null"])
			.setPosition(sf::Vector2f(pos.x + size.x / 2, pos.y))
			.setSize(sf::Vector2f(size.x / 2, size.y));
		return area.path_get<gui::AreaObject>(name + "X");
	}
	void addColorInputs4(gui::AreaObject& area, const std::string& prefix, int& line, float inputStartX, float fullWidth, float halfWindowWidth) {
		float eachWidth = (fullWidth - inputStartX) / 4;
		addSimpleInput(area, prefix + "R", gui::InputObject::Int, sf::Vector2f(inputStartX, static_cast<float>(40 * line)), sf::Vector2f(eachWidth, 40));
		addSimpleInput(area, prefix + "G", gui::InputObject::Int, sf::Vector2f(inputStartX + eachWidth, static_cast<float>(40 * line)), sf::Vector2f(eachWidth, 40));
		addSimpleInput(area, prefix + "B", gui::InputObject::Int, sf::Vector2f(inputStartX + eachWidth * 2, static_cast<float>(40 * line)), sf::Vector2f(eachWidth, 40));
		addSimpleInput(area, prefix + "A", gui::InputObject::Int, sf::Vector2f(inputStartX + eachWidth * 3, static_cast<float>(40 * line)), sf::Vector2f(eachWidth, 40));
	}
	void addColorInputSingle(gui::AreaObject& area, const std::string& prefix, int& line, float inputStartX, float fullWidth) {
		addSimpleInput(area, prefix, gui::InputObject::Int, sf::Vector2f(inputStartX, static_cast<float>(40 * line)), sf::Vector2f(fullWidth - inputStartX, 40));
	}
	void addPairInput(gui::AreaObject& area, const std::string& prefixX, const std::string& prefixY, sf::Vector2f pos, sf::Vector2f size, gui::InputObject::InputType type) {
		float halfWidth = size.x / 2;
		addSimpleInput(area, prefixX, type, pos, sf::Vector2f(halfWidth, size.y));
		addSimpleInput(area, prefixY, type, sf::Vector2f(pos.x + halfWidth, pos.y), sf::Vector2f(halfWidth, size.y));
	}
	void addBoolPairInput(gui::AreaObject& area, const std::string& prefixX, const std::string& prefixY, int& line, float inputStartX, float fullWidth) {
		float halfWidth = (fullWidth - inputStartX) / 2;
		addBoolInput(area, prefixX, sf::Vector2f(inputStartX, static_cast<float>(40 * line)), sf::Vector2f(halfWidth, 40));
		addBoolInput(area, prefixY, sf::Vector2f(inputStartX + halfWidth, static_cast<float>(40 * line)), sf::Vector2f(halfWidth, 40));
	}
	void addStateStyleInputs(gui::AreaObject& area, const std::string& stateName, int& line, float inputStartX, float fullWidth) {
		addTitleText(area, "Set" + stateName, sf::String(stateName == "Normal" ? L"普通格式" : stateName == "Over" ? L"待选格式" : L"选中格式"), sf::Vector2f(0, static_cast<float>(40 * line)), sf::Vector2f(fullWidth, 40));
		line++;
		addSimpleText(area, "Set" + stateName + "BackgroundColor", L"背景颜色:", sf::Vector2f(0, static_cast<float>(40 * line)));
		addColorInputs4(area, "Set" + stateName + "BackgroundColor", line, inputStartX, fullWidth, fullWidth);
		line++;
		addSimpleText(area, "Set" + stateName + "OutlineColor", L"轮廓颜色:", sf::Vector2f(0, static_cast<float>(40 * line)));
		addColorInputs4(area, "Set" + stateName + "OutlineColor", line, inputStartX, fullWidth, fullWidth);
		line++;
		addSimpleText(area, "Set" + stateName + "OutlineThickness", L"轮廓粗细:", sf::Vector2f(0, static_cast<float>(40 * line)));
		addSimpleInput(area, "Set" + stateName + "OutlineThickness", gui::InputObject::Int, sf::Vector2f(inputStartX, static_cast<float>(40 * line)), sf::Vector2f(fullWidth - inputStartX, 40));
	}
	void addTextStyleInputs(gui::AreaObject& area, const std::string& stateName, int& line, float inputStartX, float fullWidth) {
		addTitleText(area, "SetText" + stateName, sf::String(stateName == "Normal" ? L"普通格式" : stateName == "Over" ? L"待选格式" : L"选中格式"), sf::Vector2f(0, static_cast<float>(40 * line)), sf::Vector2f(fullWidth, 40));
		line++;
		addSimpleText(area, "SetText" + stateName + "BackgroundColor", L"背景颜色:", sf::Vector2f(0, static_cast<float>(40 * line)));
		addColorInputs4(area, "SetText" + stateName + "BackgroundColor", line, inputStartX, fullWidth, fullWidth);
		line++;
		addSimpleText(area, "SetText" + stateName + "OutlineColor", L"轮廓颜色:", sf::Vector2f(0, static_cast<float>(40 * line)));
		addColorInputs4(area, "SetText" + stateName + "OutlineColor", line, inputStartX, fullWidth, fullWidth);
	}
	void addImageStyleInputs(gui::AreaObject& area, const std::string& stateName, int& line, float inputStartX, float fullWidth) {
		addTitleText(area, "SetImage" + stateName, sf::String(stateName == "Normal" ? L"普通格式" : stateName == "Over" ? L"待选格式" : L"选中格式"), sf::Vector2f(0, static_cast<float>(40 * line)), sf::Vector2f(fullWidth, 40));
		line++;
		addSimpleText(area, "SetImage" + stateName + "Color", L"图片颜色:", sf::Vector2f(0, static_cast<float>(40 * line)));
		addColorInputs4(area, "SetImage" + stateName + "Color", line, inputStartX, fullWidth, fullWidth);
	}
	void addTypeLimitBar(gui::AreaObject& area, const std::string& name, sf::Vector2f pos, sf::Vector2f size, const std::string& defaultOption) {
		float eachWidth = size.x / 3;
		addSimpleOption(area, name + ".String", L"字符串", sf::Vector2f(eachWidth * 0, 0), sf::Vector2f(eachWidth, size.y));
		addSimpleOption(area, name + ".Int", L"整数", sf::Vector2f(eachWidth * 1, 0), sf::Vector2f(eachWidth, size.y));
		addSimpleOption(area, name + ".Float", L"浮点数", sf::Vector2f(eachWidth * 2, 0), sf::Vector2f(eachWidth, size.y));
		area.path_get<gui::AreaObject>(name)
			.setOption(defaultOption)
			.setScrollable(sf::Vector2i(false, false), sf::Vector2i(false, false))
			.setStyle(style["null"], style["null"], style["null"])
			.setPosition(pos)
			.setSize(size);
	}
	void addImageColorInputs(gui::AreaObject& area, const std::string& stateName, int& line, float inputStartX, float fullWidth) {
		addTitleText(area, "SetImage" + stateName, sf::String(stateName == "Normal" ? L"普通格式" : stateName == "Over" ? L"待选格式" : L"选中格式"), sf::Vector2f(0, static_cast<float>(40 * line)), sf::Vector2f(fullWidth, 40));
		line++;
		addSimpleText(area, "SetImage" + stateName + "Color", L"图片颜色:", sf::Vector2f(0, static_cast<float>(40 * line)));
		addColorInputs4(area, "SetImage" + stateName + "Color", line, inputStartX, fullWidth, fullWidth);
	}
	void loadAndAddButton(gui::AreaObject& area, const std::string& name, const std::string& iconPath, const sf::String& buttonText, sf::Vector2f pos) {
		imageManager.loadImage(name, iconPath);
		addSimpleImage(area, name, name, pos);
		addAutoButton(area, name, buttonText, pos);
	}
}
static void init() {
	style["stda1"].set(sf::Color::White, sf::Color(200, 200, 200), 2);
	style["stda2"].set(sf::Color(240, 240, 240), sf::Color(200, 200, 200), 2);
	style["stdbn"].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2);
	style["stdbo"].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2);
	style["stdbf"].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2);
	textStyle["stdtn"].set(sf::Color(0, 0, 0), sf::Color::Transparent);
	textStyle["stdto"].set(sf::Color(160, 160, 160), sf::Color::Transparent);
	textStyle["stdtf"].set(sf::Color(100, 100, 100), sf::Color::Transparent);
	style["null"];

	Main
		.setStyle(style["stda1"], style["stda1"], style["stda1"])
		.setPosition(sf::Vector2f(0, 0))
		.setSize(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));

	Init::loadAndAddButton(Main, "open", "resources/iconsource/open.png", L"    打开", sf::Vector2f(40, 0));
	Init::loadAndAddButton(Main, "save", "resources/iconsource/save.png", L"    保存", sf::Vector2f(40 + 124 * 1, 0));

	Init::loadAndAddButton(Main, "exportReflection", "resources/iconsource/export.png", L"    导出反射", sf::Vector2f(40 + 124 * 2, 0));

	Init::loadAndAddButton(Main, "about", "resources/iconsource/about.png", L"    关于", sf::Vector2f(static_cast<float>(windowWidth - 40 - 124), static_cast<float>(windowHeight - 40)));

	float halfWindowWidth = static_cast<float>((windowWidth - 40 * 2 - 10) / 2);
	Init::addSimpleArea(Main, "list", sf::Vector2f(40, 42), sf::Vector2f(halfWindowWidth, static_cast<float>(windowHeight - 42 * 2)), sf::Vector2i(0, 0), sf::Vector2i(1, 1));
	Init::addSimpleArea(Main, "settings", sf::Vector2f(static_cast<float>(halfWindowWidth + 40 + 10), 42), sf::Vector2f(halfWindowWidth, static_cast<float>(windowHeight - 42 * 2)), sf::Vector2i(0, 0), sf::Vector2i(1, 1));

	Init::loadAndAddButton(Main, "new", "resources/iconsource/new.png", L"    添加", sf::Vector2f(40, static_cast<float>(windowHeight - 40)));
	Init::loadAndAddButton(Main, "delete", "resources/iconsource/delete.png", L"    删除", sf::Vector2f(40 + 124 * 1, static_cast<float>(windowHeight - 40)));
	Init::loadAndAddButton(Main, "moveup", "resources/iconsource/moveup.png", L"    上移", sf::Vector2f(40 + 124 * 2, static_cast<float>(windowHeight - 40)));
	Init::loadAndAddButton(Main, "movedown", "resources/iconsource/movedown.png", L"    下移", sf::Vector2f(40 + 124 * 3, static_cast<float>(windowHeight - 40)));
	Init::loadAndAddButton(Main, "cut", "resources/iconsource/cut.png", L"    剪切", sf::Vector2f(40 + 124 * 4, static_cast<float>(windowHeight - 40)));
	Init::loadAndAddButton(Main, "copy", "resources/iconsource/copy.png", L"    复制", sf::Vector2f(40 + 124 * 5, static_cast<float>(windowHeight - 40)));
	Init::loadAndAddButton(Main, "paste", "resources/iconsource/paste.png", L"    粘贴", sf::Vector2f(40 + 124 * 6, static_cast<float>(windowHeight - 40)));



	int NewLine = 0;

	Init::addTitleText(New, "title", L"添加", sf::Vector2f(600 / 2, 20), sf::Vector2f(600, 40)).setCenter();
	NewLine++;

	imageManager.loadImage("area", "resources/iconsource/area.png");
	Init::addSimpleImage(New, "window", "area", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	Init::addAutoOption(New, "window", L"    新建窗口", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	NewLine++;

	Init::addSimpleImage(New, "area", "area", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	Init::addAutoOption(New, "area", L"    新建区域", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	NewLine++;

	imageManager.loadImage("input", "resources/iconsource/input.png");
	Init::addSimpleImage(New, "input", "input", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	Init::addAutoOption(New, "input", L"    新建输入框", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	NewLine++;

	imageManager.loadImage("button", "resources/iconsource/button.png");
	Init::addSimpleImage(New, "button", "button", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	Init::addAutoOption(New, "button", L"    新建按钮", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	NewLine++;

	imageManager.loadImage("option", "resources/iconsource/option.png");
	Init::addSimpleImage(New, "option", "option", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	Init::addAutoOption(New, "option", L"    新建选项", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	NewLine++;

	imageManager.loadImage("image", "resources/iconsource/image.png");
	Init::addSimpleImage(New, "image", "image", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	Init::addAutoOption(New, "image", L"    新建图片", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	NewLine++;

	imageManager.loadImage("text", "resources/iconsource/text.png");
	Init::addSimpleImage(New, "text", "text", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	Init::addAutoOption(New, "text", L"    新建文本", sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	NewLine++;
	New.path_get<gui::ButtonObject>("isSubText")
		.setText(L"□在当前区域子类中创建")
		.setTextStyle(textStyle["stdtn"], textStyle["stdto"], textStyle["stdtf"])
		.setFont("ht")
		.setCharacterSize(40)
		.setSizeAuto()
		.setJustification(gui::UIBase::Left, gui::UIBase::Mid)
		.setStyle(style["null"], style["null"], style["null"])
		.setPosition(sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	New.path_get<gui::TextObject>("isSub")
		.setText(L"√")
		.setFont("ht")
		.setCharacterSize(20)
		.setSizeAuto()
		.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
		.setPosition(sf::Vector2f(0 + 20, static_cast<float>(40 * NewLine) + 20))
		.setCenter();
	New.path_get<gui::TextObject>("isSubDisabledText")
		.setText(L"□在当前区域子类中创建")
		.setTextStyle(textStyle["stdto"], textStyle["stdto"], textStyle["stdto"])
		.setFont("ht")
		.setCharacterSize(40)
		.setSizeAuto()
		.setJustification(gui::UIBase::Left, gui::UIBase::Mid)
		.setPosition(sf::Vector2f(0, static_cast<float>(40 * NewLine)));
	NewLine++;
	Init::addTitleText(New, "name", L"名称：", sf::Vector2f(0, static_cast<float>(40 * NewLine)),sf::Vector2f(120,40));

	New.path_get<gui::InputObject>("name")
		.setStringTypeLimit(true, {}, { {L'A',L'Z'},{L'a',L'z'},{L'0',L'9'}})
		.setText(L"")
		.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
		.setFont("ht")
		.setCharacterSize(40)
		.setPosition(sf::Vector2f(120, static_cast<float>(40 * NewLine)))
		.setSize(sf::Vector2f(480, 40));
	NewLine++;

	Init::addSimpleButton(New, "ok", L"确定", sf::Vector2f(600 / 4, static_cast<float>(40 * NewLine) + 20), sf::Vector2f(300, 40)).setCenter();
	Init::addSimpleButton(New, "cancel", L"取消", sf::Vector2f(600 / 4 * 3, static_cast<float>(40 * NewLine) + 20), sf::Vector2f(300, 40)).setCenter();
	NewLine++;
	New
		.setOption()
		.setStyle(style["stda1"], style["stda1"], style["stda1"])
		.setPosition(sf::Vector2f(static_cast<float>(windowWidth) / 2, static_cast<float>(windowHeight) / 2))
		.setSize(sf::Vector2f(600, static_cast<float>(40 * NewLine)))
		.setCenter();


	//Open窗口初始化
	int OpenLine = 0;

	Init::addTitleText(Open, "title", L"打开文件", sf::Vector2f(600 / 2, 20), sf::Vector2f(600, 40)).setCenter();
	OpenLine++;

	Init::addSimpleOption(Open, "binary", L"Binary", sf::Vector2f(0, static_cast<float>(40 * OpenLine)), sf::Vector2f(300, 40));
	Init::addSimpleOption(Open, "json", L"JSON", sf::Vector2f(300, static_cast<float>(40 * OpenLine)), sf::Vector2f(300, 40));
	OpenLine++;

	Init::addSimpleText(Open, "filepath", L"文件名：", sf::Vector2f(0, static_cast<float>(40 * OpenLine)));

	Init::addSimpleInput(Open, "filepath", gui::InputObject::String, sf::Vector2f(160, static_cast<float>(40 * OpenLine)), sf::Vector2f(600 - 160, 40), L"");
	OpenLine++;

	Init::addSimpleButton(Open, "ok", L"确定", sf::Vector2f(600 / 4, static_cast<float>(40 * OpenLine) + 20), sf::Vector2f(300, 40)).setCenter();
	Init::addSimpleButton(Open, "cancel", L"取消", sf::Vector2f(600 / 4 * 3, static_cast<float>(40 * OpenLine) + 20), sf::Vector2f(300, 40)).setCenter();
	OpenLine++;
	Open
		.setOption()
		.setStyle(style["stda1"], style["stda1"], style["stda1"])
		.setPosition(sf::Vector2f(static_cast<float>(windowWidth) / 2, static_cast<float>(windowHeight) / 2))
		.setSize(sf::Vector2f(600, static_cast<float>(40 * OpenLine)))
		.setCenter();


	//Save窗口初始化
	int SaveLine = 0;

	Init::addTitleText(Save, "title", L"保存文件", sf::Vector2f(600 / 2, 20), sf::Vector2f(600, 40)).setCenter();
	SaveLine++;

	Init::addSimpleOption(Save, "binary", L"Binary", sf::Vector2f(0, static_cast<float>(40 * SaveLine)), sf::Vector2f(300, 40));
	Init::addSimpleOption(Save, "json", L"JSON", sf::Vector2f(300, static_cast<float>(40 * SaveLine)), sf::Vector2f(300, 40));
	SaveLine++;

	Init::addSimpleText(Save, "filepath", L"文件名：", sf::Vector2f(0, static_cast<float>(40 * SaveLine)));

	Init::addSimpleInput(Save, "filepath", gui::InputObject::String, sf::Vector2f(160, static_cast<float>(40 * SaveLine)), sf::Vector2f(600 - 160, 40), L"");
	SaveLine++;

	Init::addSimpleText(Save, "windowToSave", L"要保存的窗口：", sf::Vector2f(0, static_cast<float>(40 * SaveLine)));
	SaveLine++;

	Save.path_get<gui::AreaObject>("windowToSave")
		.setScrollable(sf::Vector2i(0, 1), sf::Vector2i(0, 1))
		.setStyle(style["stda1"], style["stda1"], style["stda1"])
		.setPosition(sf::Vector2f(0, static_cast<float>(40 * SaveLine)))
		.setSize(sf::Vector2f(600, static_cast<float>(40 * 5)));
	SaveLine += 5;
	Init::addSimpleButton(Save, "ok", L"确定", sf::Vector2f(600 / 4, static_cast<float>(40 * SaveLine) + 20), sf::Vector2f(300, 40)).setCenter();
	Init::addSimpleButton(Save, "cancel", L"取消", sf::Vector2f(600 / 4 * 3, static_cast<float>(40 * SaveLine) + 20), sf::Vector2f(300, 40)).setCenter();
	SaveLine++;
	Save
		.setOption()
		.setStyle(style["stda1"], style["stda1"], style["stda1"])
		.setPosition(sf::Vector2f(static_cast<float>(windowWidth) / 2, static_cast<float>(windowHeight) / 2))
		.setSize(sf::Vector2f(600, static_cast<float>(40 * SaveLine)))
		.setCenter();


	//Paste窗口初始化
	int PasteLine = 0;

	Init::addTitleText(Paste, "title", L"粘贴", sf::Vector2f(600 / 2, 20), sf::Vector2f(600, 40)).setCenter();
	PasteLine++;

	Paste.path_get<gui::ButtonObject>("isSubText")
		.setText(L"□粘贴到当前区域子类中")
		.setTextStyle(textStyle["stdtn"], textStyle["stdto"], textStyle["stdtf"])
		.setFont("ht")
		.setCharacterSize(40)
		.setSizeAuto()
		.setJustification(gui::UIBase::Left, gui::UIBase::Mid)
		.setStyle(style["null"], style["null"], style["null"])
		.setPosition(sf::Vector2f(0, static_cast<float>(40 * PasteLine)));
	Paste.path_get<gui::TextObject>("isSub")
		.setText(L"√")
		.setFont("ht")
		.setCharacterSize(20)
		.setSizeAuto()
		.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
		.setPosition(sf::Vector2f(0 + 20, static_cast<float>(40 * PasteLine) + 20))
		.setCenter();
	Paste.path_get<gui::TextObject>("isSubDisabledText")
		.setText(L"□粘贴到当前区域子类中")
		.setTextStyle(textStyle["stdto"], textStyle["stdto"], textStyle["stdto"])
		.setFont("ht")
		.setCharacterSize(40)
		.setSizeAuto()
		.setJustification(gui::UIBase::Left, gui::UIBase::Mid)
		.setPosition(sf::Vector2f(0, static_cast<float>(40 * PasteLine)));
	PasteLine++;

	Init::addSimpleButton(Paste, "ok", L"确定", sf::Vector2f(600 / 4, static_cast<float>(40 * PasteLine) + 20), sf::Vector2f(300, 40)).setCenter();
	Init::addSimpleButton(Paste, "cancel", L"取消", sf::Vector2f(600 / 4 * 3, static_cast<float>(40 * PasteLine) + 20), sf::Vector2f(300, 40)).setCenter();
	PasteLine++;
	Paste
		.setOption()
		.setStyle(style["stda1"], style["stda1"], style["stda1"])
		.setPosition(sf::Vector2f(static_cast<float>(windowWidth) / 2, static_cast<float>(windowHeight) / 2))
		.setSize(sf::Vector2f(600, static_cast<float>(40 * PasteLine)))
		.setCenter();


	///
	int UIBaseLine = 0;

	Init::addTitleText(settings::UIBase, "UIBaseSettings", L"基础设置", sf::Vector2f(0, static_cast<float>(40 * UIBaseLine)), sf::Vector2f(halfWindowWidth, 40));
	UIBaseLine++;

	Init::addSimpleText(settings::UIBase, "SetLeftUpPosition", L"左上坐标:", sf::Vector2f(0, static_cast<float>(40 * UIBaseLine)));
	Init::addPairInput(settings::UIBase, "SetLeftUpPositionX", "SetLeftUpPositionY", sf::Vector2f(200, static_cast<float>(40 * UIBaseLine)), sf::Vector2f(halfWindowWidth - 200, 40), gui::InputObject::Float);
	UIBaseLine++;

	Init::addSimpleText(settings::UIBase, "SetCenterPosition", L"中心坐标:", sf::Vector2f(0, static_cast<float>(40 * UIBaseLine)));
	Init::addPairInput(settings::UIBase, "SetCenterPositionX", "SetCenterPositionY", sf::Vector2f(200, static_cast<float>(40 * UIBaseLine)), sf::Vector2f(halfWindowWidth - 200, 40), gui::InputObject::Float);
	UIBaseLine++;

	Init::addSimpleText(settings::UIBase, "SetSize", L"大小(宽高):", sf::Vector2f(0, static_cast<float>(40 * UIBaseLine)));
	Init::addPairInput(settings::UIBase, "SetSizeX", "SetSizeY", sf::Vector2f(200, static_cast<float>(40 * UIBaseLine)), sf::Vector2f(halfWindowWidth - 200, 40), gui::InputObject::Float);
	UIBaseLine++;

	Init::addSimpleText(settings::UIBase, "SetShow", L"是否显示:", sf::Vector2f(0, static_cast<float>(40 * UIBaseLine)));
	Init::addBoolInput(settings::UIBase, "SetShow", sf::Vector2f(200, static_cast<float>(40 * UIBaseLine)), sf::Vector2f(halfWindowWidth - 200, 40));
	UIBaseLine++; UIBaseLine++;

	Init::addStateStyleInputs(settings::UIBase, "Normal", UIBaseLine, 200, halfWindowWidth);
	UIBaseLine++;
	Init::addStateStyleInputs(settings::UIBase, "Over", UIBaseLine, 200, halfWindowWidth);
	UIBaseLine++;
	Init::addStateStyleInputs(settings::UIBase, "Focus", UIBaseLine, 200, halfWindowWidth);
	UIBaseLine++; UIBaseLine++;

	//
	int AreaLine = UIBaseLine;

	Init::addTitleText(settings::Area, "AreaSettings", L"Area设置", sf::Vector2f(0, static_cast<float>(40 * AreaLine)), sf::Vector2f(halfWindowWidth, 40));
	AreaLine++;

	Init::addSimpleText(settings::Area, "SetOption", L"默认子Option:", sf::Vector2f(0, static_cast<float>(40 * AreaLine)));
	Init::addAlphaInput(settings::Area, "SetOption", sf::Vector2f(260, static_cast<float>(40 * AreaLine)), sf::Vector2f(halfWindowWidth - 260, 40));
	AreaLine++;

	Init::addSimpleText(settings::Area, "SetScrollableDrag", L"可拖拽:", sf::Vector2f(0, static_cast<float>(40 * AreaLine)));
	Init::addBoolPairInput(settings::Area, "SetScrollableDragX", "SetScrollableDragY", AreaLine, 160, halfWindowWidth);
	AreaLine++;

	Init::addSimpleText(settings::Area, "SetScrollableWheel", L"可滚轮:", sf::Vector2f(0, static_cast<float>(40 * AreaLine)));
	Init::addBoolPairInput(settings::Area, "SetScrollableWheelX", "SetScrollableWheelY", AreaLine, 160, halfWindowWidth);
	AreaLine++;
	//
	int TextLine = UIBaseLine;

	Init::addTitleText(settings::Text, "TextSettings", L"文字设置", sf::Vector2f(0, static_cast<float>(40 * TextLine)), sf::Vector2f(halfWindowWidth, 40));
	TextLine++;

	Init::addTextStyleInputs(settings::Text, "Normal", TextLine, 200, halfWindowWidth);
	TextLine++;
	Init::addTextStyleInputs(settings::Text, "Over", TextLine, 200, halfWindowWidth);
	TextLine++;
	Init::addTextStyleInputs(settings::Text, "Focus", TextLine, 200, halfWindowWidth);
	TextLine++; TextLine++;

	Init::addSimpleText(settings::Text, "SetText", L"文字:", sf::Vector2f(0, static_cast<float>(40 * TextLine)));
	Init::addStringInput(settings::Text, "SetText", sf::Vector2f(200, static_cast<float>(40 * TextLine)), sf::Vector2f(halfWindowWidth - 200, 40));
	TextLine++;

	Init::addSimpleText(settings::Text, "SetFont", L"字体:", sf::Vector2f(0, static_cast<float>(40 * TextLine)));
	Init::addStringInput(settings::Text, "SetFont", sf::Vector2f(200, static_cast<float>(40 * TextLine)), sf::Vector2f(halfWindowWidth - 200, 40));
	TextLine++;

	Init::addSimpleText(settings::Text, "SetCharacterSize", L"文字大小:", sf::Vector2f(0, static_cast<float>(40 * TextLine)));
	Init::addSimpleInput(settings::Text, "SetCharacterSize", gui::InputObject::Int, sf::Vector2f(200, static_cast<float>(40 * TextLine)), sf::Vector2f(halfWindowWidth - 200, 40));
	TextLine++;

	Init::addSimpleText(settings::Text, "SetLetterSpacing", L"字间距:", sf::Vector2f(0, static_cast<float>(40 * TextLine)));
	Init::addSimpleInput(settings::Text, "SetLetterSpacing", gui::InputObject::Float, sf::Vector2f(200, static_cast<float>(40 * TextLine)), sf::Vector2f(halfWindowWidth - 200, 40));
	TextLine++;

	Init::addSimpleText(settings::Text, "SetLineSpacing", L"行间距:", sf::Vector2f(0, static_cast<float>(40 * TextLine)));
	Init::addSimpleInput(settings::Text, "SetLineSpacing", gui::InputObject::Float, sf::Vector2f(200, static_cast<float>(40 * TextLine)), sf::Vector2f(halfWindowWidth - 200, 40));
	TextLine++;

	Init::addSimpleText(settings::Text, "SetJustification", L"对齐方式:", sf::Vector2f(0, static_cast<float>(40 * TextLine)));
	Init::addJustificationBar(settings::Text, "SetJustification", sf::Vector2f(200, static_cast<float>(40 * TextLine)), sf::Vector2f(halfWindowWidth - 200, 40), "Mid", "Mid");
	TextLine++;	TextLine++;

	//
	int ImageLine = UIBaseLine;

	Init::addTitleText(settings::Image, "ImageSettings", L"图片设置", sf::Vector2f(0, static_cast<float>(40 * ImageLine)), sf::Vector2f(halfWindowWidth, 40));
	ImageLine++;

	Init::addSimpleText(settings::Image, "SetImageId", L"图片ID:", sf::Vector2f(0, static_cast<float>(40 * ImageLine)));
	Init::addStringInput(settings::Image, "SetImageId", sf::Vector2f(200, static_cast<float>(40 * ImageLine)), sf::Vector2f(halfWindowWidth - 200, 40));
	ImageLine++;

	Init::addSimpleText(settings::Image, "SetScale", L"缩放:", sf::Vector2f(0, static_cast<float>(40 * ImageLine)));
	Init::addPairInput(settings::Image, "SetScaleX", "SetScaleY", sf::Vector2f(200, static_cast<float>(40 * ImageLine)), sf::Vector2f(halfWindowWidth - 200, 40), gui::InputObject::Float);
	ImageLine++;

	Init::addSimpleText(settings::Image, "SetJustification", L"对齐方式:", sf::Vector2f(0, static_cast<float>(40 * ImageLine)));
	Init::addJustificationBar(settings::Image, "SetJustification", sf::Vector2f(200, static_cast<float>(40 * ImageLine)), sf::Vector2f(halfWindowWidth - 200, 40), "Mid", "Mid");
	ImageLine++;	ImageLine++;

	Init::addImageStyleInputs(settings::Image, "Normal", ImageLine, 200, halfWindowWidth);
	ImageLine++;
	Init::addImageStyleInputs(settings::Image, "Over", ImageLine, 200, halfWindowWidth);
	ImageLine++;
	Init::addImageStyleInputs(settings::Image, "Focus", ImageLine, 200, halfWindowWidth);
	ImageLine++;	ImageLine++;

	int InputLine = TextLine;

	Init::addTitleText(settings::Input, "InputSettings", L"输入框设置", sf::Vector2f(0, static_cast<float>(40 * InputLine)), sf::Vector2f(halfWindowWidth, 40));
	InputLine++;

	Init::addSimpleText(settings::Input, "SetTypeLimit", L"输入类型:", sf::Vector2f(0, static_cast<float>(40 * InputLine)));
	Init::addTypeLimitBar(settings::Input, "SetTypeLimit", sf::Vector2f(200, static_cast<float>(40 * InputLine)), sf::Vector2f(halfWindowWidth - 200, 40), "String");
	InputLine++;

	Init::addSimpleText(settings::Input, "SetSizeLimit", L"长度限制:", sf::Vector2f(0, static_cast<float>(40 * InputLine)));
	Init::addSimpleInput(settings::Input, "SetSizeLimit", gui::InputObject::Int, sf::Vector2f(200, static_cast<float>(40 * InputLine)), sf::Vector2f(halfWindowWidth - 200, 40));
	InputLine++;	InputLine++;
}
namespace designer {
	gui::AreaObject data;
	VarianTmap<std::string>nameList;
	
	struct Clipboard {
		VarianTmap<std::string> nameListClip;
		VarianTmap<gui::UIBase> dataClip;
		std::string copiedItemName;
		bool hasData = false;
	} clipboard;
	
	void printDataRecursive(const gui::AreaObject& area, const std::string& prefix, int depth) {
		std::string indent(depth * 2, ' ');
		std::cout << indent << prefix << "[AreaObject] sub.size=" << area.sub.size() << std::endl;
		
		// 输出 Order 列表
		std::cout << indent << "  Order (" << area.sub.size() << " items):" << std::endl;
		for (auto it = area.sub.begin(); it != area.sub.end(); ++it) {
			std::string key = area.sub.find_key(it);
			bool isNull = (*it == nullptr);
			std::string typeStr = "unknown";
			if (!isNull) {
				if (area.sub.find<gui::AreaObject>(it)) typeStr = "AreaObject";
				else if (area.sub.find<gui::ButtonObject>(it)) typeStr = "ButtonObject";
				else if (area.sub.find<gui::ImageObject>(it)) typeStr = "ImageObject";
				else if (area.sub.find<gui::InputObject>(it)) typeStr = "InputObject";
				else if (area.sub.find<gui::OptionObject>(it)) typeStr = "OptionObject";
				else if (area.sub.find<gui::TextObject>(it)) typeStr = "TextObject";
			}
			std::cout << indent << "    key=\"" << key << "\" type=" << typeStr << " ptr=" << (isNull ? "nullptr" : "valid");
			if (!isNull) {
				sf::FloatRect rect = (*it)->getPosRect();
				std::cout << " posRect=(" << rect.position.x << "," << rect.position.y << ") size=(" << rect.size.x << "," << rect.size.y << ")";
			}
			std::cout << std::endl;
		}
		
		// 递归输出子 AreaObject
		for (auto it = area.sub.begin(); it != area.sub.end(); ++it) {
			if (auto* childArea = area.sub.find<gui::AreaObject>(it)) {
				std::string childKey = area.sub.find_key(it);
				printDataRecursive(*childArea, childKey, depth + 1);
			}
		}
	}
	
	void printDataDebug() {
		std::cout << "\n========== DATA DEBUG ==========" << std::endl;
		std::cout << "data.sub.size() = " << data.sub.size() << std::endl;
		printDataRecursive(data, "root", 0);
		std::cout << "================================\n" << std::endl;
	}
	
	void printNameListDebug() {
		std::cout << "--- nameList ---" << std::endl;
		for (auto it = nameList.begin(); it != nameList.end(); it++) {
			std::cout << "  " << *nameList.find<std::string>(it) << std::endl;
		}
		std::cout << "================\n" << std::endl;
	}
	
	void printClipboardDataRecursive(const gui::AreaObject& area, const std::string& prefix, int depth) {
		std::string indent(depth * 2, ' ');
		std::cout << indent << prefix << "[AreaObject] sub.size=" << area.sub.size() << std::endl;
		
		std::cout << indent << "  Order (" << area.sub.size() << " items):" << std::endl;
		for (auto it = area.sub.begin(); it != area.sub.end(); ++it) {
			std::string key = area.sub.find_key(it);
			bool isNull = (*it == nullptr);
			std::string typeStr = "unknown";
			if (!isNull) {
				if (area.sub.find<gui::AreaObject>(it)) typeStr = "AreaObject";
				else if (area.sub.find<gui::ButtonObject>(it)) typeStr = "ButtonObject";
				else if (area.sub.find<gui::ImageObject>(it)) typeStr = "ImageObject";
				else if (area.sub.find<gui::InputObject>(it)) typeStr = "InputObject";
				else if (area.sub.find<gui::OptionObject>(it)) typeStr = "OptionObject";
				else if (area.sub.find<gui::TextObject>(it)) typeStr = "TextObject";
			}
			std::cout << indent << "    key=\"" << key << "\" type=" << typeStr << " ptr=" << (isNull ? "nullptr" : "valid");
			if (!isNull) {
				sf::FloatRect rect = (*it)->getPosRect();
				std::cout << " posRect=(" << rect.position.x << "," << rect.position.y << ") size=(" << rect.size.x << "," << rect.size.y << ")";
			}
			std::cout << std::endl;
		}
		
		for (auto it = area.sub.begin(); it != area.sub.end(); ++it) {
			if (auto* childArea = area.sub.find<gui::AreaObject>(it)) {
				std::string childKey = area.sub.find_key(it);
				printClipboardDataRecursive(*childArea, childKey, depth + 1);
			}
		}
	}
	
	void printMainListDebug() {
		std::cout << "\n========== MAINLIST DEBUG ==========" << std::endl;
		gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
		for (auto it = mainList.sub.begin(); it != mainList.sub.end(); ++it) {
			std::string key = mainList.sub.find_key(it);
			bool isNull = (*it == nullptr);
			std::string typeStr = "unknown";
			if (!isNull) {
				if (mainList.sub.find<gui::ImageObject>(it)) typeStr = "ImageObject";
				else if (mainList.sub.find<gui::OptionObject>(it)) typeStr = "OptionObject";
				else if (mainList.sub.find<gui::AreaObject>(it)) typeStr = "AreaObject";
				else if (mainList.sub.find<gui::ButtonObject>(it)) typeStr = "ButtonObject";
				else if (mainList.sub.find<gui::InputObject>(it)) typeStr = "InputObject";
				else if (mainList.sub.find<gui::TextObject>(it)) typeStr = "TextObject";
			}
			std::cout << "  key=\"" << key << "\" type=" << typeStr;
			if (!isNull) {
				sf::FloatRect rect = (*it)->getPosRect();
				std::cout << " posRect=(" << rect.position.x << "," << rect.position.y << ") size=(" << rect.size.x << "," << rect.size.y << ")";
			}
			std::cout << std::endl;
		}
		std::cout << "==================================\n" << std::endl;
	}
	
	void printClipboardDebug() {
		std::cout << "\n========== CLIPBOARD DEBUG ==========" << std::endl;
		std::cout << "hasData: " << clipboard.hasData << std::endl;
		std::cout << "copiedItemName: " << clipboard.copiedItemName << std::endl;
		
		std::cout << "\n--- nameListClip (" << clipboard.nameListClip.size() << " items) ---" << std::endl;
		for (auto it = clipboard.nameListClip.begin(); it != clipboard.nameListClip.end(); it++) {
			std::string key = clipboard.nameListClip.find_key(it);
			std::string val = *clipboard.nameListClip.find<std::string>(it);
			std::cout << "  " << key << " -> " << val << std::endl;
		}
		
		std::cout << "\n--- dataClip (" << clipboard.dataClip.size() << " items) ---" << std::endl;
		for (auto it = clipboard.dataClip.begin(); it != clipboard.dataClip.end(); ++it) {
			std::string key = clipboard.dataClip.find_key(it);
			bool isNull = (*it == nullptr);
			std::string typeStr = "unknown";
			if (!isNull) {
				if (clipboard.dataClip.find<gui::AreaObject>(it)) typeStr = "AreaObject";
				else if (clipboard.dataClip.find<gui::ButtonObject>(it)) typeStr = "ButtonObject";
				else if (clipboard.dataClip.find<gui::ImageObject>(it)) typeStr = "ImageObject";
				else if (clipboard.dataClip.find<gui::InputObject>(it)) typeStr = "InputObject";
				else if (clipboard.dataClip.find<gui::OptionObject>(it)) typeStr = "OptionObject";
				else if (clipboard.dataClip.find<gui::TextObject>(it)) typeStr = "TextObject";
			}
			std::cout << "  key=\"" << key << "\" type=" << typeStr << " ptr=" << (isNull ? "nullptr" : "valid");
			if (!isNull) {
				sf::FloatRect rect = (*it)->getPosRect();
				std::cout << " posRect=(" << rect.position.x << "," << rect.position.y << ") size=(" << rect.size.x << "," << rect.size.y << ")";
			}
			std::cout << std::endl;
			
			if (!isNull) {
				if (auto* childArea = clipboard.dataClip.find<gui::AreaObject>(it)) {
					printClipboardDataRecursive(*childArea, key, 1);
				}
			}
		}
		std::cout << "====================================\n" << std::endl;
	}
	
	template<typename T>
	sf::String toStr(T val) { return sf::String(std::to_wstring(val)); }
	sf::String floatToStr(float val) {
		std::wstring s = std::to_wstring(val);
		//去掉末尾的0和小数点
		if (s.find(L'.') != std::wstring::npos) {
			while (s.back() == L'0') s.pop_back();
			if (s.back() == L'.') s.pop_back();
		}
		return sf::String(s);
	}
	//该参数为名称，用'-'连接；返回(type, name)，name用'-'连接
	std::pair<std::string, std::string> getType(const std::string& optionName) {
		return make_pair(optionName.substr(0, optionName.find(')') + 1), optionName.substr(optionName.find(')') + 1));
	}
	bool isAvailableName(const sf::String& name) {
		return (!name.isEmpty()) && !(name[0] >= L'0' && name[0] <= L'9');
	}
	//该参数为路径，用'_'连接；返回名称，用'-'连接
	std::string toMenuName(const std::string& dataPath) {
		std::string res = dataPath;
		for (size_t i = 0; i < dataPath.size(); i++) {
			if (res[i] == '_') {
				res[i] = '-';
			}
		}
		return res;
	}
	//该参数为名称，用'-'连接；返回路径，用'_'连接
	std::string toDataPath(const std::string& name) {
		std::string res = name;
		for (size_t i = 0; i < name.size(); i++) {
			if (res[i] == '-') {
				res[i] = '_';
			}
		}
		return res;
	}
	//预览相关的辅助函数
	namespace Preview {
		std::string currentWindowName; //当前正在预览的窗口名称
		//将指定的窗口数据复制到预览窗口（仅在切换窗口时调用）
		//该参数为名称，用'-'连接
		void copyWindowToPreview(const std::string& windowName, gui::AreaObject& previewData) {
			if (currentWindowName == windowName) return;
			//清空现有的预览数据
			previewData.sub.clear();
			//从data中找到对应的窗口并复制到预览
			if (auto* windowPtr = data.sub.find_named<gui::AreaObject>(windowName)) {
				//同步窗口的UIBase属性到preview窗口
				previewData.setPosition(windowPtr->getPosition());
				previewData.setSize(windowPtr->getSize());
				for (int i = 0; i < 3; i++) {
					previewData.style(i) = windowPtr->style(i);
				}
				//将窗口的子元素合并到预览数据
				previewData.sub.merge(windowPtr->sub);
			}
			currentWindowName = windowName;
		}
		//根据dataPath在preview中找到对应的对象并应用修改
		template<typename T>
		void syncUIBase(T* src, T* dst) {
			dst->setPosition(src->getPosition());
			dst->setSize(src->getSize());
			dst->setShow(src->getShow());
			for (int i = 0; i < 3; i++) {
				dst->style(i) = src->style(i);
			}
		}
		template<typename T>
		void syncText(T* src, T* dst) {
			syncUIBase(src, dst);
			dst->setText(src->getText());
			dst->setFont(src->getFont());
			dst->setCharacterSize(src->getCharacterSize());
			dst->setSpacing(src->getLetterSpacing(), src->getLineSpacing());
			auto just = src->getJustification();
			dst->setJustification(static_cast<gui::UIBase::Justification>(just.x),
								   static_cast<gui::UIBase::Justification>(just.y));
			for (int i = 0; i < 3; i++) {
				dst->textStyle(i) = src->textStyle(i);
			}
		}
		//从dataPath中提取窗口名
		//该参数为路径，用'_'连接；返回名称，用'-'连接
		std::string extractWindowName(const std::string& dataPath) {
			std::string::size_type dashPos = dataPath.find('_');
			return (dashPos != std::string::npos) ? dataPath.substr(0, dashPos) : dataPath;
		}//该参数type为类型标识；dataPath为路径，用'_'连接
		void applyToPreview(const std::string& type, const std::string& dataPath) {
			std::cout << "  [applyToPreview] type=" << type << " dataPath=" << dataPath << " currentWindow=" << currentWindowName << std::endl;
			if (currentWindowName == "") return;
			gui::AreaObject& previewData = previewManager.window("preview");
			std::string windowName = extractWindowName(dataPath);
			//如果dataPath是窗口本身，直接同步previewData
			if (dataPath == windowName) {
				if (type == attr::designer::type::area) {
					auto* src = data.path_find<gui::AreaObject>(dataPath);
					if (!src) return;
					syncUIBase(src, &previewData);
					previewData.setOption(src->getOption());
				}
				return;
			}
			//否则在子元素中查找（去掉窗口名前缀）
			std::string subPath = dataPath.substr(windowName.length() + 1);
			if (type == attr::designer::type::area) {
				auto* src = data.path_find<gui::AreaObject>(dataPath);
				auto* dst = previewData.path_find<gui::AreaObject>(subPath);
				std::cout << "    src=" << (src ? "found" : "NULL") << " dst=" << (dst ? "found" : "NULL") << std::endl;
				if (!src || !dst) return;
				syncUIBase(src, dst);
				dst->setOption(src->getOption());
			}
			else if (type == attr::designer::type::button) {
				auto* src = data.path_find<gui::ButtonObject>(dataPath);
				auto* dst = previewData.path_find<gui::ButtonObject>(subPath);
				std::cout << "    src=" << (src ? "found" : "NULL") << " dst=" << (dst ? "found" : "NULL") << std::endl;
				if (!src || !dst) return;
				syncText(src, dst);
			}
			else if (type == attr::designer::type::image) {
				auto* src = data.path_find<gui::ImageObject>(dataPath);
				auto* dst = previewData.path_find<gui::ImageObject>(subPath);
				std::cout << "    src=" << (src ? "found" : "NULL") << " dst=" << (dst ? "found" : "NULL") << std::endl;
				if (!src || !dst) return;
				syncUIBase(src, dst);
				dst->setImageId(src->getImageId());
				dst->setScale(src->getScale());
				dst->setImageColor(src->getImageColor(0), src->getImageColor(1), src->getImageColor(2));
				auto just = src->getJustification();
				dst->setJustification(static_cast<gui::UIBase::Justification>(just.x),
									   static_cast<gui::UIBase::Justification>(just.y));
			}
			else if (type == attr::designer::type::input) {
				auto* src = data.path_find<gui::InputObject>(dataPath);
				auto* dst = previewData.path_find<gui::InputObject>(subPath);
				std::cout << "    src=" << (src ? "found" : "NULL") << " dst=" << (dst ? "found" : "NULL") << std::endl;
				if (!src || !dst) return;
				syncText(src, dst);
				dst->setSizeLimit(src->getSizeLimit());
				dst->setTypeLimit(static_cast<gui::InputObject::InputType>(src->getTypeLimit()));
			}
			else if (type == attr::designer::type::option) {
				auto* src = data.path_find<gui::OptionObject>(dataPath);
				auto* dst = previewData.path_find<gui::OptionObject>(subPath);
				std::cout << "    src=" << (src ? "found" : "NULL") << " dst=" << (dst ? "found" : "NULL") << std::endl;
				if (!src || !dst) return;
				syncText(src, dst);
			}
			else if (type == attr::designer::type::text) {
				auto* src = data.path_find<gui::TextObject>(dataPath);
				auto* dst = previewData.path_find<gui::TextObject>(subPath);
				std::cout << "    src=" << (src ? "found" : "NULL") << " dst=" << (dst ? "found" : "NULL") << std::endl;
				if (!src || !dst) return;
				syncText(src, dst);
			}
		}
		//从preview窗口中删除对应的元素
		//该参数为路径，用'_'连接
		void removeFromPreview(const std::string& dataPath) {
			std::cout << "[PREVIEW DEBUG] removeFromPreview dataPath: " << dataPath << std::endl;
			if (currentWindowName == "") return;
			gui::AreaObject& previewData = previewManager.window("preview");
			//提取路径的第一段作为窗口名，后面的作为子路径
			std::string::size_type dashPos = dataPath.find('_');
			std::string subPath = (dashPos != std::string::npos) ? dataPath.substr(dashPos + 1) : "";
			std::cout << "[PREVIEW DEBUG] dashPos: " << dashPos << ", subPath: " << subPath << std::endl;
			if (subPath == "") {
				//删除的是窗口本身，重新拷贝
				currentWindowName = "";
			}
			else {
				//收集所有要删除的名称（当前元素 + 所有子元素）
				std::vector<std::string> toErase;
				toErase.push_back(subPath);
				auto* nameListPtr = &designer::nameList;
				std::cout << "[PREVIEW DEBUG] searching for: " << dataPath << " in nameList" << std::endl;
				auto iter = nameListPtr->find_order_named<std::string>(dataPath);
				std::cout << "[PREVIEW DEBUG] iter found: " << (iter != nameListPtr->end() ? "yes" : "no") << std::endl;
				if (iter != nameListPtr->end()) {
					iter++;
					while (iter != nameListPtr->end()) {
						std::string& nextName = *nameListPtr->find<std::string>(iter);
						if (nextName.find(dataPath) != 0) break;
						//提取子路径
						std::string childSubPath = nextName.substr(dashPos + 1);
						toErase.push_back(childSubPath);
						iter++;
					}
				}
				//从preview中删除
				for (const auto& name : toErase) {
					if (auto* ptr = previewData.sub.find_named<gui::OptionObject>(name)) {
						previewData.sub.erase(ptr);
					}
					if (auto* ptr = previewData.sub.find_named<gui::ImageObject>(name)) {
						previewData.sub.erase(ptr);
					}
					if (auto* ptr = previewData.sub.find_named<gui::ButtonObject>(name)) {
						previewData.sub.erase(ptr);
					}
					if (auto* ptr = previewData.sub.find_named<gui::InputObject>(name)) {
						previewData.sub.erase(ptr);
					}
					if (auto* ptr = previewData.sub.find_named<gui::TextObject>(name)) {
						previewData.sub.erase(ptr);
					}
					if (auto* ptr = previewData.sub.find_named<gui::AreaObject>(name)) {
						previewData.sub.erase(ptr);
					}
				}
			}
		}
		//窗口大小改变时同步preview窗口大小
		void syncWindowSize() {
			if (currentWindowName == "") return;
			if (auto* windowPtr = data.sub.find_named<gui::AreaObject>(currentWindowName)) {
				sf::Vector2f pos = windowPtr->getPosition();
				sf::Vector2f size = windowPtr->getSize();
				unsigned int newWidth = std::max(static_cast<unsigned int>(windowWidth), static_cast<unsigned int>(pos.x + size.x));
				unsigned int newHeight = std::max(static_cast<unsigned int>(windowHeight), static_cast<unsigned int>(pos.y + size.y));
				if (static_cast<unsigned int>(preview.getSize().x) != newWidth || static_cast<unsigned int>(preview.getSize().y) != newHeight) {
					preview.setSize(sf::Vector2u(newWidth, newHeight));
					preview.setView(sf::View(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(static_cast<float>(newWidth), static_cast<float>(newHeight)))));
				}
			}
		}
	}
	//传入参数使用的是名称的函数，名称即用'-'连接的字符串，操作的是mainList中的对象
	namespace Name {
		//该参数为名称，用'-'连接；返回层级数
		size_t countLevel(const std::string& name) {
			size_t count = 0;
			for (size_t i = 0; i < name.size(); i++) {
				if (name[i] == '-') {
					count++;
				}
			}
			return count;
		}
		//该参数为名称，用'-'连接；返回窗口名，用'-'连接
		std::string getWindowName(const std::string& name) {
			return name.substr(0, name.find('-'));
		}
		//该参数为名称，用'-'连接；返回(父名称, 自身名)，均用'-'连接
		std::pair<std::string, std::string> getFatherName(const std::string& name) {
			size_t lastPos = name.find_last_of('-');
			if (lastPos != std::string::npos) {
				return { name.substr(0, lastPos), name.substr(lastPos + 1) };
			}
			return { "",name };
		}
		//father != child
		//该参数均为名称，用'-'连接
		bool isFather(const std::string& fatherName, const std::string& childName) {
			if (childName.size() <= fatherName.size())return false;
			for (size_t i = 0; i < fatherName.size(); i++) {
				if (fatherName[i] != childName[i])return false;
			}
			if (childName[fatherName.size()] == '-')
				return true;
			else return false;
		}
			namespace createHelper {
				//该参数name为名称，用'-'连接；返回名称，用'-'连接
				std::string getNextListOptionName(gui::AreaObject& mainList, std::string type, std::string name, bool isSub) {
					static int x = 0;
				if (name == "")return {};
				std::string optionName = type + name;
				auto iter = std::next(nameList.find_order_named<std::string>(optionName));
				if (!isSub) {
					while (iter!=nameList.end()) {
						std::string& nextOptionName = *nameList.find<std::string>(iter);
						auto [_, nextName] = getType(nextOptionName);
						std::cout << "father = " << name << " child = " << nextName<<" isFather = ";
						if (!isFather(name, nextName)) {
							std::cout << 0 << std::endl;
							return nextOptionName;
						}
						std::cout << 1 << std::endl;
						iter++;
					}
					return {};
				}
				else {
					if (iter != nameList.end()) {
						return **iter;
					}
					else return {};
				}
			}
			//该参数为名称，用'-'连接
			void moveDown(const std::string& optionName, const gui::AreaObject& mainList) {
				auto iter = mainList.sub.find_order_named<gui::ImageObject>(optionName);
				while (iter != mainList.sub.end()) {
					(*iter)->setPosition((*iter)->getPosition() + sf::Vector2f(0.f, 40.f));
					iter++;
				}
			}
		}
		//移动相关的辅助函数（改变渲染顺序）
		namespace moveHelper {
			void printNameList() {
				std::cout << "=== nameList ===" << std::endl;
				for (auto it = designer::nameList.begin(); it != designer::nameList.end(); it++) {
					std::cout << "  " << *designer::nameList.find<std::string>(it) << std::endl;
				}
				std::cout << "================" << std::endl;
			}
			//判断两个路径是否是同一级（同一个父路径）
			//该参数均为路径，用'_'连接
			bool isSameLevel(const std::string& path1, const std::string& path2) {
				auto father1 = designer::Name::getFatherName(path1);
				auto father2 = designer::Name::getFatherName(path2);
				return father1.first == father2.first;
			}
			
			//该参数为名称，用'-'连接；返回名称列表，用'-'连接
			std::vector<std::string> collectItemsToMove(const std::string& optionName) {
				auto [type, path] = designer::getType(optionName);//type为类型标识，path为名称，用'-'连接
				std::vector<std::string> items;
				items.push_back(optionName);
				
				if (type == attr::designer::type::area) {
					auto iter = std::next(designer::nameList.find_order_named<std::string>(optionName));
					while (iter != designer::nameList.end()) {
						std::string& nextName = *designer::nameList.find<std::string>(iter);//该变量为名称，用'-'连接
						auto [_, nextPath] = designer::getType(nextName);//nextPath为名称，用'-'连接
						if (!designer::Name::isFather(path, nextPath)) break;
						items.push_back(nextName);
						iter++;
					}
				}
				return items;
			}
			
			//从mainList.sub中提取指定项到剪贴板
			//该参数items为名称列表，用'-'连接
			void extractToClip(gui::AreaObject& mainList, const std::vector<std::string>& items, VarianTmap<gui::UIBase>& subClip) {
				for (const auto& itemName : items) {
					auto img = mainList.sub.extract_named<gui::ImageObject>(itemName);
					subClip.emplace_named<gui::ImageObject>(subClip.end(), itemName, std::move(img));
					auto opt = mainList.sub.extract_named<gui::OptionObject>(itemName);
					subClip.emplace_named<gui::OptionObject>(subClip.end(), itemName, std::move(opt));
				}
			}
			
			void repositionItems(gui::AreaObject& mainList) {
				float startY = 0.f;
				for (auto it = mainList.sub.begin(); it != mainList.sub.end(); it++) {
					if (auto* img = mainList.sub.find<gui::ImageObject>(it)) {
						img->setPosition(sf::Vector2f(img->getPosition().x, startY));
					}
					if (auto* opt = mainList.sub.find<gui::OptionObject>(it)) {
						opt->setPosition(sf::Vector2f(opt->getPosition().x, startY));
						startY += 40.f;
					}
				}
			}
				
			//该参数为名称，用'-'连接
			void moveUp(const std::string& optionName) {
				gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
				
				auto [dataType, dataPath] = designer::getType(optionName);//dataType为类型标识，dataPath为名称，用'-'连接
				auto nameFatherPair = designer::Name::getFatherName(dataPath);//nameFatherPair.first为父名称，nameFatherPair.second为自身名，均用'-'连接
				std::string nameParentPath = nameFatherPair.first;//该变量为父名称，用'-'连接
				std::string itemName = nameFatherPair.second;//该变量为自身名，用'-'连接
				
				if (nameParentPath != "") {
					std::string dataParentPath = designer::toDataPath(nameParentPath);//该变量为父路径，用'_'连接
					gui::AreaObject& parent = data.path_at<gui::AreaObject>(dataParentPath);
					
					if (dataType == attr::designer::type::button) {
						auto iter = parent.sub.find_order_named<gui::ButtonObject>(itemName);
						if (iter == parent.sub.begin()) return;
						auto iterPrev = std::prev(iter);
						auto dataClip = parent.sub.extract<gui::ButtonObject>(iter);
						parent.sub.insert_named(iterPrev, itemName, std::move(dataClip));
					}
					else if (dataType == attr::designer::type::image) {
						auto iter = parent.sub.find_order_named<gui::ImageObject>(itemName);
						if (iter == parent.sub.begin()) return;
						auto iterPrev = std::prev(iter);
						auto dataClip = parent.sub.extract<gui::ImageObject>(iter);
						parent.sub.insert_named(iterPrev, itemName, std::move(dataClip));
					}
					else if (dataType == attr::designer::type::input) {
						auto iter = parent.sub.find_order_named<gui::InputObject>(itemName);
						if (iter == parent.sub.begin()) return;
						auto iterPrev = std::prev(iter);
						auto dataClip = parent.sub.extract<gui::InputObject>(iter);
						parent.sub.insert_named(iterPrev, itemName, std::move(dataClip));
					}
					else if (dataType == attr::designer::type::option) {
						auto iter = parent.sub.find_order_named<gui::OptionObject>(itemName);
						if (iter == parent.sub.begin()) return;
						auto iterPrev = std::prev(iter);
						auto dataClip = parent.sub.extract<gui::OptionObject>(iter);
						parent.sub.insert_named(iterPrev, itemName, std::move(dataClip));
					}
					else if (dataType == attr::designer::type::text) {
						auto iter = parent.sub.find_order_named<gui::TextObject>(itemName);
						if (iter == parent.sub.begin()) return;
						auto iterPrev = std::prev(iter);
						auto dataClip = parent.sub.extract<gui::TextObject>(iter);
						parent.sub.insert_named(iterPrev, itemName, std::move(dataClip));
					}
					else if (dataType == attr::designer::type::area) {
						auto iter = parent.sub.find_order_named<gui::AreaObject>(itemName);
						if (iter == parent.sub.begin()) return;
						auto iterPrev = std::prev(iter);
						auto dataClip = parent.sub.extract<gui::AreaObject>(iter);
						parent.sub.insert_named(iterPrev, itemName, std::move(dataClip));
					}
				}
				
				std::vector<std::string> items = collectItemsToMove(optionName);
				auto [currentType, currentPath] = designer::getType(optionName);//currentType为类型标识，currentPath为名称，用'-'连接
				
				auto imgIter = mainList.sub.find_order_named<gui::ImageObject>(items.front());
				if (imgIter == mainList.sub.end() || imgIter == mainList.sub.begin()) {
					return;
				}
				
				std::string prevSiblingKey = "";//该变量为名称，用'-'连接
				auto nameFatherPair2 = designer::Name::getFatherName(currentPath);
				std::string nameParentPath2 = nameFatherPair2.first;//该变量为父名称，用'-'连接
				auto nameIter = designer::nameList.find_order_named<std::string>(optionName);
				while (nameIter != designer::nameList.begin()) {
					nameIter = std::prev(nameIter);
					std::string& checkName = *designer::nameList.find<std::string>(nameIter);
					auto [checkType, checkPath] = designer::getType(checkName);
					if (designer::Name::isFather(currentPath, checkPath)) continue;
					auto checkFather = designer::Name::getFatherName(checkPath);
					if (checkFather.first == nameParentPath2) {
						prevSiblingKey = checkName;
						break;
					}
					if (checkPath == nameParentPath2 || designer::Name::isFather(checkPath, currentPath)) {
						return;
					}
				}
				
				if (prevSiblingKey == "") return;
				
				VarianTmap<std::string> nameListClip;
				VarianTmap<gui::UIBase> subClip;
				
				for (const auto& itemName : items) {
					std::string val = designer::nameList.extract_named<std::string>(itemName);
					nameListClip.push_back_named(itemName, val);
				}
				
				extractToClip(mainList, items, subClip);
				
				auto targetImgIter = mainList.sub.find_order_named<gui::ImageObject>(prevSiblingKey);
				mainList.sub.merge(targetImgIter, subClip);
				
				auto nameListInsertPos = designer::nameList.find_order_named<std::string>(prevSiblingKey);
				designer::nameList.merge(nameListInsertPos, nameListClip);
				
				repositionItems(mainList);
			}
			
			//该参数为名称，用'-'连接
			void moveDown(const std::string& optionName) {
				gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
				
				auto [dataType, dataPath] = designer::getType(optionName);
				auto nameFatherPair = designer::Name::getFatherName(dataPath);
				std::string nameParentPath = nameFatherPair.first;
				std::string itemName = nameFatherPair.second;
				
				if (nameParentPath != "") {
					std::string dataParentPath = designer::toDataPath(nameParentPath);
					gui::AreaObject& parent = data.path_at<gui::AreaObject>(dataParentPath);
					
					if (dataType == attr::designer::type::button) {
						auto iter = parent.sub.find_order_named<gui::ButtonObject>(itemName);
						auto endIter = parent.sub.end();
						if (iter == endIter || std::next(iter) == endIter) return;
						auto iterNext = std::next(iter, 2);
						auto dataClip = parent.sub.extract<gui::ButtonObject>(iter);
						parent.sub.insert_named(iterNext, itemName, std::move(dataClip));
					}
					else if (dataType == attr::designer::type::image) {
						auto iter = parent.sub.find_order_named<gui::ImageObject>(itemName);
						auto endIter = parent.sub.end();
						if (iter == endIter || std::next(iter) == endIter) return;
						auto iterNext = std::next(iter, 2);
						auto dataClip = parent.sub.extract<gui::ImageObject>(iter);
						parent.sub.insert_named(iterNext, itemName, std::move(dataClip));
					}
					else if (dataType == attr::designer::type::input) {
						auto iter = parent.sub.find_order_named<gui::InputObject>(itemName);
						auto endIter = parent.sub.end();
						if (iter == endIter || std::next(iter) == endIter) return;
						auto iterNext = std::next(iter, 2);
						auto dataClip = parent.sub.extract<gui::InputObject>(iter);
						parent.sub.insert_named(iterNext, itemName, std::move(dataClip));
					}
					else if (dataType == attr::designer::type::option) {
						auto iter = parent.sub.find_order_named<gui::OptionObject>(itemName);
						auto endIter = parent.sub.end();
						if (iter == endIter || std::next(iter) == endIter) return;
						auto iterNext = std::next(iter, 2);
						auto dataClip = parent.sub.extract<gui::OptionObject>(iter);
						parent.sub.insert_named(iterNext, itemName, std::move(dataClip));
					}
					else if (dataType == attr::designer::type::text) {
						auto iter = parent.sub.find_order_named<gui::TextObject>(itemName);
						auto endIter = parent.sub.end();
						if (iter == endIter || std::next(iter) == endIter) return;
						auto iterNext = std::next(iter, 2);
						auto dataClip = parent.sub.extract<gui::TextObject>(iter);
						parent.sub.insert_named(iterNext, itemName, std::move(dataClip));
					}
					else if (dataType == attr::designer::type::area) {
						auto iter = parent.sub.find_order_named<gui::AreaObject>(itemName);
						auto endIter = parent.sub.end();
						if (iter == endIter || std::next(iter) == endIter) return;
						auto iterNext = std::next(iter, 2);
						auto dataClip = parent.sub.extract<gui::AreaObject>(iter);
						parent.sub.insert_named(iterNext, itemName, std::move(dataClip));
					}
				}
				
				std::vector<std::string> items = collectItemsToMove(optionName);
				auto [currentType, currentPath] = designer::getType(optionName);//currentType为类型标识，currentPath为名称，用'-'连接
				
				auto optIter = mainList.sub.find_order_named<gui::OptionObject>(items.back());
				if (optIter == mainList.sub.end()) return;
				
				std::string nextSiblingKey = "";//该变量为名称，用'-'连接
				auto nameFatherPair2 = designer::Name::getFatherName(currentPath);
				std::string nameParentPath2 = nameFatherPair2.first;//该变量为父名称，用'-'连接
				auto nameIter = designer::nameList.find_order_named<std::string>(items.back());
				nameIter = std::next(nameIter);
				while (nameIter != designer::nameList.end()) {
					std::string& checkName = *designer::nameList.find<std::string>(nameIter);
					auto [checkType, checkPath] = designer::getType(checkName);
					if (designer::Name::isFather(currentPath, checkPath)) {
						nameIter = std::next(nameIter);
						continue;
					}
					auto checkFather = designer::Name::getFatherName(checkPath);
					if (checkFather.first == nameParentPath2) {
						nextSiblingKey = checkName;
						break;
					}
					break;
				}
				
				if (nextSiblingKey == "") return;
				
				auto [nextSiblingType, nextSiblingPath] = designer::getType(nextSiblingKey);
				std::string insertAfterKey = nextSiblingKey;//该变量为名称，用'-'连接
				if (nextSiblingType == attr::designer::type::area) {
					auto iter = std::next(designer::nameList.find_order_named<std::string>(nextSiblingKey));
					while (iter != designer::nameList.end()) {
						std::string& checkName = *designer::nameList.find<std::string>(iter);//该变量为名称，用'-'连接
						auto [_, checkPath] = designer::getType(checkName);//checkPath为名称，用'-'连接
						if (!designer::Name::isFather(nextSiblingPath, checkPath)) break;
						insertAfterKey = checkName;
						iter++;
					}
				}
				
				VarianTmap<std::string> nameListClip;
				VarianTmap<gui::UIBase> subClip;
				
				for (const auto& itemName : items) {
					std::string val = designer::nameList.extract_named<std::string>(itemName);//该变量为名称，用'-'连接
					nameListClip.push_back_named(itemName, val);
				}
				
				extractToClip(mainList, items, subClip);
				
				auto targetOptIter = mainList.sub.find_order_named<gui::OptionObject>(insertAfterKey);
				auto insertPos = std::next(targetOptIter);
				mainList.sub.merge(insertPos, subClip);
				
				auto nameListOptIter = designer::nameList.find_order_named<std::string>(insertAfterKey);
				auto nameListInsertPos = std::next(nameListOptIter);
				designer::nameList.merge(nameListInsertPos, nameListClip);
				
				repositionItems(mainList);
			}
			//该参数为名称，用'-'连接
			void copyToClipboard(const std::string& optionName) {
				auto [type, path] = designer::getType(optionName);//type为类型标识，path为名称，用'-'连接
				
				designer::clipboard.nameListClip.clear();
				designer::clipboard.dataClip.clear();
				
				std::vector<std::string> items;
				items.push_back(optionName);
				
				if (type == attr::designer::type::area) {
					auto iter = std::next(designer::nameList.find_order_named<std::string>(optionName));
					while (iter != designer::nameList.end()) {
						std::string& nextName = *designer::nameList.find<std::string>(iter);//该变量为名称，用'-'连接
						auto [_, nextPath] = designer::getType(nextName);//nextPath为名称，用'-'连接
						if (!designer::Name::isFather(path, nextPath)) break;
						items.push_back(nextName);
						iter++;
					}
				}
				
				std::string fatherPath = designer::Name::getFatherName(path).first;//该变量为父名称，用'-'连接
			
				for (const auto& itemName : items) {
					auto [itemType, itemPath] = designer::getType(itemName);//itemType为类型标识，itemPath为名称，用'-'连接
					std::string relativeName;//该变量为名称，用'-'连接
					if (fatherPath.empty()) {
						relativeName = itemPath;
					}
					else {
						std::string prefix = fatherPath + '-';//该变量为名称前缀，用'-'连接
						relativeName = itemPath.substr(prefix.size());
					}
					std::string relativeNameWithType = itemType + relativeName;//该变量为名称，用'-'连接
					designer::clipboard.nameListClip.push_back_named(relativeNameWithType, relativeNameWithType);
				}
				
				auto [rootType, rootPath] = designer::getType(optionName);//rootType为类型标识，rootPath为名称，用'-'连接
				std::string rootName = designer::Name::getFatherName(rootPath).second;//该变量为名称，用'-'连接
				std::string rootFatherPath = designer::Name::getFatherName(rootPath).first;//该变量为名称，用'-'连接
				std::string rootDataFatherPath = designer::toDataPath(rootFatherPath);//该变量为路径，用'_'连接
				gui::AreaObject* rootFather = rootFatherPath.empty() ? &designer::data : &designer::data.path_at<gui::AreaObject>(rootDataFatherPath);
				
				if (rootType == attr::designer::type::area) {
					if (auto* obj = rootFather->sub.find_named<gui::AreaObject>(rootName)) {
						designer::clipboard.dataClip.emplace_named<gui::AreaObject>(designer::clipboard.dataClip.end(), rootName, *obj);
					}
				}
				else if (rootType == attr::designer::type::button) {
					if (auto* obj = rootFather->sub.find_named<gui::ButtonObject>(rootName)) {
						designer::clipboard.dataClip.emplace_named<gui::ButtonObject>(designer::clipboard.dataClip.end(), rootName, *obj);
					}
				}
				else if (rootType == attr::designer::type::image) {
					if (auto* obj = rootFather->sub.find_named<gui::ImageObject>(rootName)) {
						designer::clipboard.dataClip.emplace_named<gui::ImageObject>(designer::clipboard.dataClip.end(), rootName, *obj);
					}
				}
				else if (rootType == attr::designer::type::input) {
					if (auto* obj = rootFather->sub.find_named<gui::InputObject>(rootName)) {
						designer::clipboard.dataClip.emplace_named<gui::InputObject>(designer::clipboard.dataClip.end(), rootName, *obj);
					}
				}
				else if (rootType == attr::designer::type::option) {
					if (auto* obj = rootFather->sub.find_named<gui::OptionObject>(rootName)) {
						designer::clipboard.dataClip.emplace_named<gui::OptionObject>(designer::clipboard.dataClip.end(), rootName, *obj);
					}
				}
				else if (rootType == attr::designer::type::text) {
					if (auto* obj = rootFather->sub.find_named<gui::TextObject>(rootName)) {
						designer::clipboard.dataClip.emplace_named<gui::TextObject>(designer::clipboard.dataClip.end(), rootName, *obj);
					}
				}
				
				designer::clipboard.copiedItemName = optionName;
				designer::clipboard.hasData = true;
			}
		}
		
		//删除相关的辅助函数
		namespace removeHelper {
			//计算要删除的元素数量（当前元素 + 所有子元素）
			//该参数为名称，用'-'连接
			int countElementsToDelete(const std::string& optionName) {
				auto [type, path] = designer::getType(optionName);//type为类型标识，path为名称，用'-'连接
				int count = 1; // 当前元素
				auto iter = std::next(designer::nameList.find_order_named<std::string>(optionName));
				while (iter != designer::nameList.end()) {
					std::string& nextOptionName = *designer::nameList.find<std::string>(iter);//该变量为名称，用'-'连接
					auto [_, nextName] = designer::getType(nextOptionName);//nextName为名称，用'-'连接
					if (!designer::Name::isFather(path, nextName)) {
						break;
					}
					count++;
					iter++;
				}
				return count;
			}
			//找到下一个不被删除的元素（即不是当前元素，也不是当前元素的子元素）
			//该参数为名称，用'-'连接
			std::string getNextNotDeletedOptionName(const std::string& optionName) {
				auto [type, path] = designer::getType(optionName);//type为类型标识，path为名称，用'-'连接
				auto iter = std::next(designer::nameList.find_order_named<std::string>(optionName));
				while (iter != designer::nameList.end()) {
					std::string& nextOptionName = *designer::nameList.find<std::string>(iter);//该变量为名称，用'-'连接
					auto [_, nextName] = designer::getType(nextOptionName);//nextName为名称，用'-'连接
					if (!designer::Name::isFather(path, nextName)) {
						return nextOptionName;
					}
					iter++;
				}
				return {};
			}
			//上移被删除元素后方的所有元素
			//该参数为名称，用'-'连接
			void moveUp(const std::string& optionName, gui::AreaObject& mainList) {
				// 计算要删除的元素数量
				int elementsToDelete = countElementsToDelete(optionName);
				// 找到下一个不被删除的元素
				std::string nextNotDeleted = getNextNotDeletedOptionName(optionName);//该变量为名称，用'-'连接
				if (nextNotDeleted == "") {
					return;
				}
				// 从下一个不被删除的元素开始，上移所有元素
				auto iter = mainList.sub.find_order_named<gui::ImageObject>(nextNotDeleted);
				while (iter != mainList.sub.end()) {
					(*iter)->setPosition((*iter)->getPosition() - sf::Vector2f(0.f, 40.f * elementsToDelete));
					iter++;
				}
			}
			//从nameList中删除指定的元素
			//该参数为名称列表，用'-'连接
			void removeFromNameList(const std::vector<std::string>& toDelete) {
				for (const auto& deleteName : toDelete) {//该变量为名称，用'-'连接
					designer::nameList.erase_named<std::string>(deleteName);
				}
			}
			//该参数为名称，用'-'连接
			void cut(const std::string& optionName) {
				gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
				
				std::cout << "[CUT DEBUG] optionName: " << optionName << std::endl;
				
				designer::Name::moveHelper::copyToClipboard(optionName);
				
				auto [type, path] = designer::getType(optionName);//type为类型标识，path为名称，用'-'连接
				
				std::cout << "[CUT DEBUG] type: " << type << ", path: " << path << std::endl;
				
				designer::Name::removeHelper::moveUp(optionName, mainList);
				
				std::vector<std::string> items;
				items.push_back(optionName);
				if (type == attr::designer::type::area) {
					auto iter = std::next(designer::nameList.find_order_named<std::string>(optionName));
					while (iter != designer::nameList.end()) {
						std::string& nextName = *designer::nameList.find<std::string>(iter);//该变量为名称，用'-'连接
						auto [_, nextPath] = designer::getType(nextName);//nextPath为名称，用'-'连接
						std::cout << "[CUT DEBUG] checking nextName: " << nextName << ", nextPath: " << nextPath << std::endl;
						if (!designer::Name::isFather(path, nextPath)) break;
						items.push_back(nextName);
						iter++;
					}
				}
				
				std::cout << "[CUT DEBUG] items.size(): " << items.size() << std::endl;
				for (const auto& item : items) {
					std::cout << "[CUT DEBUG] item: " << item << std::endl;
				}
				
				for (const auto& itemName : items) {
					designer::nameList.erase_named<std::string>(itemName);
				}
				
				//从mainList中删除对应的img和opt
				for (const auto& itemName : items) {
					if (auto* img = mainList.sub.find_named<gui::ImageObject>(itemName)) {
						mainList.sub.erase(img);
					}
					if (auto* opt = mainList.sub.find_named<gui::OptionObject>(itemName)) {
						mainList.sub.erase(opt);
					}
				}
				
				//只删除顶层元素，子元素会随父元素一起被删除
				if (!items.empty()) {
					auto [itemType, itemPath] = designer::getType(items[0]);//itemType为类型标识，itemPath为名称，用'-'连接
					std::string name = designer::Name::getFatherName(itemPath).second;//该变量为名称，用'-'连接
					std::string fatherPath = designer::Name::getFatherName(itemPath).first;//该变量为名称，用'-'连接
					std::string dataFatherPath = designer::toDataPath(fatherPath);//该变量为路径，用'_'连接
					
					gui::AreaObject* father = fatherPath.empty() ? &designer::data : &designer::data.path_at<gui::AreaObject>(dataFatherPath);
					
					if (itemType == attr::designer::type::area) {
						if (auto* obj = father->sub.find_named<gui::AreaObject>(name)) {
							father->sub.erase(obj);
						}
					}
					else if (itemType == attr::designer::type::button) {
						if (auto* obj = father->sub.find_named<gui::ButtonObject>(name)) {
							father->sub.erase(obj);
						}
					}
					else if (itemType == attr::designer::type::image) {
						if (auto* obj = father->sub.find_named<gui::ImageObject>(name)) {
							father->sub.erase(obj);
						}
					}
					else if (itemType == attr::designer::type::input) {
						if (auto* obj = father->sub.find_named<gui::InputObject>(name)) {
							father->sub.erase(obj);
						}
					}
					else if (itemType == attr::designer::type::option) {
						if (auto* obj = father->sub.find_named<gui::OptionObject>(name)) {
							father->sub.erase(obj);
						}
					}
					else if (itemType == attr::designer::type::text) {
						if (auto* obj = father->sub.find_named<gui::TextObject>(name)) {
							father->sub.erase(obj);
						}
					}
				}
			}
		}
		//重建相关的辅助函数（打开文件后重建）
			namespace rebuildHelper {
				//该参数prefix为名称前缀，用'-'连接
				void rebuildNameListFromArea(const std::string& prefix, gui::AreaObject* area) {
					for (auto& elem : area->sub) {
					std::string key = area->sub.find_key(elem);//该变量为名称，用'-'连接
					if (auto* obj = area->sub.find<gui::AreaObject>(elem)) {
						std::string name = prefix + key;//该变量为名称，用'-'连接
						std::string optionName = attr::designer::type::area + name;//该变量为名称，用'-'连接
						designer::nameList.push_back_named(optionName, optionName);
						rebuildNameListFromArea(name + "-", obj);
					}
					else if (auto* obj = area->sub.find<gui::ButtonObject>(elem)) {
						std::string name = prefix + key;//该变量为名称，用'-'连接
						std::string optionName = attr::designer::type::button + name;//该变量为名称，用'-'连接
						designer::nameList.push_back_named(optionName, optionName);
					}
					else if (auto* obj = area->sub.find<gui::ImageObject>(elem)) {
						std::string name = prefix + key;//该变量为名称，用'-'连接
						std::string optionName = attr::designer::type::image + name;//该变量为名称，用'-'连接
						designer::nameList.push_back_named(optionName, optionName);
					}
					else if (auto* obj = area->sub.find<gui::InputObject>(elem)) {
						std::string name = prefix + key;//该变量为名称，用'-'连接
						std::string optionName = attr::designer::type::input + name;//该变量为名称，用'-'连接
						designer::nameList.push_back_named(optionName, optionName);
					}
					else if (auto* obj = area->sub.find<gui::OptionObject>(elem)) {
						std::string name = prefix + key;//该变量为名称，用'-'连接
						std::string optionName = attr::designer::type::option + name;//该变量为名称，用'-'连接
						designer::nameList.push_back_named(optionName, optionName);
					}
					else if (auto* obj = area->sub.find<gui::TextObject>(elem)) {
						std::string name = prefix + key;//该变量为名称，用'-'连接
						std::string optionName = attr::designer::type::text + name;//该变量为名称，用'-'连接
						designer::nameList.push_back_named(optionName, optionName);
					}
				}
			}
			
			void rebuildMainList(gui::AreaObject& mainList) {
				mainList.sub.clear();
				
				for (auto it = designer::nameList.begin(); it != designer::nameList.end(); it++) {
					std::string& optionName = *designer::nameList.find<std::string>(it);//该变量为名称，用'-'连接
					auto [type, path] = designer::getType(optionName);//type为类型标识，path为名称，用'-'连接
					std::string pureType = type.substr(1, type.size() - 2);
					std::string name = designer::Name::getFatherName(path).second;//该变量为名称，用'-'连接
					size_t level = designer::Name::countLevel(path);
					float linePos = (it == designer::nameList.begin()) ? 0.f : 
						mainList.sub.find_named<gui::OptionObject>(
							*designer::nameList.find<std::string>(std::prev(it))
						)->getPosition().y + 40.f;
					
					auto ptropt = mainList.sub.insert_named(mainList.sub.end(), optionName, gui::OptionObject{});
					(*ptropt)
						.setText("    " + name).setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
						.setFont("ht")
						.setCharacterSize(40)
						.setSizeAuto()
						.setPosition(sf::Vector2f(level * 40.f, linePos));
					
					auto ptrimg = mainList.sub.insert_named(ptropt, optionName, gui::ImageObject{});
					(*ptrimg)
						.setImageId(pureType)
						.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
						.setSizeAuto()
						.setPosition(sf::Vector2f(level * 40.f, linePos));
				}
			}
		}
		
		void fillUIBaseSettings(gui::AreaObject& mainSettings, gui::UIBase* obj) {
			mainSettings.path_get<gui::InputObject>("SetLeftUpPositionX").setText(floatToStr(obj->getPosition().x));
			mainSettings.path_get<gui::InputObject>("SetLeftUpPositionY").setText(floatToStr(obj->getPosition().y));
			mainSettings.path_get<gui::InputObject>("SetCenterPositionX").setText(floatToStr(obj->getPosition().x + obj->getSize().x / 2));
			mainSettings.path_get<gui::InputObject>("SetCenterPositionY").setText(floatToStr(obj->getPosition().y + obj->getSize().y / 2));
			mainSettings.path_get<gui::InputObject>("SetSizeX").setText(floatToStr(obj->getSize().x));
			mainSettings.path_get<gui::InputObject>("SetSizeY").setText(floatToStr(obj->getSize().y));
			mainSettings.path_get<gui::InputObject>("SetShow").setText(obj->getShow() ? L"1" : L"0");
			for (int i = 0; i < 3; i++) {
				std::string stateName[] = { "Normal", "Over", "Focus" };
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "BackgroundColorR").setText(toStr(obj->style(i).backgroundColor.r));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "BackgroundColorG").setText(toStr(obj->style(i).backgroundColor.g));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "BackgroundColorB").setText(toStr(obj->style(i).backgroundColor.b));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "BackgroundColorA").setText(toStr(obj->style(i).backgroundColor.a));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "OutlineColorR").setText(toStr(obj->style(i).outlineColor.r));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "OutlineColorG").setText(toStr(obj->style(i).outlineColor.g));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "OutlineColorB").setText(toStr(obj->style(i).outlineColor.b));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "OutlineColorA").setText(toStr(obj->style(i).outlineColor.a));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "OutlineThickness").setText(floatToStr(obj->style(i).outlineThickness));
			}
		}
		void fillTextSettings(gui::AreaObject& mainSettings, gui::TextObject* obj) {
			fillUIBaseSettings(mainSettings, obj);
			mainSettings.path_get<gui::InputObject>("SetText").setText(obj->getText());
			mainSettings.path_get<gui::InputObject>("SetFont").setText(sf::String(obj->getFont()));
			mainSettings.path_get<gui::InputObject>("SetCharacterSize").setText(toStr(obj->getCharacterSize()));
			mainSettings.path_get<gui::InputObject>("SetLetterSpacing").setText(floatToStr(obj->getLetterSpacing()));
			mainSettings.path_get<gui::InputObject>("SetLineSpacing").setText(floatToStr(obj->getLineSpacing()));
			//填充对齐选项
			{
				std::string hOpts[] = { "Left", "Mid", "Right" };
				std::string vOpts[] = { "Top", "Mid", "Bottom" };
				int jx = static_cast<int>(obj->getJustification().x);
				int jy = static_cast<int>(obj->getJustification().y);
				auto* justXArea = mainSettings.sub.find_named<gui::AreaObject>("SetJustificationX");
				auto* justYArea = mainSettings.sub.find_named<gui::AreaObject>("SetJustificationY");
				std::cout << "  fillTextSettings justification: jx=" << hOpts[jx] << " jy=" << vOpts[jy]
						  << " justXArea=" << (justXArea ? "found" : "NOT_FOUND")
						  << " justYArea=" << (justYArea ? "found" : "NOT_FOUND") << std::endl;
				if (justXArea) justXArea->setOption(hOpts[jx]);
				if (justYArea) justYArea->setOption(vOpts[jy]);
			}
			for (int i = 0; i < 3; i++) {
				std::string stateName[] = { "TextNormal", "TextOver", "TextFocus" };
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "BackgroundColorR").setText(toStr(obj->textStyle(i).fillColor.r));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "BackgroundColorG").setText(toStr(obj->textStyle(i).fillColor.g));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "BackgroundColorB").setText(toStr(obj->textStyle(i).fillColor.b));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "BackgroundColorA").setText(toStr(obj->textStyle(i).fillColor.a));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "OutlineColorR").setText(toStr(obj->textStyle(i).outlineColor.r));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "OutlineColorG").setText(toStr(obj->textStyle(i).outlineColor.g));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "OutlineColorB").setText(toStr(obj->textStyle(i).outlineColor.b));
				mainSettings.path_get<gui::InputObject>("Set" + stateName[i] + "OutlineColorA").setText(toStr(obj->textStyle(i).outlineColor.a));
			}
		}
		void fillAreaSettings(gui::AreaObject& mainSettings, gui::AreaObject* obj) {
			fillUIBaseSettings(mainSettings, obj);
			mainSettings.path_get<gui::InputObject>("SetOption").setText(sf::String(obj->getOption()));
			mainSettings.path_get<gui::InputObject>("SetScrollableDragX").setText(toStr(obj->getMouseDragScrollable().x));
			mainSettings.path_get<gui::InputObject>("SetScrollableDragY").setText(toStr(obj->getMouseDragScrollable().y));
			mainSettings.path_get<gui::InputObject>("SetScrollableWheelX").setText(toStr(obj->getMouseWheelScrollable().x));
			mainSettings.path_get<gui::InputObject>("SetScrollableWheelY").setText(toStr(obj->getMouseWheelScrollable().y));
		}
		void fillButtonSettings(gui::AreaObject& mainSettings, gui::ButtonObject* obj) {
			fillTextSettings(mainSettings, obj);
		}
		void fillImageSettings(gui::AreaObject& mainSettings, gui::ImageObject* obj) {
			fillUIBaseSettings(mainSettings, obj);
			mainSettings.path_get<gui::InputObject>("SetImageId").setText(sf::String(obj->getImageId()));
			mainSettings.path_get<gui::InputObject>("SetScaleX").setText(floatToStr(obj->getScale().x));
			mainSettings.path_get<gui::InputObject>("SetScaleY").setText(floatToStr(obj->getScale().y));
			mainSettings.path_get<gui::InputObject>("SetImageNormalColorR").setText(toStr(obj->getImageColor(0).r));
			mainSettings.path_get<gui::InputObject>("SetImageNormalColorG").setText(toStr(obj->getImageColor(0).g));
			mainSettings.path_get<gui::InputObject>("SetImageNormalColorB").setText(toStr(obj->getImageColor(0).b));
			mainSettings.path_get<gui::InputObject>("SetImageNormalColorA").setText(toStr(obj->getImageColor(0).a));
			mainSettings.path_get<gui::InputObject>("SetImageOverColorR").setText(toStr(obj->getImageColor(1).r));
			mainSettings.path_get<gui::InputObject>("SetImageOverColorG").setText(toStr(obj->getImageColor(1).g));
			mainSettings.path_get<gui::InputObject>("SetImageOverColorB").setText(toStr(obj->getImageColor(1).b));
			mainSettings.path_get<gui::InputObject>("SetImageOverColorA").setText(toStr(obj->getImageColor(1).a));
			mainSettings.path_get<gui::InputObject>("SetImageFocusColorR").setText(toStr(obj->getImageColor(2).r));
			mainSettings.path_get<gui::InputObject>("SetImageFocusColorG").setText(toStr(obj->getImageColor(2).g));
			mainSettings.path_get<gui::InputObject>("SetImageFocusColorB").setText(toStr(obj->getImageColor(2).b));
			mainSettings.path_get<gui::InputObject>("SetImageFocusColorA").setText(toStr(obj->getImageColor(2).a));
		}
		void fillInputSettings(gui::AreaObject& mainSettings, gui::InputObject* obj) {
			fillTextSettings(mainSettings, obj);
			mainSettings.path_get<gui::InputObject>("SetSizeLimit").setText(toStr(obj->getSizeLimit()));
		}
		void fillOptionSettings(gui::AreaObject& mainSettings, gui::OptionObject* obj) {
			fillTextSettings(mainSettings, obj);
		}
		//解析输入框内容为浮点数
		float parseFloat(const sf::String& text) {
			try { return std::stof(text.toWideString()); }
			catch (...) { return 0.f; }
		}
		//解析输入框内容为整数
		int parseInt(const sf::String& text) {
			try { return std::stoi(text.toWideString()); }
			catch (...) { return 0; }
		}
		//解析输入框内容为sf::Color的RGBA分量
		sf::Color parseColor(gui::AreaObject& settings, const std::string& prefix) {
			return sf::Color(
				parseInt(settings.path_get<gui::InputObject>(prefix + "R").getText()),
				parseInt(settings.path_get<gui::InputObject>(prefix + "G").getText()),
				parseInt(settings.path_get<gui::InputObject>(prefix + "B").getText()),
				parseInt(settings.path_get<gui::InputObject>(prefix + "A").getText())
			);
		}
		//解析选项名称为Justification值
		gui::UIBase::Justification parseJustification(const std::string& opt) {
			if (opt == "Left" || opt == "Top") return gui::UIBase::Left;
			if (opt == "Right" || opt == "Bottom") return gui::UIBase::Right;
			return gui::UIBase::Mid;
		}
		//读取Justification选项
		std::pair<gui::UIBase::Justification, gui::UIBase::Justification> readJustification(gui::AreaObject& settings, const std::string& prefix) {
			std::string xName = prefix + "X";
			std::string yName = prefix + "Y";
			std::string xOpt = settings.path_get<gui::AreaObject>(xName).getOption();
			std::string yOpt = settings.path_get<gui::AreaObject>(yName).getOption();
			std::cout << "  readJustification[" << prefix << "] X=" << xOpt << " Y=" << yOpt << std::endl;
			return { parseJustification(xOpt), parseJustification(yOpt) };
		}
		//同步pos/centerpos：根据触发源自动更新另一个
		void syncPosition(gui::AreaObject& mainSettings, const std::string& triggerInput) {
			float lx = parseFloat(mainSettings.path_get<gui::InputObject>("SetLeftUpPositionX").getText());
			float ly = parseFloat(mainSettings.path_get<gui::InputObject>("SetLeftUpPositionY").getText());
			float cx = parseFloat(mainSettings.path_get<gui::InputObject>("SetCenterPositionX").getText());
			float cy = parseFloat(mainSettings.path_get<gui::InputObject>("SetCenterPositionY").getText());
			float sx = parseFloat(mainSettings.path_get<gui::InputObject>("SetSizeX").getText());
			float sy = parseFloat(mainSettings.path_get<gui::InputObject>("SetSizeY").getText());

			//判断触发源
			bool isPosChanged = (triggerInput == "SetLeftUpPositionX" || triggerInput == "SetLeftUpPositionY");
			bool isCenterChanged = (triggerInput == "SetCenterPositionX" || triggerInput == "SetCenterPositionY");
			//size改变时更新centerPos，centerPos改变时更新pos，pos改变时更新centerPos
			if (isPosChanged || triggerInput == "SetSizeX" || triggerInput == "SetSizeY") {
				//pos或size改变 → 更新centerPos = pos + size/2
				mainSettings.path_get<gui::InputObject>("SetCenterPositionX").setText(floatToStr(lx + sx / 2));
				mainSettings.path_get<gui::InputObject>("SetCenterPositionY").setText(floatToStr(ly + sy / 2));
			}
			else if (isCenterChanged) {
				//centerPos改变 → 更新leftUpPos = centerPos - size/2
				mainSettings.path_get<gui::InputObject>("SetLeftUpPositionX").setText(floatToStr(cx - sx / 2));
				mainSettings.path_get<gui::InputObject>("SetLeftUpPositionY").setText(floatToStr(cy - sy / 2));
			}
		}
		//应用UIBase设置
		void applyUIBaseSettings(gui::UIBase* obj, gui::AreaObject& mainSettings) {
			std::string path;
			try {
				path = "SetLeftUpPositionX";
				float lx = parseFloat(mainSettings.path_get<gui::InputObject>(path).getText());
				path = "SetLeftUpPositionY";
				float ly = parseFloat(mainSettings.path_get<gui::InputObject>(path).getText());
				obj->setPosition({ lx, ly });
				std::cout << "  applyUIBase pos=(" << lx << "," << ly << ")" << std::endl;

				path = "SetSizeX";
				float sx = parseFloat(mainSettings.path_get<gui::InputObject>(path).getText());
				path = "SetSizeY";
				float sy = parseFloat(mainSettings.path_get<gui::InputObject>(path).getText());
				obj->setSize({ sx, sy });
				std::cout << "  applyUIBase size=(" << sx << "," << sy << ")" << std::endl;

				path = "SetShow";
				obj->setShow(parseInt(mainSettings.path_get<gui::InputObject>(path).getText()) != 0);

				std::string states[] = { "Normal", "Over", "Focus" };
				for (int i = 0; i < 3; i++) {
					std::string p = "Set" + states[i];
					obj->style(i).backgroundColor = parseColor(mainSettings, p + "BackgroundColor");
					obj->style(i).outlineColor = parseColor(mainSettings, p + "OutlineColor");
					obj->style(i).outlineThickness = parseFloat(mainSettings.path_get<gui::InputObject>(p + "OutlineThickness").getText());
				}
			}
			catch (const std::exception& e) {
				std::cout << "  applyUIBaseSettings error on " << path << ": " << e.what() << std::endl;
			}
		}
		//应用Text设置
		void applyTextSettings(gui::TextObject* obj, gui::AreaObject& mainSettings) {
			applyUIBaseSettings(obj, mainSettings);
			obj->setText(mainSettings.path_get<gui::InputObject>("SetText").getText());
			obj->setFont(mainSettings.path_get<gui::InputObject>("SetFont").getText().toAnsiString());
			obj->setCharacterSize(parseInt(mainSettings.path_get<gui::InputObject>("SetCharacterSize").getText()));
			obj->setSpacing(parseFloat(mainSettings.path_get<gui::InputObject>("SetLetterSpacing").getText()),
						   parseFloat(mainSettings.path_get<gui::InputObject>("SetLineSpacing").getText()));
			auto [hJust, vJust] = readJustification(mainSettings, "SetJustification");
			obj->setJustification(hJust, vJust);
			std::string states[] = { "TextNormal", "TextOver", "TextFocus" };
			for (int i = 0; i < 3; i++) {
				std::string p = "Set" + states[i];
				obj->textStyle(i).fillColor = parseColor(mainSettings, p + "BackgroundColor");
				obj->textStyle(i).outlineColor = parseColor(mainSettings, p + "OutlineColor");
			}
		}
		//应用各类型设置
		//该参数type为类型标识；dataPath为路径，用'_'连接
		void applySettings(const std::string& type, const std::string& dataPath, gui::AreaObject& mainSettings) {
			std::cout << "  [applySettings] type=" << type << " path=" << dataPath << std::endl;
			if (type == attr::designer::type::area) {
				auto* obj = data.path_find<gui::AreaObject>(dataPath);
				if (!obj) return;
				applyUIBaseSettings(obj, mainSettings);
				obj->setOption(mainSettings.path_get<gui::InputObject>("SetOption").getText().toAnsiString());
				obj->setScrollable(
					{ parseInt(mainSettings.path_get<gui::InputObject>("SetScrollableDragX").getText()),
					  parseInt(mainSettings.path_get<gui::InputObject>("SetScrollableDragY").getText()) },
					{ parseInt(mainSettings.path_get<gui::InputObject>("SetScrollableWheelX").getText()),
					  parseInt(mainSettings.path_get<gui::InputObject>("SetScrollableWheelY").getText()) });
			}
			else if (type == attr::designer::type::button) {
				auto* obj = data.path_find<gui::ButtonObject>(dataPath);
				if (!obj) return;
				applyTextSettings(obj, mainSettings);
			}
			else if (type == attr::designer::type::image) {
				auto* obj = data.path_find<gui::ImageObject>(dataPath);
				if (!obj) return;
				applyUIBaseSettings(obj, mainSettings);
				obj->setImageId(mainSettings.path_get<gui::InputObject>("SetImageId").getText().toAnsiString());
				obj->setScale({ parseFloat(mainSettings.path_get<gui::InputObject>("SetScaleX").getText()),
								parseFloat(mainSettings.path_get<gui::InputObject>("SetScaleY").getText()) });
				auto [hJust, vJust] = readJustification(mainSettings, "SetJustification");
				obj->setJustification(hJust, vJust);
				obj->setImageColor(
					parseColor(mainSettings, "SetImageNormalColor"),
					parseColor(mainSettings, "SetImageOverColor"),
					parseColor(mainSettings, "SetImageFocusColor"));
			}
			else if (type == attr::designer::type::input) {
				auto* obj = data.path_find<gui::InputObject>(dataPath);
				if (!obj) return;
				applyTextSettings(obj, mainSettings);
				obj->setSizeLimit(parseInt(mainSettings.path_get<gui::InputObject>("SetSizeLimit").getText()));
				std::string typeOpt = mainSettings.path_get<gui::AreaObject>("SetTypeLimit").getOption();
				std::cout << "  [applySettings input] typeOpt=" << typeOpt << std::endl;
				if (typeOpt == "Int") obj->setTypeLimit(gui::InputObject::Int);
				else if (typeOpt == "Float") obj->setTypeLimit(gui::InputObject::Float);
				else obj->setTypeLimit(gui::InputObject::String);
			}
			else if (type == attr::designer::type::option) {
				auto* obj = data.path_find<gui::OptionObject>(dataPath);
				if (!obj) return;
				applyTextSettings(obj, mainSettings);
			}
			else if (type == attr::designer::type::text) {
				auto* obj = data.path_find<gui::TextObject>(dataPath);
				if (!obj) return;
				applyTextSettings(obj, mainSettings);
			}
			//同步到preview窗口
			designer::Preview::applyToPreview(type, dataPath);
			designer::Preview::syncWindowSize();
		}
		//该参数type为类型标识；name为名称，用'-'连接
		void switchOption(const std::string& type,const std::string& name) {
			gui::AreaObject& mainSettings = menuManager.path_at<gui::AreaObject>(attr::garea::main_settings);
			mainSettings.sub.clear();
			std::string dataPath = designer::toDataPath(name);//该变量为路径，用'_'连接
			if (type==attr::designer::type::area) {
				mainSettings.sub.merge(settings::UIBase.sub);
				mainSettings.sub.merge(settings::Area.sub);
				gui::AreaObject* obj = data.path_find<gui::AreaObject>(dataPath);
				if (!obj) return;
				fillAreaSettings(mainSettings, obj);
			}
			if (type == attr::designer::type::button) {
				mainSettings.sub.merge(settings::UIBase.sub);
				mainSettings.sub.merge(settings::Text.sub);
				gui::ButtonObject* obj = data.path_find<gui::ButtonObject>(dataPath);
				if (!obj) return;
				fillButtonSettings(mainSettings, obj);
			}
			if (type == attr::designer::type::image) {
				mainSettings.sub.merge(settings::UIBase.sub);
				mainSettings.sub.merge(settings::Image.sub);
				gui::ImageObject* obj = data.path_find<gui::ImageObject>(dataPath);
				if (!obj) return;
				fillImageSettings(mainSettings, obj);
			}
			if (type == attr::designer::type::input) {
				mainSettings.sub.merge(settings::UIBase.sub);
				mainSettings.sub.merge(settings::Text.sub);
				mainSettings.sub.merge(settings::Input.sub);
				gui::InputObject* obj = data.path_find<gui::InputObject>(dataPath);
				if (!obj) return;
				fillInputSettings(mainSettings, obj);
			}
			if (type == attr::designer::type::option) {
				mainSettings.sub.merge(settings::UIBase.sub);
				mainSettings.sub.merge(settings::Text.sub);
				gui::OptionObject* obj = data.path_find<gui::OptionObject>(dataPath);
				if (!obj) return;
				fillOptionSettings(mainSettings, obj);
			}
			if (type == attr::designer::type::text) {
				mainSettings.sub.merge(settings::UIBase.sub);
				mainSettings.sub.merge(settings::Text.sub);
				gui::TextObject* obj = data.path_find<gui::TextObject>(dataPath);
				if (!obj) return;
				fillTextSettings(mainSettings, obj);
			}
		}
	}
	//传入参数使用的是路径的函数，路径即用'_'分割的字符串，操作的是data中的对象
	namespace Data {
		//该参数为路径，用'_'连接；返回(父路径, 自身名)，均用'_'连接
		std::pair<std::string, std::string> getFatherPath(const std::string& path) {
			size_t lastPos = path.find_last_of('_');
			if (lastPos != std::string::npos) {
				return { path.substr(0, lastPos), path.substr(lastPos + 1) };
			}
			return { "",path };
		}
		//该参数type为类型标识；path为路径，用'_'连接
		std::pair<gui::AreaObject*,std::pmr::list<gui::UIBase*>::const_iterator> getNextIter(const std::string& type, const std::string& path,bool isSub) {
			if (type == "") {
				return { &data, data.sub.end() };
			}
			else {
				if (isSub) {
					gui::AreaObject* father = &data.path_at<gui::AreaObject>(path);
					return { father,father->sub.begin() };
				}
				else {
					auto [fatherPath, name] = getFatherPath(path);//fatherPath为父路径，name为自身名，均用'_'连接
					gui::AreaObject* father=nullptr;
					if (fatherPath == "") {
						father = &data;
					}
					else {
						father = &data.path_at<gui::AreaObject>(fatherPath);
					}
					std::pmr::list<gui::UIBase*>::const_iterator ptr;
					if (type == attr::designer::type::area)
						ptr = father->sub.find_order_named<gui::AreaObject>(name);
					if (type == attr::designer::type::button)
						ptr = father->sub.find_order_named<gui::ButtonObject>(name);
					if (type == attr::designer::type::image)
						ptr = father->sub.find_order_named<gui::ImageObject>(name);
					if (type == attr::designer::type::input)
						ptr = father->sub.find_order_named<gui::InputObject>(name);
					if (type == attr::designer::type::option)
						ptr = father->sub.find_order_named<gui::OptionObject>(name);
					if (type == attr::designer::type::text)
						ptr = father->sub.find_order_named<gui::TextObject>(name);
					return { father,std::next(ptr) };
				}
			}
		}
		//该参数chosenType为类型标识；chosenPath为路径，用'_'连接；newType为类型标识；newName为单个名称
		gui::UIBase* insert(const std::string& chosenType, const std::string& chosenPath, bool isSub, const std::string& newType, const std::string& newName) {
			auto [father, nextDataIter] = getNextIter(chosenType, chosenPath, isSub);
			if (newType == attr::designer::type::area)
				return father->sub.emplace_named<gui::AreaObject>(nextDataIter,newName);
			if (newType == attr::designer::type::button)
				return father->sub.emplace_named<gui::ButtonObject>(nextDataIter, newName);
			if (newType == attr::designer::type::image)
				return father->sub.emplace_named<gui::ImageObject>(nextDataIter, newName);
			if (newType == attr::designer::type::input)
				return father->sub.emplace_named<gui::InputObject>(nextDataIter, newName);
			if (newType == attr::designer::type::option)
				return father->sub.emplace_named<gui::OptionObject>(nextDataIter, newName);
			if (newType == attr::designer::type::text)
				return father->sub.emplace_named<gui::TextObject>(nextDataIter, newName);
			return nullptr;
		}
		//从data中删除指定路径的对象及其所有子对象
		//该参数type为类型标识；path为路径，用'_'连接
		void remove(const std::string& type, const std::string& path) {
			auto [fatherPath, name] = getFatherPath(path);//fatherPath为父路径，name为自身名，均用'_'连接
			gui::AreaObject* father = nullptr;
			if (fatherPath == "") {
				father = &data;
			}
			else {
				father = &data.path_at<gui::AreaObject>(fatherPath);
			}
			//按类型查找并删除对象
			if (type == attr::designer::type::area) {
				if (auto* obj = father->sub.find_named<gui::AreaObject>(name)) {
					father->sub.erase(obj);
				}
			}
			else if (type == attr::designer::type::button) {
				if (auto* obj = father->sub.find_named<gui::ButtonObject>(name)) {
					father->sub.erase(obj);
				}
			}
			else if (type == attr::designer::type::image) {
				if (auto* obj = father->sub.find_named<gui::ImageObject>(name)) {
					father->sub.erase(obj);
				}
			}
			else if (type == attr::designer::type::input) {
				if (auto* obj = father->sub.find_named<gui::InputObject>(name)) {
					father->sub.erase(obj);
				}
			}
			else if (type == attr::designer::type::option) {
				if (auto* obj = father->sub.find_named<gui::OptionObject>(name)) {
					father->sub.erase(obj);
				}
			}
			else if (type == attr::designer::type::text) {
				if (auto* obj = father->sub.find_named<gui::TextObject>(name)) {
					father->sub.erase(obj);
				}
			}
		}
	}
	namespace Save {
		void populateCheckboxes() {
			gui::AreaObject& windowToSave = menuManager.path_at<gui::AreaObject>("save_windowToSave");
			windowToSave.sub.clear();
			int windowToSaveLine = 0;
			for (auto it = designer::data.sub.begin(); it != designer::data.sub.end(); it++) {
				std::string key = designer::data.sub.find_key(it);
				windowToSave.path_get<gui::TextObject>(key)
					.setText(L"√")
					.setFont("ht")
					.setCharacterSize(20)
					.setSizeAuto()
					.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
					.setPosition(sf::Vector2f(20, static_cast<float>(40 * windowToSaveLine) + 20))
					.setShow(true)
					.setCenter();
				windowToSave.path_get<gui::ButtonObject>(key)
					.setText(L"□"+sf::String(key))
					.setTextStyle(textStyle["stdtn"], textStyle["stdto"], textStyle["stdtf"])
					.setFont("ht")
					.setCharacterSize(40)
					.setSizeAuto()
					.setJustification(gui::UIBase::Left, gui::UIBase::Mid)
					.setStyle(style["null"], style["null"], style["null"])
					.setPosition(sf::Vector2f(0, static_cast<float>(40 * windowToSaveLine)));
				windowToSaveLine++;
			}
			windowToSave.setSize(sf::Vector2f(600, static_cast<float>(5 * 40)));
		}
		void toggleCheckbox(const std::string& key) {
			gui::TextObject& check = menuManager.path_at<gui::TextObject>("save_windowToSave_" + key);
			check.toggleShow();
		}
		bool isChecked(const std::string& key) {
			return menuManager.path_at<gui::TextObject>("save_windowToSave_" + key).getShow();
		}
	}
	namespace New {
		int isSubType = 0;
		bool isSubEnabled = false;
		void resetIsSub() {
			menuManager.path_at<gui::TextObject>(attr::gtext::new_isSub).setShow(true);
		}
		void enableIsSub() {
			menuManager.path_at<gui::ButtonObject>(attr::gbutton::new_isSubText).setShow(true);
		}
		void disableIsSub(bool val) {
			menuManager.path_at<gui::TextObject>(attr::gtext::new_isSub).setShow(val);
			menuManager.path_at<gui::ButtonObject>(attr::gbutton::new_isSubText).setShow(false);
		}
		void isSubSetStatu(int _isSubType) {
			if (_isSubType == 0) {
				isSubEnabled = true;
				designer::New::enableIsSub();
			}
			else {
				isSubEnabled = false;
				if (_isSubType == 1) {
					designer::New::disableIsSub(false);
				}
				else {
					designer::New::disableIsSub(true);
				}
			}
		}
	}
	namespace Paste {
		int isSubType = 0;
		bool isSubEnabled = false;
		void resetIsSub() {
			menuManager.path_at<gui::TextObject>(attr::gtext::paste_isSub).setShow(true);
		}
		void enableIsSub() {
			menuManager.path_at<gui::ButtonObject>(attr::gbutton::paste_isSubText).setShow(true);
		}
		void disableIsSub(bool val) {
			menuManager.path_at<gui::TextObject>(attr::gtext::paste_isSub).setShow(val);
			menuManager.path_at<gui::ButtonObject>(attr::gbutton::paste_isSubText).setShow(false);
		}
		void isSubSetStatu(int _isSubType) {
			if (_isSubType == 0) {
				isSubEnabled = true;
				designer::Paste::enableIsSub();
			}
			else {
				isSubEnabled = false;
				if (_isSubType == 1) {
					designer::Paste::disableIsSub(false);
				}
				else {
					designer::Paste::disableIsSub(true);
				}
			}
		}
		void pasteExecute(gui::AreaObject& mainList, const std::string& ChosenType, const std::string& ChosenPath, bool isSub) {
			std::string fatherPath = isSub ? ChosenPath : designer::Name::getFatherName(ChosenPath).first;//该变量为父名称，用'-'连接
			
			std::string firstNewName;//该变量为名称，用'-'连接
			
			std::vector<std::pair<std::string, std::string>> clipItems;
			for (auto it = designer::clipboard.nameListClip.begin(); it != designer::clipboard.nameListClip.end(); it++) {
				std::string nameListKey = designer::clipboard.nameListClip.find_key(it);//该变量为名称，用'-'连接
				auto [clipType, clipPath] = designer::getType(nameListKey);//clipType为类型标识，clipPath为名称，用'-'连接
				
				std::string newName = fatherPath.empty() ? (clipType + clipPath) : (clipType + fatherPath + '-' + clipPath);//该变量为名称，用'-'连接
				if (clipItems.empty()) firstNewName = newName;
				
				clipItems.push_back({nameListKey, newName});
			}
			
			std::string nextOptionName = designer::Name::createHelper::getNextListOptionName(mainList, ChosenType, ChosenPath, isSub);//该变量为名称，用'-'连接
			
			float linePos;
			gui::OptionObject* nextOptionPtr = mainList.sub.find_named<gui::OptionObject>(nextOptionName);
			if (nextOptionPtr == nullptr)
				linePos = designer::nameList.size() * 40.f;
			else linePos = nextOptionPtr->getPosition().y;
			
			size_t itemCount = clipItems.size();
			for (size_t i = 0; i < itemCount; i++) {
				designer::Name::createHelper::moveDown(nextOptionName, mainList);
			}
			
			for (const auto& [oldName, newName] : clipItems) {
				auto [itemType, itemPath] = designer::getType(newName);//itemType为类型标识，itemPath为名称，用'-'连接
				std::string itemName = designer::Name::getFatherName(itemPath).second;//该变量为名称，用'-'连接
				size_t level = designer::Name::countLevel(itemPath);
				
				designer::nameList.insert_named(designer::nameList.find_order_named<std::string>(nextOptionName), newName, newName);
				
				std::string imageId = itemType.substr(1, itemType.size() - 2);//该变量为类型标识，去掉括号
				
				auto nextOptionIter = mainList.sub.find_order_named<gui::ImageObject>(nextOptionName);
				auto ptropt = mainList.sub.insert_named(nextOptionIter, newName, gui::OptionObject{});
				auto ptrimg = mainList.sub.insert_named(ptropt, newName, gui::ImageObject{});
				(*ptrimg)
					.setImageId(imageId)
					.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
					.setSizeAuto()
					.setPosition(sf::Vector2f(level * 40.f, linePos));
				(*ptropt)
					.setText("    " + itemName).setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
					.setFont("ht")
					.setCharacterSize(40)
					.setSizeAuto()
					.setPosition(sf::Vector2f(level * 40.f, linePos));
				
				linePos += 40.f;
			}
			
			auto rootName = designer::Name::getFatherName(designer::clipboard.copiedItemName).second;//该变量为名称，用'-'连接
			auto [rootType, _] = designer::getType(designer::clipboard.copiedItemName);//rootType为类型标识
			
			std::string newDataChosenPath = designer::toDataPath(ChosenPath);//该变量为路径，用'_'连接
			auto [father, nextDataIter] = designer::Data::getNextIter(ChosenType, newDataChosenPath, isSub);
			
			if (rootType == attr::designer::type::area) {
				if (auto* obj = designer::clipboard.dataClip.find_named<gui::AreaObject>(rootName)) {
					father->sub.emplace_named<gui::AreaObject>(nextDataIter, rootName, *obj);
				}
			}
			else if (rootType == attr::designer::type::button) {
				if (auto* obj = designer::clipboard.dataClip.find_named<gui::ButtonObject>(rootName)) {
					father->sub.emplace_named<gui::ButtonObject>(nextDataIter, rootName, *obj);
				}
			}
			else if (rootType == attr::designer::type::image) {
				if (auto* obj = designer::clipboard.dataClip.find_named<gui::ImageObject>(rootName)) {
					father->sub.emplace_named<gui::ImageObject>(nextDataIter, rootName, *obj);
				}
			}
			else if (rootType == attr::designer::type::input) {
				if (auto* obj = designer::clipboard.dataClip.find_named<gui::InputObject>(rootName)) {
					father->sub.emplace_named<gui::InputObject>(nextDataIter, rootName, *obj);
				}
			}
			else if (rootType == attr::designer::type::option) {
				if (auto* obj = designer::clipboard.dataClip.find_named<gui::OptionObject>(rootName)) {
					father->sub.emplace_named<gui::OptionObject>(nextDataIter, rootName, *obj);
				}
			}
			else if (rootType == attr::designer::type::text) {
				if (auto* obj = designer::clipboard.dataClip.find_named<gui::TextObject>(rootName)) {
					father->sub.emplace_named<gui::TextObject>(nextDataIter, rootName, *obj);
				}
			}
			
			menuManager.close("paste");
			mainList.setOption(firstNewName);
			
			designer::Preview::currentWindowName = "";
			designer::Preview::copyWindowToPreview(designer::Name::getWindowName(firstNewName), previewManager.window("preview"));
			designer::Preview::syncWindowSize();
		}
	}
}
int main() {
	fontManager.loadFont("ht", "resources/FZHTJW.TTF");

	init();

	/*BoolFStream bf("D:/1.bin");
	bf.clear();
	bf << windowManager.preset;
	bf.close();
	exit(0);*/

	/*BoolFStream bf("D:/1.bin");
	windowManager.readPreset(bf);
	bf.close();*/
	
	//初始化预览窗口
	gui::AreaObject PreviewWindow;
	PreviewWindow
		.setStyle(style["stda1"], style["stda1"], style["stda1"])
		.setPosition({0,0})
		.setSize({0,0});
	previewManager.open("preview", PreviewWindow);
	preview.create(sf::VideoMode(sf::Vector2u(windowWidth, windowHeight)), L"Preview", sf::Style::Close, sf::State::Windowed);
	preview.setFramerateLimit(60);
	
	menuManager.open("main",Main);
	menu.create(sf::VideoMode(sf::Vector2u(windowWidth, windowHeight)), L"WindowDesigner", sf::Style::Close, sf::State::Windowed);
	menu.setFramerateLimit(60);
	
	while (true) {
		//处理菜单窗口事件
		while (const std::optional sfEvt = menu.pollEvent()) {
			if (sfEvt->is<sf::Event::Closed>()) {
				exit(0);
			}
			else {
				menuManager.update(sfEvt);
			}
		}
		
		//处理预览窗口事件
		while (const std::optional sfEvt = preview.pollEvent()) {
			if (sfEvt->is<sf::Event::Closed>()) {
				//预览窗口关闭时退出程序
				exit(0);
			}
			else {
				previewManager.update(sfEvt);
			}
		}
		while (auto evtptr = menuManager.pollEvent()) {
			if (auto evt = evtptr->getIf<gui::Events::ButtonPressed>()) {
				std::cout << "Button pressed : " << evt->wholePath() << std::endl;
				if (evt->wholePath() == attr::gbutton::new_ok) {
					std::string option = menuManager.path_at<gui::AreaObject>("new").getOption();//该变量为类型选项
					if (!option.empty() && designer::isAvailableName(menuManager.path_at<gui::InputObject>(attr::ginput::new_name).getText())) {
						std::string name = menuManager.path_at<gui::InputObject>(attr::ginput::new_name).getText();//该变量为名称，用'-'连接
						bool isSub = menuManager.path_at<gui::TextObject>(attr::gtext::new_isSub).getShow();
						gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
						std::string ChosenOptionName = mainList.getOption();//该变量为名称，用'-'连接
						auto [ChosenType, ChosenPath] = designer::getType(ChosenOptionName);//ChosenType为类型标识，ChosenPath为名称，用'-'连接
						int addType = 0;
						if (option == "window")
							addType = 1;
						else if (!ChosenOptionName.empty())
							addType = 2;
						if (addType) {
							std::string type;//该变量为类型标识
							std::string path;//该变量为名称，用'-'连接
							if (addType == 1) {
								type = attr::designer::type::area;
								option = "area";
								path = name;
								ChosenType = attr::designer::type::area;
								ChosenPath = designer::Name::getWindowName(ChosenPath);
							}
							else {
								type = '(' + option + ')';
								if (isSub)
									path = ChosenPath + '-' + name;
								else path = designer::Name::getFatherName(ChosenPath).first + '-' + name;
							}
							std::string OptionName = type + path;//该变量为名称，用'-'连接
							if (!mainList.sub.find_named<gui::OptionObject>(OptionName)) {
								float linePos;
								std::string nextOptionName;//该变量为名称，用'-'连接
								nextOptionName = designer::Name::createHelper::getNextListOptionName(mainList, ChosenType, ChosenPath, isSub);
								gui::OptionObject* nextOptionPtr = mainList.sub.find_named<gui::OptionObject>(nextOptionName);
								if (nextOptionPtr == nullptr)
									linePos = designer::nameList.size() * 40.f;
								else linePos = nextOptionPtr->getPosition().y;
								size_t level = designer::Name::countLevel(path);
								designer::Name::createHelper::moveDown(nextOptionName,mainList);
								designer::nameList.insert_named(designer::nameList.find_order_named<std::string>(nextOptionName),OptionName, OptionName);
								auto nextOptionIter = mainList.sub.find_order_named<gui::ImageObject>(nextOptionName);
								auto ptropt = mainList.sub.insert_named(nextOptionIter, OptionName, gui::OptionObject{});
								auto ptrimg = mainList.sub.insert_named(ptropt, OptionName, gui::ImageObject{});
								(*ptrimg)
									.setImageId(option)
									.setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
									.setSizeAuto()
									.setPosition(sf::Vector2f(level*40.f, linePos));
								(*ptropt)
									.setText("    " + name).setJustification(gui::UIBase::Mid, gui::UIBase::Mid)
									.setFont("ht")
									.setCharacterSize(40)
									.setSizeAuto()
									.setPosition(sf::Vector2f(level*40.f, linePos));
								designer::Data::insert(ChosenType, designer::toDataPath(ChosenPath), isSub, type, name);
								designer::Name::switchOption(type,path);
								mainList.setOption(OptionName);
								ChosenOptionName = OptionName;
								//同步到preview窗口
								std::string dataPath = designer::toDataPath(path);//该变量为路径，用'_'连接
								designer::Preview::currentWindowName = ""; //重置以强制重新拷贝
								designer::Preview::copyWindowToPreview(designer::Name::getWindowName(path), previewManager.window("preview"));
								designer::Preview::syncWindowSize();
								menuManager.close("new");
							}
						}
					}
				}
				if (evt->wholePath() == attr::gbutton::new_cancel) {
					menuManager.close("new");
				}
				if (evt->wholePath() == attr::gbutton::new_isSubText) {
					menuManager.path_at<gui::TextObject>(attr::gtext::new_isSub).toggleShow();
				}
				if (evt->wholePath() == attr::gbutton::main_new) {
					menuManager.open("new", New);
					gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
					std::string ChosenOptionName = mainList.getOption();
					auto [ChosenType, ChosenPath] = designer::getType(ChosenOptionName);
					int isSubType = 0;//是需要选择isSub对象
					if (ChosenType != attr::designer::type::area)
						isSubType = 1;//是非area对象
					else if (designer::Name::countLevel(ChosenPath) == 0)
						isSubType = 2;//是window对象
					designer::New::isSubType = isSubType;
					designer::New::resetIsSub();
					designer::New::isSubSetStatu(isSubType);
				}
				//处理打开按钮按下事件
				if (evt->wholePath() == attr::gbutton::main_open) {
					menuManager.open("open", Open);
				}
				if (evt->wholePath() == attr::gbutton::open_ok) {
					std::filesystem::path filepath = menuManager.path_at<gui::InputObject>(attr::ginput::open_filepath).getText().toWideString();
					std::string format = menuManager.path_at<gui::AreaObject>("open").getOption();
					if (!filepath.empty() && !format.empty()) {
						if (format == "binary") {
							BinaryFileStream bf(filepath);
							bf.read(designer::data);
							bf.close();
						}
						else if (format == "json") {
							std::ifstream ifs(filepath);
							nlohmann::json j;
							ifs >> j;
							ifs.close();
							j.get_to(designer::data);
						}
						//刷新nameList
						designer::nameList.clear();
						designer::Name::rebuildHelper::rebuildNameListFromArea("", &designer::data);
						//刷新main_list
						gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
						designer::Name::rebuildHelper::rebuildMainList(mainList);
						//清空settings面板
						menuManager.path_at<gui::AreaObject>(attr::garea::main_settings).sub.clear();
						//刷新preview窗口
						designer::Preview::currentWindowName = "";
						if (auto* firstWindow = designer::data.sub.find<gui::AreaObject>(designer::data.sub.begin())) {
							std::string firstWindowName = designer::data.sub.find_key(designer::data.sub.begin());
							designer::Preview::copyWindowToPreview(firstWindowName, previewManager.window("preview"));
							designer::Preview::syncWindowSize();
						}
						//关闭打开窗口
						menuManager.close("open");
					}
				}
				if (evt->wholePath() == attr::gbutton::open_cancel) {
					menuManager.close("open");
				}
				//处理保存按钮按下事件
				if (evt->wholePath() == attr::gbutton::main_save || evt->wholePath() == attr::gbutton::main_saveas) {
					menuManager.open("save", Save);
					designer::Save::populateCheckboxes();
				}
				if (evt->wholePath() == attr::gbutton::save_ok) {
					std::filesystem::path filepath = menuManager.path_at<gui::InputObject>(attr::ginput::save_filepath).getText().toWideString();
					std::string format = menuManager.path_at<gui::AreaObject>("save").getOption();
					if (!filepath.empty() && !format.empty()) {
						gui::AreaObject dataToSave;
						for (auto it = designer::data.sub.begin(); it != designer::data.sub.end(); it++) {
							std::string key = designer::data.sub.find_key(it);
							if (designer::Save::isChecked(key)) {
								if (auto* ptr = designer::data.sub.find<gui::AreaObject>(it)) {
									dataToSave.sub.emplace_named<gui::AreaObject>(dataToSave.sub.end(), key, *ptr);
								}
								else if (auto* ptr = designer::data.sub.find<gui::ButtonObject>(it)) {
									dataToSave.sub.emplace_named<gui::ButtonObject>(dataToSave.sub.end(), key, *ptr);
								}
								else if (auto* ptr = designer::data.sub.find<gui::InputObject>(it)) {
									dataToSave.sub.emplace_named<gui::InputObject>(dataToSave.sub.end(), key, *ptr);
								}
								else if (auto* ptr = designer::data.sub.find<gui::OptionObject>(it)) {
									dataToSave.sub.emplace_named<gui::OptionObject>(dataToSave.sub.end(), key, *ptr);
								}
								else if (auto* ptr = designer::data.sub.find<gui::TextObject>(it)) {
									dataToSave.sub.emplace_named<gui::TextObject>(dataToSave.sub.end(), key, *ptr);
								}
								else if (auto* ptr = designer::data.sub.find<gui::ImageObject>(it)) {
									dataToSave.sub.emplace_named<gui::ImageObject>(dataToSave.sub.end(), key, *ptr);
								}
							}
						}
						if (format == "binary") {
							BinaryFileStream bf(filepath);
							bf.clear();
							bf.write(dataToSave);
							bf.close();
						}
						else if (format == "json") {
							nlohmann::json j = dataToSave;
							std::ofstream ofs(filepath);
							ofs << j.dump(1,'\t');
							ofs.close();
						}
						menuManager.close("save");
					}
				}
				if (evt->path=="save_windowToSave") {
					designer::Save::toggleCheckbox(evt->name);
				}
				if (evt->wholePath() == attr::gbutton::save_cancel) {
					menuManager.close("save");
				}
				if (evt->wholePath() == attr::gbutton::paste_ok) {
					gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
					std::string ChosenOptionName = mainList.getOption();//该变量为名称，用'-'连接
					if (!ChosenOptionName.empty() && designer::clipboard.hasData) {
						auto [ChosenType, ChosenPath] = designer::getType(ChosenOptionName);//ChosenType为类型标识，ChosenPath为名称，用'-'连接
						bool isSub = menuManager.path_at<gui::TextObject>(attr::gtext::paste_isSub).getShow();
						designer::Paste::pasteExecute(mainList, ChosenType, ChosenPath, isSub);
					}
				}
			if (evt->wholePath() == attr::gbutton::paste_cancel) {
					menuManager.close("paste");
				}
				if (evt->wholePath() == attr::gbutton::paste_isSubText) {
					menuManager.path_at<gui::TextObject>(attr::gtext::paste_isSub).toggleShow();
				}
				//处理删除按钮按下事件
				if (evt->wholePath() == attr::gbutton::main_delete) {
					gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
					gui::AreaObject& mainSettings = menuManager.path_at<gui::AreaObject>(attr::garea::main_settings);
					std::string ChosenOptionName = mainList.getOption();//该变量为名称，用'-'连接
					//检查是否有选中项
					if (!ChosenOptionName.empty()) {
						auto [ChosenType, ChosenPath] = designer::getType(ChosenOptionName);//ChosenType为类型标识，ChosenPath为名称，用'-'连接
						//上移被删除元素后方的所有元素（在删除之前调用）
						designer::Name::removeHelper::moveUp(ChosenOptionName, mainList);
						
						// 收集所有要删除的元素名称（当前元素 + 所有子元素）
						std::vector<std::string> toDelete;
						toDelete.push_back(ChosenOptionName);
						auto iter = designer::nameList.find_order_named<std::string>(ChosenOptionName);
						iter++; // 跳过第一个元素
						while (iter != designer::nameList.end()) {
							std::string& nextOptionName = *designer::nameList.find<std::string>(iter);//该变量为名称，用'-'连接
							auto [_, nextName] = designer::getType(nextOptionName);//nextName为名称，用'-'连接
							if (!designer::Name::isFather(ChosenPath, nextName)) {
								break;
							}
							toDelete.push_back(nextOptionName);
							iter++;
						}
						
						//从mainList中删除所有OptionObject和ImageObject
						for (const auto& optionName : toDelete) {
							if (auto* ptr = mainList.sub.find_named<gui::OptionObject>(optionName)) {
								mainList.sub.erase(ptr);
							}
							if (auto* ptr = mainList.sub.find_named<gui::ImageObject>(optionName)) {
								mainList.sub.erase(ptr);
							}
						}
						
						//从nameList中删除所有对应的条目
						designer::Name::removeHelper::removeFromNameList(toDelete);
						//从data中删除对应的UI对象及其所有子对象
					designer::Data::remove(ChosenType, designer::toDataPath(ChosenPath));
						//同步删除preview窗口
						designer::Preview::removeFromPreview(ChosenPath);
						//清空settings面板
						mainSettings.sub.clear();
						//清空选中状态
						mainList.setOption();
					}
				}
				//处理上移按钮按下事件
				if (evt->wholePath() == attr::gbutton::main_moveup) {
					gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
					std::string ChosenOptionName = mainList.getOption();//该变量为名称，用'-'连接
					if (!ChosenOptionName.empty()) {
						auto [ChosenType, ChosenPath] = designer::getType(ChosenOptionName);//ChosenType为类型标识，ChosenPath为名称，用'-'连接
						designer::Name::moveHelper::moveUp(ChosenOptionName);
						//重新选中原来的项
						mainList.setOption(ChosenOptionName);
					}
				}
				//处理下移按钮按下事件
				if (evt->wholePath() == attr::gbutton::main_movedown) {
					gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
					std::string ChosenOptionName = mainList.getOption();//该变量为名称，用'-'连接
					if (!ChosenOptionName.empty()) {
						auto [ChosenType, ChosenPath] = designer::getType(ChosenOptionName);//ChosenType为类型标识，ChosenPath为名称，用'-'连接
						designer::Name::moveHelper::moveDown(ChosenOptionName);
						//重新选中原来的项
						mainList.setOption(ChosenOptionName);
					}
				}
				//处理剪切按钮按下事件
				if (evt->wholePath() == attr::gbutton::main_cut) {
					gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
					gui::AreaObject& mainSettings = menuManager.path_at<gui::AreaObject>(attr::garea::main_settings);
					std::string ChosenOptionName = mainList.getOption();//该变量为名称，用'-'连接
					if (!ChosenOptionName.empty()) {
						auto [ChosenType, ChosenPath] = designer::getType(ChosenOptionName);//ChosenType为类型标识，ChosenPath为名称，用'-'连接
						
						designer::Name::removeHelper::cut(ChosenOptionName);
						designer::Preview::removeFromPreview(ChosenPath);
						
						mainSettings.sub.clear();
						mainList.setOption();
					}
				}
				//处理复制按钮按下事件
				if (evt->wholePath() == attr::gbutton::main_copy) {
					gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
					std::string ChosenOptionName = mainList.getOption();//该变量为名称，用'-'连接
					if (!ChosenOptionName.empty()) {
						designer::Name::moveHelper::copyToClipboard(ChosenOptionName);
						mainList.setOption(ChosenOptionName);
					}
				}
				//处理粘贴按钮按下事件
				if (evt->wholePath() == attr::gbutton::main_paste) {
					if (designer::clipboard.hasData) {
						gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
						std::string ChosenOptionName = mainList.getOption();//该变量为名称，用'-'连接
						auto [ChosenType, ChosenPath] = designer::getType(ChosenOptionName);//ChosenType为类型标识，ChosenPath为名称，用'-'连接
						
						auto [clipRootType, _] = designer::getType(designer::clipboard.copiedItemName);//clipRootType为类型标识
						bool clipIsArea = (clipRootType == attr::designer::type::area);
						
						int isSubType = 0;
						if (ChosenType != attr::designer::type::area)
							isSubType = 1;
						else if (designer::Name::countLevel(ChosenPath) == 0)
							isSubType = 2;
						
						designer::Paste::isSubType = isSubType;
						
						if (isSubType == 2) {
							if (clipIsArea) {
								menuManager.open("paste", Paste);
								designer::Paste::resetIsSub();
								designer::Paste::enableIsSub();
							}
							else {
								designer::Paste::pasteExecute(mainList, ChosenType, ChosenPath, true);
							}
						}
						else if (isSubType != 0) {
							designer::Paste::pasteExecute(mainList, ChosenType, ChosenPath, false);
						}
						else {
							menuManager.open("paste", Paste);
							designer::Paste::resetIsSub();
							designer::Paste::isSubSetStatu(isSubType);
						}
					}
				}
			}
			if (auto evt = evtptr->getIf<gui::Events::InputSelected>()) {
				std::cout << "Input selected : " << evt->wholePath() << std::endl;
			}
			if (auto evt = evtptr->getIf<gui::Events::InputDeselected>()) {
				std::cout << "Input deselected : " << evt->wholePath() << std::endl;
				//将settings面板中的修改应用到data中对应的对象
				if (evt->path.find(attr::garea::main_settings) == 0) {
					gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
					std::string ChosenOptionName = mainList.getOption();
					//先同步pos/centerpos联动
					std::string inputName = evt->name;
					if (inputName == "SetLeftUpPositionX" || inputName == "SetLeftUpPositionY" ||
						inputName == "SetCenterPositionX" || inputName == "SetCenterPositionY" ||
						inputName == "SetSizeX" || inputName == "SetSizeY") {
						designer::Name::syncPosition(menuManager.path_at<gui::AreaObject>(attr::garea::main_settings), inputName);
					}
					if (!ChosenOptionName.empty()) {
						auto [ChosenType, ChosenPath] = designer::getType(ChosenOptionName);
						designer::Name::applySettings(ChosenType, designer::toDataPath(ChosenPath), menuManager.path_at<gui::AreaObject>(attr::garea::main_settings));
					}
				}
			}
			if (auto evt = evtptr->getIf<gui::Events::OptionSelected>()) {
				std::cout << "Option selected : " << evt->wholePath() << std::endl;
				if (evt->wholePath() == attr::goption::new_window) {
					designer::New::disableIsSub(false);
				}
				if (evt->path == "new" && evt->name != "window") {
					designer::New::isSubSetStatu(designer::New::isSubType);
				}
				if (evt->path == attr::garea::main_list) {
					auto [type, name] = designer::getType(evt->name);
					designer::Name::switchOption(type, name);
					designer::printDataDebug();
					designer::printNameListDebug();
					designer::printClipboardDebug();
					designer::printMainListDebug();
				}
				//settings面板中的选项被选中时（如justification/typeLimit），应用设置到data
				if (evt->path.find(attr::garea::main_settings) == 0) {
					gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
					std::string ChosenOptionName = mainList.getOption();
					if (!ChosenOptionName.empty()) {
						auto [ChosenType, ChosenPath] = designer::getType(ChosenOptionName);
						designer::Name::applySettings(ChosenType, designer::toDataPath(ChosenPath), menuManager.path_at<gui::AreaObject>(attr::garea::main_settings));
					}
				}
			}
			if (auto evt = evtptr->getIf<gui::Events::OptionDeselected>()) {
				std::cout << "Option deselected : " << evt->wholePath() << std::endl;
				if (evt->wholePath() == attr::goption::new_window) {
					designer::New::resetIsSub();
				}
			}
		}
		//切换窗口时更新预览
		gui::AreaObject& mainList = menuManager.path_at<gui::AreaObject>(attr::garea::main_list);
			std::string ChosenOptionName = mainList.getOption();
			if (!ChosenOptionName.empty()) {
				auto [type, fullPath] = designer::getType(ChosenOptionName);
				std::string windowName = designer::Name::getWindowName(fullPath);
				//只在切换窗口时才拷贝
				designer::Preview::copyWindowToPreview(windowName, previewManager.window("preview"));
				//根据窗口大小调整preview窗口大小
				designer::Preview::syncWindowSize();
			}
		
		menu.clear();
		menuManager.draw(menu);
		menu.display();
		
		preview.clear();
		previewManager.draw(preview);
		preview.display();
	}
	return 0;
}
