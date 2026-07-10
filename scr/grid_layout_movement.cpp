#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include "../include/util-sfml.hpp"
#include "../include/grid.hpp"


enum {
	NO,
	UP,
	DN,
	LF,
	RT,
	SIDE_LF,
	SIDE_RT
};

struct Particule_t {
	fvec2			_p;
	fvec2			_v;
	int				_dir;
	sf::Color		_color;
};

Particule_t make_particule(float x, float y, float vx, float vy, const sf::Color& color) {
	Particule_t		m;
	m._p.x = x;
	m._p.y = y;
	m._v = fvec2(vx, vy);
	m._dir = 0;
	m._color = color;

	return m;
}

const int		Max_Coord = 30;

void update(float &x, float &y, float vx, float vy, int dir) {
	switch (dir) {
	case NO:break;
	case UP:y -= vy; break;
	case DN:y += vy; break;
	case LF:x -= vx; break;
	case RT:x += vx; break;
	case SIDE_LF:x -= vx; y += vy; break;
	case SIDE_RT:x += vx; y += vy; break;
	}

	util::clamp<float>(x, 0.f, Max_Coord - 1);
	util::clamp<float>(y, 0.f, Max_Coord - 1);
}

void update(Particule_t& p) {
	update(p._p.x, p._p.y, p._v.x, p._v.y, p._dir);
}


int main() {

	const i32			WinWidth = 1200;
	const i32			WinHeight = 800;
	const f32			gridWidth = 600;
	const f32			gridHeight = 600;

	const fvec2			gridPos(10.f, 10.f);

	const i32			gridcells = Max_Coord;
	const f32			gridlength = (f32)gridWidth / (f32)gridcells;
	const f32			imguiFontScale = 1.4f;


	sf::RenderWindow	window(sf::VideoMode(WinWidth, WinHeight), "Cellular Automata");
	ImGui::SFML::Init(window);
	ImGui::GetIO().FontGlobalScale = imguiFontScale;


	util::Grid					grid(sf::FloatRect(gridPos.x, gridPos.y, gridWidth, gridHeight), gridlength);

	sf::RectangleShape			box({ gridlength, gridlength });
	box.setFillColor(sf::Color::White);
	box.setOutlineColor(sf::Color::Black);


	util::mapBool			Bool;

	Bool["Async"] = { false, "Enable asyncronous key" };
	Bool["Random"] = { false, "Enable random walk" };
	Bool["Desable"] = { false, "Desable updating" };

	Bool["Up"] = { false, "Up" };
	Bool["Down"] = { false, "Down" };
	Bool["Left"] = { false, "Left" };
	Bool["Right"] = { false, "Right" };


	auto Reset_All_DirKey = [&]() {
		Bool["Up"]() = false;
		Bool["Down"]() = false;
		Bool["Left"]() = false;
		Bool["Right"]() = false;
		};

	Particule_t p1 = make_particule(0.f, 0.f, 0.05f, 0.05f, sf::Color::Red);

	float bx{}, b2x{10.f};
	float by{}, b2y{10.f};

	fvec2 v(0.04f, 0.04f), v2(0.03f, 0.03f);
	int		dir{ NO }, dir2{};

	util::iRG<int>	 rand;

	sf::Clock		clock;
	float			delay{ 0.2f };

	sf::Clock		iclock;
	/////// MAIN LOOP ///
	while (window.isOpen()) {

		sf::Event	event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed) window.close();


			if (!Bool["Async"]() && event.type == sf::Event::KeyPressed) {

				if (event.key.code == sf::Keyboard::Left) dir = LF;

				if (event.key.code == sf::Keyboard::Right) dir = RT;

				if (event.key.code == sf::Keyboard::Up) dir = UP;

				if (event.key.code == sf::Keyboard::Down) dir = DN;
			}

		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) Bool["Left"]() = true;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) Bool["Right"]() = true;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) Bool["Up"]() = true;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) Bool["Down"]() = true;

		if (Bool["Async"]()) {
			Bool["Left"]() ? dir = LF :
			Bool["Right"]() ? dir = RT : 
			Bool["Down"]() ? dir = DN : 
			Bool["Up"]() ? dir = UP : 1/*dir = NO*/;
		}

		if (Bool["Random"]()) {
			if (clock.getElapsedTime().asSeconds() > delay) {
				clock.restart();
				dir2 = rand(0, 4);
				p1._dir = rand(0, 4);
			}
		}

		//if (Bool["Async"]()) {
		//	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) dir = LF;
		//	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir = RT;
		//	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) dir = UP;
		//	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) dir = DN;
		//}

		ImGui::SFML::Update(window, iclock.restart());




		if (!Bool["Desable"]()) {
			update(bx, by, v.x, v.y, dir);
			update(b2x, b2y, v2.x, v2.y, dir2);
			update(p1);
			Reset_All_DirKey();
		}

		if (int(bx) == int(b2x) && int(by) == int(b2y)) {
			Bool["Desable"]() = true;
			bx = 0.f; by = 0.f;
		}

		ImGui::Begin("Option");
		if (ImGui::Button("Close")) window.close();
		ImGui::Text("Cell[%d] [%d]", int(bx), int(by));
		ImGui::Text("Cell2[%d] [%d]", int(b2x), int(b2y));
		ImGui::SliderFloat("Velocity X", &v.x, 0.f, 10.f);
		ImGui::SliderFloat("Velocity Y", &v.y, 0.f, 10.f);
		ImGui::SliderFloat("Velocity X2", &v2.x, 0.f, 10.f);
		ImGui::SliderFloat("Velocity Y2", &v2.y, 0.f, 10.f);
		ImGui::SliderFloat("Delay random", &delay, 0.f, 1.f);
		ImGui::Checkbox("Enable Async", &Bool["Async"]._action);
		ImGui::Checkbox("Enable Random walk", &Bool["Random"]._action);
		ImGui::Checkbox("Desable updating move", &Bool["Desable"]());

		ImGui::End();



		window.clear();

		box.setPosition(gridPos.x + int(bx) * gridlength, gridPos.y + int(by) * gridlength);
		box.setFillColor(sf::Color::White);
		window.draw(box);
		box.setPosition(gridPos.x + int(b2x) * gridlength, gridPos.y + int(b2y) * gridlength);
		box.setFillColor(sf::Color::Red);
		window.draw(box);

		box.setPosition(gridPos.x + int(p1._p.x) * gridlength, gridPos.y + int(p1._p.y) * gridlength);
		box.setFillColor(p1._color);
		window.draw(box);

		window.draw(grid);

		ImGui::SFML::Render(window);
		window.display();
	}



	ImGui::SFML::Shutdown();





	return 0;
}