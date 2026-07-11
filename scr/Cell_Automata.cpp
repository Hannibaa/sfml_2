#include "imgui.h"
#include "imgui-SFML.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include "../include/util-sfml.hpp"
#include "../include/grid.hpp"




const int		MAX_CELL_X = 60;
const int		MAX_CELL_Y = 50;
const int		UNIT_LENGTH = 16;

#define			mx	MAX_CELL_X
#define			my	MAX_CELL_Y

using Cell_t = int[MAX_CELL_Y][MAX_CELL_X];

bool in_bound(int x, int y) {
	return x >= 0 && x < MAX_CELL_X && y >= 0 && y < MAX_CELL_Y;
}

int BITS(int l, int c, int r) {
	l = l & 1;
	c = c & 1;
	r = r & 1;

	return r + c << 1 + l << 2;
}

std::string make_random_pattern(u32 size) {
	util::iRG<i32>		urand;
	std::string			pattern;
	pattern.resize(size);

	for (auto& s : pattern) s = urand(0, 1) ? ' ' :'#';

	return pattern;
}

typedef		void(*strategy_t)(Cell_t&, Cell_t&, int, int, i8);

void set(Cell_t& stat, int x, int y, const std::string& lay_out) {

	if (!in_bound(x, y)) return;

	int i{};
	for (auto c : lay_out) {
		stat[y][x + i] = c == '#' ? 1 : 0;
		++i;
		if (!in_bound(x + i, y)) break;
	}
}

void set(Cell_t& stat, int x, int y, u32 lay_out) {
	if (!in_bound(x, y)) return;

	int i{};
	for (; ;) {
		stat[y][x + i] = util::Bit(lay_out, i);
		++i;
		if (!in_bound(x + i, y) || i >= sizeof(lay_out) ) break;
	}

}

void set_random(Cell_t& stat) {
	util::iRG<int>		irand;

	for (int i = 0; i < mx; ++i)
		for (int j = 0; j < my; ++j)
			stat[j][i] = irand(0, 1);
}

void reset_zero(Cell_t& cell, int value = 0) {
	if (value != 0 && value != 1) return;

	for (int i = 0; i < mx; ++i)
		for (int j = 0; j < my; ++j)
			cell[j][i] = value;
}


void strategy_0(Cell_t& cell, Cell_t& stat, int i, int j, i8) {
	int nNeibors =
		cell[j - 1][i - 1] + cell[j - 1][i] + cell[j - 1][i + 1] +
		cell[j][i - 1] + 0 + cell[j][i + 1] +
		cell[j + 1][i - 1] + cell[j + 1][i] + cell[j + 1][i + 1];

	if (cell[j][i] == 1)
		stat[j][i] = nNeibors == 2 || nNeibors == 3;
	else
		stat[j][i] = nNeibors == 3;
}

void strategy_1(Cell_t& cell, Cell_t& stat, int i, int j, i8) {
	
	int C = cell[j][i];
	int L = cell[j][i - 1];
	int R = cell[j][i + 1];
	int Stat = 0;

	switch (BITS(L, C, R)) {
	case 0: Stat = 0; break;
	case 1: Stat = 1; break;
	case 2: Stat = 1; break;
	case 3: Stat = 1; break;
	case 4: Stat = 0; break;
	case 5: Stat = 1; break;
	case 6: Stat = 1; break;
	case 7: Stat = 0; break;
	}

	stat[j][i] = Stat;
}

void strategy_2(Cell_t& cell, Cell_t& stat, int i, int j, i8 pattern) {

	int C = cell[j][i];
	int L = cell[j][i - 1];
	int R = cell[j][i + 1];
	int Stat = util::Bit(pattern, BITS(L,C,R));

	stat[j][i] = Stat;
}

void strategy_3(Cell_t& cell, Cell_t& stat, int i, int j, i8 pat) {
	int nNeibors =
		util::Bit(pat, 0) * cell[j - 1][i - 1] + util::Bit(pat, 1) *cell[j - 1][i] + util::Bit(pat,2) * cell[j - 1][i + 1] +
		util::Bit(pat,3) * cell[j][i - 1] + 0 + util::Bit(pat,4) * cell[j][i + 1] +
		util::Bit(pat, 5) * cell[j + 1][i - 1] + util::Bit(pat, 6) * cell[j + 1][i] + util::Bit(pat, 7) * cell[j + 1][i + 1];

	if (cell[j][i] == 1)
		stat[j][i] = nNeibors == 2 || nNeibors == 3;
	else
		stat[j][i] = nNeibors == 3;
}

void strategy_4(Cell_t& cell, Cell_t& stat, int i, int j, i8 pattern) {

	int C = cell[j][i];
	int L = cell[j][i - 1];
	int R = cell[j][i + 1];
	int nNeibors = L + R;
	int Stat = util::Bit(pattern, BITS(L,C,R));
	if (C == 1) Stat = nNeibors == 2 || nNeibors == 1;
	else Stat = nNeibors == 1;

	stat[j][i] = Stat;
}



void update(strategy_t strategy, Cell_t& cell, Cell_t& stat, int i, int j, i8 pattern) {
	strategy(cell, stat, i, j, pattern);
}


int main() {

	auto pat = make_random_pattern(13);
	std::cout << "pattern |" << pat << "\n";

	const i32			WinWidth = 1200;
	const i32			WinHeight = 800;
	const f32			gridWidth = MAX_CELL_X * UNIT_LENGTH;
	const f32			gridHeight = MAX_CELL_Y * UNIT_LENGTH;

	const fvec2			gridPos(0.f, 0.f);

	const i32			gridcells = MAX_CELL_X;
	const f32			gridlength = f32(UNIT_LENGTH);
	const f32			FontScale = 1.4f;

	util::iRG<int>		irand;

	sf::RenderWindow	window(sf::VideoMode(WinWidth, WinHeight), "Cell Automata");
	ImGui::SFML::Init(window);
	//ImGui::GetIO().FontGlobalScale = FontScale;


	util::mapBool		Bool;
	Bool["imguiShow"] = { true, "Show window imgui" };
	Bool["Run"] = { true, "Run the strategy" };
	Bool["Grid"] = { true, "Show grid" };

	// grid
	util::Grid			grid(MAX_CELL_X, MAX_CELL_Y, UNIT_LENGTH);
	grid.setPosition(gridPos);
	ivec2				gnum{};

	int					cell[MAX_CELL_Y][MAX_CELL_X]{};
	int					stat[MAX_CELL_Y][MAX_CELL_X]{};
	set_random(stat);

	strategy_t			func = nullptr;
	int					strategy_n{};
	i32					pattern{ 0b1101110 };

	Box_t				box;
	box.setSize(fvec2(UNIT_LENGTH, UNIT_LENGTH));
	color_t				bc[2]{ color_t::Black, color_t::White };

	fvec2				fmouse{};
	ivec2				imouse{};
	sf::Clock			iclock;
	sf::Clock			fpsClock;
	sf::Clock			updateClock;
	sf::Clock			delayClock;
	float				delay{ 0.001f };

	float				fps;
	float				max_fps{};
	float				min_fps{};
	float				fps5sec{};
	float				elapsed_time = 1.f;
	// MAIN LOOP
	while (window.isOpen()) {
		util::get_maximum(max_fps, fps);
		util::get_minimum(min_fps, fps);
		fps = 1.f / elapsed_time;
		elapsed_time = fpsClock.getElapsedTime().asSeconds(); 
		fpsClock.restart();

		util::update_every(updateClock, 3.f, [&fps5sec, fps]() {fps5sec = fps; });

		sf::Event	event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed) window.close();

			imouse = sf::Mouse::getPosition(window);
			fmouse = util::as<float>(imouse);

			if (event.type == sf::Event::KeyPressed) {

				if (event.key.code == sf::Keyboard::A && event.key.control) {
					Bool["imguiShow"].not_();
				}
			}

			if (event.type = sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					auto n2 = grid.getGridPosition(fmouse);
					if (!n2.empty()) {
						gnum = n2[0];
						stat[n2[0].y][n2[0].x] = 1;
					}
				}
			}

		} // Event Loop

		ImGui::SFML::Update(window, iclock.restart());

		// ALL IMGUI Menu here Begin

		if (Bool["imguiShow"]()) {
			ImGui::Begin("Option");
			ImGui::Text("FPS[ %.2f ] ELAPSED[%2.4f]", fps, elapsed_time);
			ImGui::Text("FPS[ %.2f ] ", fps5sec);
			ImGui::Text("Grid Number[%d][%d]", gnum.x, gnum.y);
			ImGui::Text("Max[%.2f]   Min[%.2f]", max_fps, min_fps);
			//if (ImGui::Button("Run")) Bool["Run"].not_();
			if (ImGui::Button("Reset")) {
				reset_zero(stat);
				set_random(stat);
			}
			if (ImGui::Button("PreSet")) {
				reset_zero(stat);
				set(stat, 10, MAX_CELL_Y / 2, "########.#####...###......#######.#####");
			}
			if (ImGui::Button("Clear")) {
				reset_zero(stat); reset_zero(cell);
			}
			ImGui::Checkbox("Show Grid", &Bool["Grid"]());
			ImGui::Checkbox("Run", &Bool["Run"]());
			ImGui::SliderFloat("Delay", &delay, 0.f, 0.01f);
			if (ImGui::SliderInt("Strategy N", &strategy_n, 0, 5)) {
				switch (strategy_n) {
				case 0: func = strategy_0; break;
				case 1: func = strategy_1; break;
				case 2: func = strategy_2; break;
				case 3: func = strategy_3; break;
				case 4: func = strategy_4; break;
				}
			}

			ImGui::SliderInt("Pattern", &pattern, 0, 255);

			ImGui::End();
		}


		// ALL IMGUI Menu here End








		window.clear();

		for (int i = 0; i < mx; ++i)
			for (int j = 0; j < my; ++j)
				cell[j][i] = stat[j][i];

		for (int i = 0; i < mx; ++i)
			for (int j = 0; j < my; ++j)
			{
				if (delayClock.getElapsedTime().asSeconds() > delay && Bool["Run"]()) {
					if (func)
					update(func, cell, stat, i, j, static_cast<i8>(pattern));
					delayClock.restart();
				}

				box.setPosition(gridPos + (fvec2(i, j)) * f32(UNIT_LENGTH));
				box.setFillColor(bc[cell[j][i]]);
				window.draw(box);

			}
		if (Bool["Grid"]())
		window.draw(grid);
		ImGui::SFML::Render(window);
		window.display();
	}


	ImGui::SFML::Shutdown();






	return 0;
}