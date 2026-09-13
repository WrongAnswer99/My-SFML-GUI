//Author : WrongAnswer99

#include "engine/tick/Tick.hpp"
#include "engine/gui/MyGUI.hpp"
#include "engine/serialization/BinaryFileStream.hpp"
#include "engine/serialization/JsonExtensions.hpp"
#include <fstream>
gui::Style panelStyle;
gui::Style optionNormalStyle;
gui::Style optionOverStyle;
gui::Style optionSelectedStyle;
int windowWidth = 1270, windowHeight = 720;
gui::UIwindowManager windowManager;
sf::RenderWindow window;
gui::AreaObject Main;
static void init() {
	panelStyle.set(sf::Color(240, 240, 240), sf::Color(160, 160, 160), 2.f);
	optionNormalStyle.set(sf::Color(250, 250, 250), sf::Color(180, 180, 180), 2.f);
	optionOverStyle.set(sf::Color(225, 235, 245), sf::Color(120, 150, 180), 2.f);
	optionSelectedStyle.set(sf::Color(160, 205, 245), sf::Color(70, 130, 190), 2.f);

	Main.setStyle(panelStyle, panelStyle, panelStyle)
		.setPositionRelative(
			{{gui::UIBase::Anchor::Left,gui::UIBase::Relative::LeftEdge,0.f},{gui::UIBase::Anchor::Right,gui::UIBase::Relative::RightEdge,0.f}},
			{{gui::UIBase::Anchor::Top,gui::UIBase::Relative::TopEdge,0.f},{gui::UIBase::Anchor::Bottom,gui::UIBase::Relative::BottomEdge,0.f}}
		);

	Main.path_get<gui::ButtonObject>("button")
		.setText(L"按钮")
		.setFont("ht")
		.setCharacterSize(50)
		.setSizeAuto()
		.setPositionRelative(
			{{gui::UIBase::Anchor::Right,gui::UIBase::Relative::RightEdge,-50.f}},
			{{gui::UIBase::Anchor::Bottom,gui::UIBase::Relative::BottomEdge,0.f}}
		);

	Main.path_get<gui::TextObject>("text")
		.setText(L"这是外层窗口，被设置为不可拖动")
		.setFont("ht")
		.setCharacterSize(50)
		.setSizeAuto()
		.setPositionRelative(
			{{gui::UIBase::Anchor::Mid, gui::UIBase::Relative::MidLine, 0.f}},
			{{gui::UIBase::Anchor::Top, gui::UIBase::Relative::TopEdge, 0.f}}
		);

	Main.path_get<gui::AreaObject>("area")
		.setScrollable(sf::Vector2i(1, 1), sf::Vector2i(1, 1))
		.setStyle(panelStyle, panelStyle, panelStyle)
		.setPositionRelative(
			{{gui::UIBase::Anchor::Left,gui::UIBase::Relative::LeftEdge,50.f},{gui::UIBase::Anchor::Right,gui::UIBase::Relative::RightEdge,-50.f}},
			{{gui::UIBase::Anchor::Top,gui::UIBase::Relative::TopEdge,50.f},{gui::UIBase::Anchor::Bottom,gui::UIBase::Relative::BottomEdge,-50.f}}
		);

	Main.path_get<gui::TextObject>("area.text2")
		.setText(L"↓↓↓向下滑↓↓↓\n当按下按钮时拖动，\n操作将转化为拖动，\n且不会触发按钮")
		.setFont("ht")
		.setCharacterSize(50)
		.setSizeAuto()
		.setPosition(sf::Vector2f(350, 500), {gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid});


	
	Main.path_get<gui::TextObject>("area.top")
		.setText(L"↑顶部，无法继续上拉")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 25), {gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid});

	Main.path_get<gui::TextObject>("area.text")
		.setText(L"这是内层窗口，被设置为可拖动\n现已支持惯性滑动")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 100), {gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid});
	
	Main.path_get<gui::InputObject>("area.input")
		.setText(L"按下'/'键切换至此文本框")
		.setAlign(gui::UIBase::Align::Mid, gui::UIBase::Align::Mid)
		.setFont("ht")
		.setCharacterSize(50)
		.setSizeAuto()
		.setPosition(sf::Vector2f(350, 175), {gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid});

	Main.path_get<gui::InputObject>("area.inputSlim")
		.setText(L"这是一个窄文本框")
		.setAlign(gui::UIBase::Align::Mid, gui::UIBase::Align::Mid)
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 225),{gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(400, 35));
	
	Main.path_get<gui::InputObject>("area.inputFloat")
		.setTypeLimit(gui::InputObject::Float)
		.setText(L"123.456")
		.setAlign(gui::UIBase::Align::Mid, gui::UIBase::Align::Mid)
		.setFont("ht")
		.setCharacterSize(50)
		.setSizeAuto()
		.setPosition(sf::Vector2f(1000, 225),{gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid});

	Main.path_get<gui::ButtonObject>("area.button")
		.setText(L"按钮").setAlign(gui::UIBase::Align::Mid, gui::UIBase::Align::Mid)
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 275),{gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(100, 50));

	Main.path_get<gui::OptionObject>("area.option1")
		.setText(L"选项1")
		.setFont("ht")
		.setCharacterSize(50)
		.setStyle(optionNormalStyle, optionOverStyle, optionSelectedStyle)
		.setPosition(sf::Vector2f(350, 750),{gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::OptionObject>("area.option2")
		.setText(L"选项2")
		.setFont("ht")
		.setCharacterSize(50)
		.setStyle(optionNormalStyle, optionOverStyle, optionSelectedStyle)
		.setPosition(sf::Vector2f(350, 800),{gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::OptionObject>("area.option3")
		.setText(L"选项3")
		.setFont("ht")
		.setCharacterSize(50)
		.setStyle(optionNormalStyle, optionOverStyle, optionSelectedStyle)
		.setPosition(sf::Vector2f(350, 850),{gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::OptionObject>("area.option4")
		.setText(L"选项4")
		.setFont("ht")
		.setCharacterSize(50)
		.setStyle(optionNormalStyle, optionOverStyle, optionSelectedStyle)
		.setPosition(sf::Vector2f(350, 900),{gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::OptionObject>("area.option5")
		.setText(L"选项5")
		.setFont("ht")
		.setCharacterSize(50)
		.setStyle(optionNormalStyle, optionOverStyle, optionSelectedStyle)
		.setPosition(sf::Vector2f(350, 950),{gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));

	Main.path_get<gui::TextObject>("area.1")
		.setText(L"1")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 700), {gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::TextObject>("area.2")
		.setText(L"2")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 1000), {gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::TextObject>("area.3")
		.setText(L"3")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 1300),{gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::TextObject>("area.4")
		.setText(L"4")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 1600),{gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::TextObject>("area.5")
		.setText(L"5")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 1900),{gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));

	Main.path_get<gui::TextObject>("area.6")
		.setText(L"6")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 700), {gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::TextObject>("area.7")
		.setText(L"7")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 1000), {gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::TextObject>("area.8")
		.setText(L"8")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 1300), {gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::TextObject>("area.9")
		.setText(L"9")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 1600), {gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::TextObject>("area.10")
		.setText(L"10")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 1900), {gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::TextObject>("area.bottom")
		.setText(L"↓底部")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 2200), {gui::UIBase::Anchor::Mid, gui::UIBase::Anchor::Mid})
		.setSize(sf::Vector2f(150, 50));
	Main.path_get<gui::ImageObject>("area.image")
		.setImageId("test")
		.setScaleAuto()
		.setAlign(gui::UIBase::Align::Mid, gui::UIBase::Align::Mid)
		//.setScale(sf::Vector2f(1, 1))
		//.setScaleTo(sf::Vector2f(200, 200))
		//.setSizeAuto()
		.setSize(sf::Vector2f(200, 200))
		.setStyle(optionNormalStyle, optionNormalStyle, optionNormalStyle)
		.setPosition(sf::Vector2f(600, 250));
		
	Main.path_get<gui::ImageObject>("area.image1")
		.setImageId("test")
		.setSizeAuto()
		.setAlign(gui::UIBase::Align::Mid, gui::UIBase::Align::Mid)
		//.setScale(sf::Vector2f(1, 1))
		//.setScaleTo(sf::Vector2f(200, 200))
		//.setSizeAuto()
		.setStyle(optionNormalStyle, optionNormalStyle, optionNormalStyle)
		.setPosition(sf::Vector2f(600, 450));
}
int main() {
	fontManager.loadAuto();
	imageManager.loadAuto();
	init();

	// 序列化测试：先保存到文件
	{
		BinaryFileStream fs(L"tools/testOutputs/test.bin");
		fs.clear();
		fs.write(gui::GUIvarianTmapBinarySerializer{Main.sub});
		std::cout << "Saved to tools/testOutputs/test.bin" << std::endl;
	}

	// 清空 Main
	Main.sub.clear();
	std::cout << "Main cleared, size: " << Main.sub.size() << std::endl;

	// 从文件读取
	{
		BinaryFileStream fs(L"tools/testOutputs/test.bin");
		fs.read(gui::GUIvarianTmapBinarySerializer{Main.sub});
		std::cout << "Loaded from tools/testOutputs/test.bin, size: " << Main.sub.size() << std::endl;
	}

	// JSON 序列化测试：先保存到文件
	{
		nlohmann::json j = Main;
		std::ofstream ofs("tools/testOutputs/test.json");
		ofs << j.dump(1, '\t');
		std::cout << "Saved to tools/testOutputs/test.json" << std::endl;
	}

	// 清空 Main
	Main.sub.clear();
	std::cout << "Main cleared (JSON test), size: " << Main.sub.size() << std::endl;

	// 从 JSON 文件读取
	{
		std::ifstream ifs("tools/testOutputs/test.json");
		nlohmann::json j;
		ifs >> j;
		j.get_to(Main);
		std::cout << "Loaded from tools/testOutputs/test.json, size: " << Main.sub.size() << std::endl;
	}

	windowManager.open("main",Main);
	window.create(sf::VideoMode(sf::Vector2u(windowWidth,windowHeight)), L"测试", sf::Style::Close | sf::Style::Resize, sf::State::Windowed);
	window.setFramerateLimit(144);
	TickManager tickManager;
	while (true) {
		for (TickManager::TickCount ticks = tickManager.getCurrentTick(); ticks > 0; --ticks) {
		while (const std::optional sfEvt=window.pollEvent()) {
			if (sfEvt->is<sf::Event::Closed>()) {
				exit(0);
			}
			else if (auto* resized = sfEvt->getIf<sf::Event::Resized>()) {
				window.setView(sf::View(sf::FloatRect({0.f,0.f},static_cast<sf::Vector2f>(resized->size))));
			}
			else windowManager.update(sfEvt);
		}
		while (const auto evt=windowManager.pollEvent()) {
			if (auto ptr = evt->getIf<gui::Events::KeyPressed>()) {
				std::cout << "Key pressed  : isFocusUI=" << ptr->isFocusUI << " focusWindow=" << ptr->focusWindow << " focusAreaPath=" << ptr->focusAreaPath << " code=" << static_cast<int>(ptr->code) << std::endl;
				if (ptr->isFocusUI && ptr->focusWindow == "main") {
					if (ptr->code == sf::Keyboard::Key::Slash) {
						windowManager.simulatePress("main_area_input");
					}
					else if (ptr->focusAreaPath == "main_area") {
						if (ptr->code == sf::Keyboard::Key::W)
							windowManager.simulateSetPrevOption("main_area");
						else if (ptr->code == sf::Keyboard::Key::S)
							windowManager.simulateSetNextOption("main_area");
						else if (ptr->code == sf::Keyboard::Key::Escape)
							windowManager.path_at<gui::AreaObject>("main_area").setOption();
						else if (ptr->code == sf::Keyboard::Key::Enter)
							windowManager.simulatePress("main_area_button");
					}
					else if (ptr->code == sf::Keyboard::Key::Enter)
						windowManager.simulatePress("main_button");
				}
			}
			if (auto ptr = evt->getIf<gui::Events::KeyReleased>()) {
				std::cout << "Key released : isFocusUI=" << ptr->isFocusUI << " focusWindow=" << ptr->focusWindow << " focusAreaPath=" << ptr->focusAreaPath << " code=" << static_cast<int>(ptr->code) << std::endl;
				if (ptr->isFocusUI && ptr->focusWindow == "main" && ptr->code == sf::Keyboard::Key::Enter) {
					if (ptr->focusAreaPath == "main_area")
						windowManager.simulateRelease("main_area_button");
					else
						windowManager.simulateRelease("main_button");
				}
			}
			if (auto ptr = evt->getIf<gui::Events::MousePressed>()) {
				std::cout << "Mouse pressed : isFocusUI=" << ptr->isFocusUI << " button=" << static_cast<int>(ptr->button) << " pos=" << ptr->position.x << "," << ptr->position.y << std::endl;
			}
			if (auto ptr = evt->getIf<gui::Events::MouseReleased>()) {
				std::cout << "Mouse released: isFocusUI=" << ptr->isFocusUI << " button=" << static_cast<int>(ptr->button) << " pos=" << ptr->position.x << "," << ptr->position.y << std::endl;
			}
			if (auto ptr = evt->getIf<gui::Events::MouseMoved>()) {
				std::cout << "Mouse moved   : isFocusUI=" << ptr->isFocusUI << " pos=" << ptr->position.x << "," << ptr->position.y << std::endl;
			}
			if (auto ptr = evt->getIf<gui::Events::MouseWheelScrolled>()) {
				std::cout << "Mouse wheel   : isFocusUI=" << ptr->isFocusUI << " wheel=" << static_cast<int>(ptr->wheel) << " delta=" << ptr->delta << " pos=" << ptr->position.x << "," << ptr->position.y << std::endl;
			}
			if (auto ptr=evt->getIf<gui::Events::ButtonPressed>()) {
				std::cout << "Button pressed : " << ptr->wholePath() << std::endl;
			}
			if (auto ptr = evt->getIf<gui::Events::OptionSelected>()) {
				std::cout << "Option selected : " << ptr->wholePath() << std::endl;
				std::cout << "Current choise : " << windowManager.path_at<gui::AreaObject>("main.area").getOption() << std::endl;
			}
			if (auto ptr = evt->getIf<gui::Events::OptionDeselected>()) {
				std::cout << "Option deselected : " << ptr->wholePath() << std::endl;
			}
			if (auto ptr = evt->getIf<gui::Events::InputSelected>()) {
				std::cout << "Input selected : " << ptr->wholePath() << std::endl;
			}
			if (auto ptr = evt->getIf<gui::Events::InputDeselected>()) {
				std::cout << "Input deselected : " << ptr->wholePath() << std::endl;
			}
		}
			windowManager.update();
		}
		window.clear();
		windowManager.draw(window);
		window.display();
	}
	return 0;
}
