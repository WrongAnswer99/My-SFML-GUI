//Author : WrongAnswer99
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <any>
#include <vector>
#include <unordered_map>
#include <queue>
//#define LET_ME_SEE_SEE
#include "GUI.cpp"
unordered_map<string, game::gui::WindowManager::AreaObj>window;
unordered_map<string, game::gui::WindowManager::ObjBase::Style>style;

int windowWidth = 800, windowHeight = 600;
static void init() {
	game::fontManager.loadFont("ht", "FZHTJW.TTF");
	
	style["stda1"].set(sf::Color::White, sf::Color(200, 200, 200), 2, Skip);
	style["stda2"].set(sf::Color(240,240,240), sf::Color(200, 200, 200), 2, Skip);
	style["stdbn"].set(sf::Color(250, 250, 250), sf::Color(200, 200, 200), 2, sf::Color::Black);
	style["stdbo"].set(sf::Color(220, 220, 220), sf::Color(200, 200, 200), 2, sf::Color::Black);
	style["stdbf"].set(sf::Color(200, 200, 200), sf::Color(150, 150, 150), 2, sf::Color::Black);

	window["main"]
		.setScrollable(false,false)
		.setStyle(style["stda1"], style["stda1"], style["stda1"])
		.setPosition(sf::Vector2f(0, 0))
		.setSize(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
	window["main"].button("button")
		.setText(L"按钮")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(650, 500))
		.setSize(sf::Vector2f(100, 50));
	
	window["main"].text("text")
		.setText(L"这是外层窗口，被设置为不可拖动")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(400, 25))
		.setCenter();

	window["main"].area("area").text("text2")
		//.setText(L"↓↓↓向下滑↓↓↓\n当按下按钮时拖动，\n操作将转化为拖动，\n且不会触发按钮\n(文本框同理)")
		.setText(L"↓↓↓向下滑↓↓↓\n当按下按钮时拖动，\n将保持按下按钮的状态拖动\n(文本框同理)")
		.setFont("ht")
		.setCharacterSize(50)
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
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 25))
		.setCenter();

	window["main"].area("area").text("text")
		.setText(L"这是内层窗口，被设置为可拖动\n现已支持惯性滑动")
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 100))
		.setCenter();
	
	window["main"].area("area").input("input")
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
	window["main"].area("area").input("input")
		.setSizeSyncText()
		.setCenter();

	window["main"].area("area").button("button")
		.setText(L"按钮").setJustification(attr::gui::Mid, attr::gui::Mid)
		.setFont("ht")
		.setCharacterSize(50)
		.setPosition(sf::Vector2f(350, 275))
		.setSize(sf::Vector2f(100, 50))
		.setCenter();

	window["main"].area("area").text("bottom")
		.setText(L"↓底部")
		.setFont("ht")
		.setCharacterSize(50)
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
		//windowManager.button("main_area_button").getStatu()
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