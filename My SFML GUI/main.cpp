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
#include "GUI.cpp"

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
		.setText(L"��ť")
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(650, 500))
		.setSize(sf::Vector2f(100, 50));
	
	window["main"].text("text")
		.setText(L"������㴰�ڣ�������Ϊ�����϶�")
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(400, 25))
		.setCenter();

	window["main"].area("area").text("text2")
		//.setText(L"���������»�������\n�����°�ťʱ�϶���\n������ת��Ϊ�϶���\n�Ҳ��ᴥ����ť\n(�ı���ͬ��)")
		.setText(L"���������»�������\n�����°�ťʱ�϶���\n�����ְ��°�ť��״̬�϶�\n(�ı���ͬ��)")
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
		.setText(L"���������޷���������")
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(350, 25))
		.setCenter();

	window["main"].area("area").text("text")
		.setText(L"�����ڲ㴰�ڣ�������Ϊ���϶�\n����֧�ֹ��Ի���")
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(350, 100))
		.setCenter();
	
	window["main"].area("area").input("input")
		.setText(L"����һ���ı���")
		.setJustification(attr::gui::Mid, attr::gui::Mid)
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(350, 225))
		.setSize(sf::Vector2f(400, 50));
	window["main"].area("area").input("input")
		.setSizeSyncText()
		.setCenter();

	window["main"].area("area").button("button")
		.setText(L"��ť").setJustification(attr::gui::Mid, attr::gui::Mid)
		.setGeneralStyle(Skip, Skip, Skip, Skip, "ht", 50, Skip, Skip)
		.setPosition(sf::Vector2f(350, 275))
		.setSize(sf::Vector2f(100, 50))
		.setCenter();

	window["main"].area("area").text("bottom")
		.setText(L"���ײ�")
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
	game::window.create(sf::VideoMode(sf::Vector2u(windowWidth,windowHeight)), L"����", sf::Style::Close, sf::State::Windowed);
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