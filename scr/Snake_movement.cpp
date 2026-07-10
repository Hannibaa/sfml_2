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


const int		MAX_CELL_X = 30;
const int		MAX_CELL_Y = 20;
const int		UNIT_LENGTH = 30;

size_t	MAX_SNAKE_SIZE = 2 * std::max<size_t>(MAX_CELL_X, MAX_CELL_Y);

using Snake_t = std::vector<ivec2>;

void make_snake(Snake_t& snake, size_t size) {
	snake.reserve(MAX_SNAKE_SIZE);
	snake.resize(size);
}

void set_snake_position(Snake_t& snake, const ivec2& p) {
	int i{};
	for (auto& s : snake) {
		s.x = p.x - i + snake.size() - 1;
		s.y = p.y;
		++i;
	}
}

void update_snake_position_tail(Snake_t& snake, const ivec2& new_position) {
	for (int i = snake.size() - 1; i != 0; --i) {
		snake[i] = snake[i - 1];
	}
	snake[0] = new_position;
}

void update(Snake_t& snake, int& dir, int Boundary_rules = 1) {
	static int old_dir = NO;
	bool updated = false;
	auto s = snake[0];
	auto old_s = snake[0];

	// check direction 
	if (dir == UP && old_dir == DN) dir = DN;
	if (dir == DN && old_dir == UP) dir = UP;
	if (dir == LF && old_dir == RT) dir = RT;
	if (dir == RT && old_dir == LF) dir = LF;

	switch (dir) {
	case NO:break;
	case UP:s.y -= 1; updated = true; break;
	case DN:s.y += 1; updated = true; break;
	case LF:s.x -= 1; updated = true; break;
	case RT:s.x += 1; updated = true; break;
	}

	if (dir != NO)
		old_dir = dir;
	
	if (Boundary_rules == 1) {

		if (s.x < 0 || s.x > MAX_CELL_X - 1) {
			dir = NO;
			updated = false;
			snake[0] = old_s;
		}
		if (s.y < 0 || s.y > MAX_CELL_Y - 1) {
			dir = NO;
			updated = false;
			snake[0] = old_s;
		}
	}

	if (Boundary_rules == 0) {
		if (s.x < 0) s.x = MAX_CELL_X - 1;
		if (s.x > MAX_CELL_X - 1) s.x = 0;
		if (s.y < 0) s.y = MAX_CELL_Y - 1;
		if (s.y > MAX_CELL_Y - 1) s.y = 0;
	}



	if (updated) {
		update_snake_position_tail(snake, s);
	}
}

bool check_game_rule(Snake_t& snake, ivec2& prey) {
	util::iRG<int>		irand;
	if (snake[0] == prey) {
		snake.push_back(snake[snake.size() -1]);
		prey = ivec2(irand(0, MAX_CELL_X - 1), irand(0, MAX_CELL_Y - 1));
		return true;
	}

	return false;
}

bool self_collision(const Snake_t& snake) {
	for (int i = 1; i < snake.size(); ++i)
	{
		if (snake[0] == snake[i]) return true;
	}

	return false;
}

void reset_snake(Snake_t& snake) {
	snake.resize(3);
	set_snake_position(snake, { 3,3 });
}

int main() {

	const i32			WinWidth = 1200;
	const i32			WinHeight = 800;
	const f32			gridWidth = MAX_CELL_X * UNIT_LENGTH;
	const f32			gridHeight = MAX_CELL_Y * UNIT_LENGTH;

	const fvec2			gridPos(10.f, 10.f);

	const i32			gridcells = MAX_CELL_X;
	const f32			gridlength = f32(UNIT_LENGTH);
	const f32			imguiFontScale = 1.4f;


	sf::RenderWindow	window(sf::VideoMode(WinWidth, WinHeight), "Cellular Automata");
	ImGui::SFML::Init(window);
	ImGui::GetIO().FontGlobalScale = imguiFontScale;


	util::Grid					grid(sf::FloatRect(gridPos.x, gridPos.y, gridWidth, gridHeight), gridlength);

	sf::RectangleShape			box({ gridlength, gridlength });
	box.setFillColor(sf::Color::White);
	box.setOutlineColor(sf::Color::Black);

	std::vector<ivec2>		snake;
	make_snake(snake, 3);
	sf::Color				snake_color = sf::Color::White;
	set_snake_position(snake, { 3,3 });

	ivec2					prey{ 10,10 };
	sf::Color				prey_color = sf::Color::Red;

	util::mapBool			Bool;

	Bool["Async"] = { true, "Enable asyncronous key" };
	Bool["Random"] = { false, "Enable random walk" };
	Bool["Desable"] = { false, "Desable updating" };
	Bool["PreyEnabled"] = { true, "Prey enabled" };
	Bool["EnableCollision"] = { true, "Enable self snake collision" };

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

	int				dir = NO;
	int				movement_rule{ 1 };

	util::iRG<int>	 rand;

	sf::Clock		clock;
	float			delay{ 0.08f };

	sf::Clock		rclock;
	float			rdelay{ 0.1f };

	sf::Clock		iclock;
	/////// MAIN LOOP ///
	while (window.isOpen()) {

		sf::Event	event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed) window.close();



		}


		if (Bool["Random"]()) {
			if (rclock.getElapsedTime().asSeconds() > rdelay) {
				rclock.restart();
				dir = rand(0, 4);
			}
		}

		if (Bool["Async"]()) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) dir = LF;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir = RT;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) dir = UP;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) dir = DN;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) dir = NO;
		}

		ImGui::SFML::Update(window, iclock.restart());



		if (!Bool["Desable"]()) {
			if (clock.getElapsedTime().asSeconds() > delay) {
				update(snake, dir, movement_rule);
				clock.restart();
			}
		}

		if (Bool["EnableCollision"]()) {
			if (self_collision(snake) ) {
				dir = NO;
			}
		}

		if (Bool["PreyEnabled"]()) {
			check_game_rule(snake, prey);
		}


		ImGui::Begin("Option");
		if (ImGui::Button("Close")) window.close();
		if (ImGui::Button("Reset")) { reset_snake(snake); dir = NO; }
		ImGui::SliderFloat("Delay random", &delay, 0.f, 1.f);
		ImGui::SliderInt("Movement rules number", &movement_rule, 0, 1);

		ImGui::Checkbox("Enable Async", &Bool["Async"]._action);
		ImGui::Checkbox("Enable Random walk", &Bool["Random"]._action);
		ImGui::Checkbox("Desable updating move", &Bool["Desable"]());

		ImGui::Text("Snake[0] Size[%d] Position [ %d | %d ]",snake.size(), snake[0].x, snake[0].y);
		ImGui::Text("Snake[1] Head Position [ %d | %d ]", snake[1].x, snake[1].y);
		ImGui::Text("Snake[2] Head Position [ %d | %d ]", snake[2].x, snake[2].y);
		ImGui::Text("Prey Position [ %d | %d ]", prey.x, prey.y);

		ImGui::End();



		window.clear();

		for (auto& p : snake) {
			box.setPosition(gridPos + util::as<float>(p) * gridlength);
			box.setFillColor(snake_color);
			window.draw(box);
		}

		box.setFillColor(prey_color);
		box.setPosition(gridPos + util::as<float>(prey) * gridlength);
		window.draw(box);

		window.draw(grid);

		ImGui::SFML::Render(window);
		window.display();
	}



	ImGui::SFML::Shutdown();





	return 0;
}