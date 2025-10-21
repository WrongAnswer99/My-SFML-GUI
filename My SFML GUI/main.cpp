//Author : WrongAnswer99

#define LET_ME_SEE_SEE
#include "GUI.cpp"
#include <iostream>
unordered_map<string, game::gui::WindowManager::Style>style;

int windowWidth = 800, windowHeight = 600;
game::gui::WindowManager windowManager;
static void init() {
	style["stda1"].set(sf::Color::White, sf::Color(200, 200, 200), 2);
	style["stda2"].set(sf::Color(240,240,240), sf::Color(200, 200, 200), 2);
	style["stdbn"].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2);
	style["stdbo"].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2);
	style["stdbf"].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2);

	windowManager.preset["main"]
		.setStyle(style["stda1"], style["stda1"], style["stda1"])
		.setPosition(sf::Vector2f(0, 0))
		.setSize(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
	windowManager.preset["main"].button["button"]
		.setText(L"按钮")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(650, 500))
		.setSize(sf::Vector2f(100, 50));
	
	windowManager.preset["main"].text["text"]
		.setText(L"这是外层窗口，被设置为不可拖动")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(400, 25))
		.setCenter();

	windowManager.preset["main"].area["area"]
		.setScrollable(sf::Vector2i(1, 1), sf::Vector2i(1, 1))
		.setStyle(style["stda2"], style["stda2"], style["stda2"])
		.setPosition(sf::Vector2f(50, 50))
		.setSize(sf::Vector2f(700, 450));

	windowManager.preset["main"].area["area"].text["text2"]
		.setText(L"↓↓↓向下滑↓↓↓\n当按下按钮时拖动，\n操作将转化为拖动，\n且不会触发按钮")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 500));
	windowManager.preset["main"].area["area"].text["text2"]
		.setSizeAuto()
		.setCenter();

	
	windowManager.preset["main"].area["area"].text["top"]
		.setText(L"↑顶部，无法继续上拉")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 25))
		.setCenter();

	windowManager.preset["main"].area["area"].text["text"]
		.setText(L"这是内层窗口，被设置为可拖动\n现已支持惯性滑动")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 100))
		.setCenter();
	
	windowManager.preset["main"].area["area"].input["input"]
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
	windowManager.preset["main"].area["area"].input["input"]
		.setSizeAuto()
		.setCenter();

	windowManager.preset["main"].area["area"].button["button"]
		.setText(L"按钮").setJustification(attr::gui::Mid, attr::gui::Mid)
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 275))
		.setSize(sf::Vector2f(100, 50))
		.setCenter();

	windowManager.preset["main"].area["area"].text["1"]
		.setText(L"1")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 700))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	windowManager.preset["main"].area["area"].text["2"]
		.setText(L"2")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 1000))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	windowManager.preset["main"].area["area"].text["3"]
		.setText(L"3")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 1300))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	windowManager.preset["main"].area["area"].text["4"]
		.setText(L"4")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 1600))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	windowManager.preset["main"].area["area"].text["5"]
		.setText(L"5")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 1900))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();

	windowManager.preset["main"].area["area"].text["6"]
		.setText(L"6")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 700))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	windowManager.preset["main"].area["area"].text["7"]
		.setText(L"7")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 1000))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	windowManager.preset["main"].area["area"].text["8"]
		.setText(L"8")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 1300))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	windowManager.preset["main"].area["area"].text["9"]
		.setText(L"9")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 1600))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	windowManager.preset["main"].area["area"].text["10"]
		.setText(L"10")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(1000, 1900))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();
	windowManager.preset["main"].area["area"].text["bottom"]
		.setText(L"↓底部")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 2200))
		.setSize(sf::Vector2f(150, 50))
		.setCenter();

	windowManager.preset["main"].area["area"]
		.setScrollLimitAuto();
}
game::Event evt;
int main() {
	game::fontManager.loadFont("ht", "FZHTJW.TTF");
	init();
	/*BinaryFStream bf("D:\\1.bin");
	windowManager.readPreset(bf);*/
	windowManager.newWindow("main");
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
				cout << "Button pressed : " << evt[attr::gui::ButtonPath].cast<string>() << endl;
			}
			if (evt.eventId == attr::gui::InputGainFocus) {
				cout << "Input gain focus : " << evt[attr::gui::InputPath].cast<string>() << endl;
			}
			if (evt.eventId == attr::gui::InputLoseFocus) {
				cout << "Input lose focus : " << evt[attr::gui::InputPath].cast<string>() << endl;
			}
		}
		game::window.clear();
		windowManager.draw(game::window);
		game::window.display();
	}
	return 0;
}