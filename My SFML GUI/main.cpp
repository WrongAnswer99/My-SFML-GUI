//Author : WrongAnswer99

#include "MyGUI.hpp"
std::unordered_map<std::string, gui::Style>style;
int windowWidth = 800, windowHeight = 600;
gui::WindowManager windowManager;
sf::RenderWindow window;
gui::AreaObject Main;
static void init() {
	style["stda1"].set(sf::Color::White, sf::Color(200, 200, 200), 2);
	style["stda2"].set(sf::Color(240,240,240), sf::Color(200, 200, 200), 2);
	style["stdbn"].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2);
	style["stdbo"].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2);
	style["stdbf"].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2);

	Main.setStyle(style["stda1"], style["stda1"], style["stda1"])
		.setPosition(sf::Vector2f(0, 0))
		.setSize(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));

	Main.path_get<gui::ButtonObject>("button")
		.setText(L"按钮")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(650, 500))
		.setSize(sf::Vector2f(100, 50));

	Main.path_get<gui::TextObject>("text")
		.setText(L"这是外层窗口，被设置为不可拖动")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(400, 25))
		.setCenter();

	Main.path_get<gui::AreaObject>("area")
		.setScrollable(sf::Vector2i(1, 1), sf::Vector2i(1, 1))
		.setStyle(style["stda2"], style["stda2"], style["stda2"])
		.setPosition(sf::Vector2f(50, 50))
		.setSize(sf::Vector2f(700, 450));

	Main.path_get<gui::TextObject>("area.text2")
		.setText(L"↓↓↓向下滑↓↓↓\n当按下按钮时拖动，\n操作将转化为拖动，\n且不会触发按钮")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 500));

	Main.path_get<gui::TextObject>("area_text2")
		.setSizeAuto()
		.setCenter();

	
	Main.path_get<gui::TextObject>("area.top")
		.setText(L"↑顶部，无法继续上拉")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 25))
		.setCenter();

	Main.path_get<gui::TextObject>("area.text")
		.setText(L"这是内层窗口，被设置为可拖动\n现已支持惯性滑动")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 100))
		.setCenter();
	
	Main.path_get<gui::InputObject>("area.input")
		//.setSizeLimit(10)
		//.setTypeLimit(attr::gui::Float)
		//.setTypeLimit(attr::gui::Int)
		//.setStringTypeLimit(true, { L'_' }, { {L'A',L'Z'},{L'a',L'z'}})
		.setText(L"这是一个文本框")
		.setJustification(attr::gui::Mid, attr::gui::Mid)
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 225))
		.setSize(sf::Vector2f(400, 50));
	Main.path_get<gui::InputObject>("area.input")
		.setSizeAuto()
		.setCenter();

	Main.path_get<gui::ButtonObject>("area.button")
		.setText(L"按钮").setJustification(attr::gui::Mid, attr::gui::Mid)
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 275))
		.setSize(sf::Vector2f(100, 50))
		.setCenter();

	Main.path_get<gui::OptionObject>("area.option1")
		.setText(L"选项1")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 750))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::OptionObject>("area.option2")
		.setText(L"选项2")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 800))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::OptionObject>("area.option3")
		.setText(L"选项3")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 850))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::OptionObject>("area.option4")
		.setText(L"选项4")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 900))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::OptionObject>("area.option5")
		.setText(L"选项5")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 950))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();

	Main.path_get<gui::TextObject>("area.1")
		.setText(L"1")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 700))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::TextObject>("area.2")
		.setText(L"2")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 1000))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::TextObject>("area.3")
		.setText(L"3")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 1300))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::TextObject>("area.4")
		.setText(L"4")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 1600))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::TextObject>("area.5")
		.setText(L"5")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 1900))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();

	Main.path_get<gui::TextObject>("area.6")
		.setText(L"6")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 700))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::TextObject>("area.7")
		.setText(L"7")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 1000))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::TextObject>("area.8")
		.setText(L"8")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 1300))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::TextObject>("area.9")
		.setText(L"9")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 1600))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::TextObject>("area.10")
		.setText(L"10")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 1900))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	Main.path_get<gui::TextObject>("area.bottom")
		.setText(L"↓底部")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 2200))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	imageManager.loadImage("test","D:\\test.png");
	Main.path_get<gui::ImageObject>("area.image")
		.setImageId("test")
		.setJustification(attr::gui::Mid, attr::gui::Mid)
		//.setScale(sf::Vector2f(1, 1))
		//.setScaleTo(sf::Vector2f(200, 200))
		//.setSizeAuto()
		.setSize(sf::Vector2f(200, 200))
		.setStyle(style["stdbn"], style["stdbn"], style["stdbn"])
		.setPosition(sf::Vector2f(600, 250));
		
	Main.path_get<gui::ImageObject>("area.image1")
		.setImageId("test")
		.setJustification(attr::gui::Mid, attr::gui::Mid)
		//.setScale(sf::Vector2f(1, 1))
		//.setScaleTo(sf::Vector2f(200, 200))
		//.setSizeAuto()
		.setSize(sf::Vector2f(200, 200))
		.setStyle(style["stdbn"], style["stdbn"], style["stdbn"])
		.setPosition(sf::Vector2f(600, 450));
	Main.path_get<gui::AreaObject>("area")
		.setScrollLimitAuto();
}
Event evt;
int main() {
	fontManager.loadFont("ht", "FZHTJW.TTF");
	init();
	/*
	BinaryFStream fs("D:\\1.bin");
	fs.clear();
	fs << imageManager;
	fs << Main;
	*/

	windowManager.open("main",Main);
	window.create(sf::VideoMode(sf::Vector2u(windowWidth,windowHeight)), L"测试", sf::Style::Close, sf::State::Windowed);
	window.setFramerateLimit(60);
	while (true) {
		while (const std::optional sfEvt=window.pollEvent()) {
			if (sfEvt->is<sf::Event::Closed>()) {
				exit(0);
			}
			else {
				windowManager.update(sfEvt);
			}
		}
		while (windowManager.pollEvent(evt)) {
			if (evt.eventId == attr::gui::ButtonPressed) {
				std::cout << "Button pressed : " << evt[attr::gui::ButtonPath].cast<std::string>() << std::endl;
			}
			if (evt.eventId == attr::gui::OptionChosen) {
				std::cout << "Option chosen : " << evt[attr::gui::OptionPath].cast<std::string>() << std::endl;
				std::cout << "Current choise : " << windowManager.path_at<gui::AreaObject>("main.area").getOption() << std::endl;
			}
			if (evt.eventId == attr::gui::InputGainFocus) {
				std::cout << "Input gain focus : " << evt[attr::gui::InputPath].cast<std::string>() << std::endl;
			}
			if (evt.eventId == attr::gui::InputLoseFocus) {
				std::cout << "Input lose focus : " << evt[attr::gui::InputPath].cast<std::string>() << std::endl;
			}
		}
		window.clear();
		windowManager.draw(window);
		window.display();
	}
	return 0;
}
