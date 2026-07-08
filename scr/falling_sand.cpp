#include "imgui.h"
#include "imgui-SFML.h"
#include <thread>
#include <iostream>

#include "SFML/Graphics.hpp"
#include "../include/util-sfml.hpp"
#include "../include/grid.hpp"

using color_t = sf::Color;

const float				gridPosX = 10.f;
const float				gridPosY = 10.f;
const float				gridSizeX = 600.f;
const float				gridSizeY = 600.f;

const int				gridDivX = 40;
const int				gridDivY = 40;

const float				life_time{ 1.5f };

enum {
	NO,
	UP,
	DN,
	LF,
	RT,
	SIDE_LF,
	SIDE_RT
};

enum {
	Empty = 0,
	Sand = 1,
	Water = 2,
};

struct Particule_t {
	u32			id = Empty;
//	ivec2		p;
	f32			life_time = 0.f;
	fvec2		velocity = fvec2(0.f, 0.f);
	color_t		color = sf::Color::Transparent;
	bool		has_been_updated = true;
};
///////////////////// Grid Stat Info ///////////////////////

Particule_t		p[gridDivY][gridDivX]{};

void set_empty(int x, int y) {
	p[y][x].id = Empty;
	p[y][x].velocity = fvec2(0.f, 0.f);
	p[y][x].color = sf::Color::Transparent;
	p[y][x].has_been_updated = false;
}

/// set all empty
void set_all_empty() {
	for (int i = 0 ; i < gridDivX; ++i)
		for (int j = 0; j < gridDivY; ++j)
		{
			set_empty(i, j);
		}
}

/// Make one particuale
void make_particule(int x, int y, float vx, float vy, color_t color) {
	p[y][x].id = Sand;
	p[y][x].velocity = fvec2(vx, vy);
	p[y][x].color = color;
	p[y][x].has_been_updated = false;
	p[y][x].life_time = life_time;
}


void copy_from_to(int oldx, int oldy, int x, int y) {
	p[y][x].id = p[oldy][oldx].id;
	p[y][x].life_time = p[oldy][oldx].life_time;
	p[y][x].velocity = p[oldy][oldx].velocity;
	p[y][x].color = p[oldy][oldx].color;
}

Particule_t get_particule(int x, int y) {
	return p[y][x];
}

bool check(int x, int y) {

	if (p[y][x].id == Empty) return false;
	else
		if (x < 0 || x >= gridDivX - 1 || y > gridDivY - 1 || y < 0) return false;

	return true;
}

bool in_grid(int x, int y) {
	return (x < 0 || x > gridDivX - 1 || y > gridDivY - 1 || y < 0);
}

// Sand behavior
bool sand_behavior(int x, int y, int& direction) {
	// check there some think down
	if (p[y + 1][x].id == Empty) {
		direction = DN;
	}
	else if (x > 0 && p[y + 1][x - 1].id == Empty) {
		direction = SIDE_LF;
	}
	else if (x < gridDivX - 1 && p[y + 1][x + 1].id == Empty) {
		direction = SIDE_RT;
	}
	else {
		p[y][x].has_been_updated = true;
		return false;
	}

	return true;
}

bool water_behavior(int x, int y, int& direction) {
	// check there some think down
	if (p[y + 1][x].id == Empty) {
		direction = DN;
	}
	else if (x > 0 && p[y][x - 1].id == Empty) {
		direction = LF;
	}
	else if (x < gridDivX - 1 && p[y][x + 1].id == Empty) {
		direction = RT;
	}
	else {
		p[y][x].has_been_updated = true;
		return false;
	}

	return true;
}

void update(int x, int y, int direction, int) {
	if (p[y + 1][x].id != Empty) {
		p[y][x].velocity = fvec2(0.f, 0.f);
		p[y][x].has_been_updated = true;
		return;
	}

	if (p[y][x].id == Empty) return;
	if (p[y][x].has_been_updated == true) return;
	if (p[y][x].velocity == fvec2(0.f, 0.f)) {
		if(p[y + 1][x].id == Empty) 
			p[y][x].velocity = fvec2(1.f, 1.f);
	}

	int oldx = x;
	int oldy = y;

	switch (direction) {
	case UP: y -= 1;break;
	case DN: y += 1;break;
	case LF: x -= 1; break;
	case RT: x += 1; break;
	}

	p[y][x].has_been_updated = true;
	copy_from_to(oldx, oldy, x, y);
	set_empty(oldx, oldy);
}

void update2(int x, int y, int direction, int mat_id) {
	// check every thing inside matrix of grid
	if (in_grid(x, y)) return;

	if (p[y][x].id == Empty) return;
	if (p[y][x].has_been_updated == true) return;

	// reach edge y = gridDivX
	if (y > gridDivY - 2) {
		p[y][x].has_been_updated = true;
		return;
	}

	switch (mat_id) {
	case Sand:if (sand_behavior(x, y, direction)); else return; break;
	case Water:if (water_behavior(x, y, direction)); else return; break;
	}

	int oldy = y;
	int oldx = x;

	switch (direction) {
	case UP: y -= 1;break;
	case DN: y += 1;break;
	case LF: x -= 1; break;
	case RT: x += 1; break;

	case SIDE_LF:y += 1; x -= 1; break;
	case SIDE_RT:y += 1; x += 1; break;
	}

	copy_from_to(oldx, oldy, x, y);
	p[y][x].has_been_updated = true;
	set_empty(oldx, oldy);
}

void set_new_stat() {
	for (int j = 0; j < gridDivY; ++j)
		for (int i = 0; i < gridDivX; ++i)
		{
			if (p[j][i].id != Empty)
			p[j][i].has_been_updated = false;
		}
	
}

void update() {

	for (int j = gridDivY - 2; j >= 0 ; j--)
		for (int i = 0; i < gridDivX; ++i)
		{
			update2(i, j, DN,Sand);
		}

	set_new_stat();
}

void general_update(void (*fupdate)(int, int, int, int), int mat_id = Sand) {
	for (int j = gridDivY - 2; j >= 0; j--)
		for (int i = 0; i < gridDivX; ++i)
		{
			fupdate(i, j, DN, mat_id);
		}
	set_new_stat();
}


int main() {

	const int				WinWidth{ 1200 };
	const int				WinHight{ 800 };


	sf::RenderWindow		window(sf::VideoMode(WinWidth, WinHight), "Sand Falling Simulator");
	ImGui::SFML::Init(window);
	// ImGui Size character
	ImGui::GetIO().FontGlobalScale = 1.3f;

	// //// BOOL ////////////////////

	util::mapBool			Bool;

	Bool["Close"] = { false, "Close application main loop" };
	Bool["Update"] = { true, "Update sand falling" };
	Bool["Random"] = { false, "Continue random emission" };
	Bool["RandomColor"] = { false, "Chose random colors" };
	Bool["MouseAsync"] = { true, "Enable mouse throwing sand method" };
	Bool["EnableGrid"] = { true, "Enable Grid showing" };
	Bool["SandUpdate"] = { true, "Enable sand falling simulation" };
	Bool["WaterUpdate"] = { false, "Enable Water simulation" };

	// definition of grid
	util::Grid				grid(sf::FloatRect(gridPosX, gridPosY, gridSizeX, gridSizeY), gridDivX, gridDivY);

	float					unitX = gridSizeX / gridDivX;
	float					unitY = gridSizeY / gridDivY;

	float					cc[3]{ 0.f,1.f, 0.f };

	sf::Color				color[9]{
		sf::Color::Black,
		sf::Color::White,
		sf::Color::Blue,
		sf::Color::Red,
		sf::Color::Green,
		sf::Color::Cyan,
		sf::Color::Magenta,
		sf::Color::Yellow,
		sf::Color(200,200,200,255)
	};

	sf::RectangleShape		rect({ unitX, unitY });
	int						mat_id{ Sand };
	
	util::iRG<int>			irand;

	sf::Clock				iclock;
	sf::Clock				clock;
	sf::Clock				clock2;
	float					delay{ 0.02f };
	float					delay2{ 0.1f };
	fvec2					fmouse;

	while (window.isOpen()) {

		float time = clock.getElapsedTime().asSeconds();

		sf::Event	event;
		while (window.pollEvent(event))
		{
			fmouse = util::as<float>( sf::Mouse::getPosition(window) );

			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed || Bool["Close"]()) window.close();



			/// ADD SAND WITH MOUSE ONE BY ONE
			if (event.type == sf::Event::MouseButtonPressed && 
				!Bool["MouseAsync"]()) {

				if (event.mouseButton.button == sf::Mouse::Left) {
					auto npos = grid.getGridPosition(fmouse);
					if (!npos.empty())
					make_particule(npos[0].x, npos[0].y, 1.f, 1.f,
						sf::Color(cc[0] * 255, cc[1] * 255, cc[2] * 255));
				}
			}



		}

		////// ADD SAND WITH MOUSE ASYNCRONOUS
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
			Bool["MouseAsync"]()) {
			auto npos = grid.getGridPosition(fmouse);
			if (!npos.empty())
				make_particule(npos[0].x, npos[0].y, 1.f, 1.f,
					sf::Color(cc[0] * 255, cc[1] * 255, cc[2] * 255));
		}

		ImGui::SFML::Update(window, iclock.restart());


		// First Menu
		ImGui::Begin("Main Option");
		ImVec2 winSize = ImGui::GetWindowSize();
		auto winPos = ImGui::GetWindowPos();

		if (ImGui::Button("Close", ImVec2(winSize.x - 20.f, 0))) Bool["Close"].not_();
		ImGui::Text("Windows Pos[ %.2f, %.2f ], Size[%.2f|%.2f]", winPos.x, winPos.y, winSize.x, winSize.y);

		if (ImGui::Button("New Particule")) {
			make_particule(irand(0,gridDivX - 1), 0, 1.f, 1.f, sf::Color::White);
		}

		if (ImGui::Button("New Particule speedy")) {
			make_particule(irand(0, gridDivX - 1), 0, 2.f, 2.f, sf::Color::Red);
		}

		if (ImGui::Button("Clear")) {
			set_all_empty();
		}

		ImGui::SliderFloat("Falling speed", &delay, 0.01f, 2.f);
		ImGui::SliderFloat("Delay between sand emition", &delay2, 0.01f, 2.f);

		ImGui::Checkbox("UnUpdate game", &Bool["Update"]._action);
		ImGui::Checkbox("Continue Random", &Bool["Random"]._action);
		ImGui::Checkbox("Random Color", &Bool["RandomColor"]._action);
		ImGui::Checkbox("(D)Enable Mouse Asyn", &Bool["MouseAsync"]._action);
		ImGui::Checkbox("Enable Grid", &Bool["EnableGrid"]._action);
		ImGui::Checkbox("Enable Sand", &Bool["SandUpdate"]._action);
		ImGui::Checkbox("Enable Water", &Bool["WaterUpdate"]._action);
		

		ImGui::Text("Elapsed Time %.2f", time);

		ImGui::ColorEdit3("Cell Color", cc);

		ImGui::End();

		////////// UPDATE /////////////////////



		if (Bool["Update"]()) {
			if (time > delay) {
				if (Bool["SandUpdate"]())
					general_update(update2, Sand);
				else if (Bool["WaterUpdate"]())
					general_update(update2, Water);
				else
					general_update(update, mat_id);

				//update();
				clock.restart();
			}
		}




		if (Bool["Random"]()) {
			if (clock2.getElapsedTime().asSeconds() > delay2) {

				sf::Color _col;
				if (Bool["RandomColor"]()) {
					_col = color[irand(0, 8)];
				}
				else
					_col = sf::Color(cc[0] * 255, cc[1] * 255, cc[2] * 255);

				make_particule(irand(0, gridDivX - 1), 0, 1.f, 1.f, _col);
				clock2.restart();
			}
		}



		window.clear();

		if (Bool["EnableGrid"]())
			window.draw(grid);

		for (int i = 0; i < gridDivX; ++i)
			for (int j = 0; j < gridDivY; ++j)
			{
				if (p[j][i].id != Empty )
				{
					rect.setPosition(gridPosX + i * unitX, gridPosY + j * unitY);
					rect.setFillColor(p[j][i].color);
					window.draw(rect);
				}
			}

		ImGui::SFML::Render(window);
		window.display();
	}


	ImGui::SFML::Shutdown();













	return 0;
}