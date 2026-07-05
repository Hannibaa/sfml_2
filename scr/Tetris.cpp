#include "imgui.h"
#include "imgui-SFML.h"
#include <thread>
#include <SFML/Graphics.hpp>
#include "../include/utility.hpp"


/*
								0	1
								2	3
								4	5
								6	7
		Implementation :
		1. make ghost termino
		2. previous termino
		3. score of lines 1, 2, 3, 4.
		4. game parameters


*/


struct Point {
	int x, y;
};


const int					M = 20;
const int					N = 10;
int							field[M][N]{0};

int							shapes[7][4] =
{
	1, 3, 5, 7, // I	0
	2, 4, 5, 7, // Z	1
	3, 5, 4, 6, // S	2
	3, 5, 4, 7, // T	3
	2, 3, 5, 7, // L	4
	3, 5, 7, 6, // J	5
	2, 3, 4, 5, // O	6
};

void piece(Point(&a)[4], int k) {
	for (int i = 0; i < 4; i++)
	{
		a[i].x = (shapes[k][i] % 2);
		a[i].y = (shapes[k][i] / 2);
	}
}

void get_new(Point (&a)[4],int& color, int k) {
	util::iRG<int>			irand;

	int x0 = irand(0, N - 2);

	for (int i = 0; i < 4; i++)
	{
		a[i].x = (shapes[k][i] % 2) + x0;
		a[i].y = (shapes[k][i] / 2);
	}

	color = irand(1, 7);
}

void get_new(Point(&a)[4], int k) {
	util::iRG<int>			irand;

	int x0 = irand(0, N - 2);

	for (int i = 0; i < 4; i++)
	{
		a[i].x = (shapes[k][i] % 2) + x0;
		a[i].y = (shapes[k][i] / 2);
	}
}

void clear() {
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
			field[j][i] = 0;
}


bool check(Point *a) {
	for (int i = 0; i < 4; ++i)
		if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) return 0;
		else
			if (field[a[i].y][a[i].x]) return 0;

	return 1;
}

void a_plusX(Point* a, int k) {
	for (int i = 0; i < 4; ++i) a[i].x += k;
}
void a_plusY(Point* a, int k) {
	for (int i = 0; i < 4; ++i) a[i].y += k;
}

void equal_point(Point* a, Point* b) {
	for (int i = 0; i < 4; ++i) a[i] = b[i];
}

void rotation_ajust(Point* a, Point* b) {
	for (int i = 0; i < 4; ++i)
	{
		if (a[i].x < 0) { a_plusX(a, 1); equal_point(b, a); }
		else if (a[i].x >= N) { a_plusX(a, -1); equal_point(b, a); }
		else if (a[i].y >= M) { a_plusY(a, -1); equal_point(b, a); }
		else if (field[a[i].y][a[i].x]) { equal_point(a, b); }
	}
}


bool is_over() {
	for (int k = 0; k < N; ++k)
		if (field[1][k]) return true;

	return false;
}

class Next {
	int previous;
	int from;
	int to;

public:
	Next() :previous{}, from{ 0 }, to{ 6 } {}
	Next(int pr): previous(pr), from{ 0 }, to{ 6 } {}
	Next(int pr, int _from, int _to): previous(pr), from{ _from }, to{ _to } {}

	int prev() const {
		return previous;
	}

	int operator()() {
		util::iRG<int>	irand;
		int next = previous;
		previous = irand(from, to);
		return next;
	}
};

int main() {

	const float					WinWidth = 1200.f;
	const float					WinHeight = 800.f;

	const float					BlockWidth = 18.f;

	Point						a[4]{}, b[4]{}, g[4]{};
	int							colorNum{1};

	bool						rotate{ false };
	bool						b_is_over{ false };

	util::iRG<int>				irand;

	float						bgcolor[3]{0.f, 0.f, 1.f};
	int							n = 3;
	int							dx{};

	int							ScoreLine1{};
	int							ScoreLine2{};
	int							ScoreLine3{};
	int							ScoreLine4{};


	sf::RenderWindow			window(sf::VideoMode(WinWidth, WinHeight), "Tetris Game");
	ImGui::SFML::Init(window);

	sf::RectangleShape			GameRect, GhostRect;
	GameRect.setSize({ BlockWidth * N, BlockWidth * M });
	GameRect.setPosition(0.f, 0.f);
	GameRect.setOutlineColor(sf::Color::Red);
	GameRect.setFillColor(sf::Color::Blue);
	GameRect.setOutlineThickness(2.f);

	GhostRect.setSize({ BlockWidth, BlockWidth });
	GhostRect.setFillColor(sf::Color(200, 200, 200, 100));

	const std::string			file_tex("resource\\tetris.png");
	sf::Texture					tex;
	tex.loadFromFile(file_tex);

	sf::Sprite					spr(tex);
	spr.setTextureRect(sf::IntRect(0, 0, BlockWidth, BlockWidth));


	Next						next(3);
	Next						colorNext(1, 1, 7);

	util::mapBool				Option;

	// Set the map of all Option of game
	Option["Close"] = { false, "close the application" };
	Option["Rotate"] = { false, "rotate tetrimino" };
	Option["Pause"] = { false, "pause game until" };
	Option["Stop"] = { false, "stop tetrino from falling" };
	Option["Over"] = { false, "Game Over" };

	// imgui clock
	sf::Clock					iClock;
	sf::Clock					clock;

	sf::Clock					wClock;

	float						time_waiting{ 1.1f };
	int						    time_velocity{ 10 };
	int							PieceAccount[7]{};
	
	float						timer = 0.f;
	float						delay = 0.3f;
	float						delay_time_normal{ 0.3f };
	float						delay_time_speedy{ 0.05f };

	get_new(a, colorNum, next());
	// MAIN LOOP
	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		sf::Event	event;
		while (window.pollEvent(event))
		{
			// attach event to imgui
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed || Option["Close"]()) window.close();

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Up) rotate = true;

				if (event.key.code == sf::Keyboard::N) get_new(a, colorNum, next());

				if (event.key.code == sf::Keyboard::S) Option["Stop"].not_();

				if (event.key.code == sf::Keyboard::P) Option["Pause"].not_();

				if (event.key.code == sf::Keyboard::I) get_new(a, colorNum, 0);
				if (event.key.code == sf::Keyboard::Z) get_new(a, colorNum, 1);
				if (event.key.code == sf::Keyboard::S) get_new(a, colorNum, 2);
				if (event.key.code == sf::Keyboard::T) get_new(a, colorNum, 3);
				if (event.key.code == sf::Keyboard::L) get_new(a, colorNum, 4);
				if (event.key.code == sf::Keyboard::J) get_new(a, colorNum, 5);
				if (event.key.code == sf::Keyboard::O) get_new(a, colorNum, 6);
			}


		}// EVENT LOOP

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) delay = delay_time_speedy;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) dx = -1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dx = 1;

		ImGui::SFML::Update(window, iClock.restart());


		///////////////// Option Menu ///////////////////
		ImGui::Begin("Option Window");
		if (ImGui::Button("Close Application")) Option["Close"].not_();
		// set background color
		ImGui::ColorEdit3("Background Color", bgcolor);

		if (ImGui::Button("Clear")) {
			clear();
			get_new(a,colorNum, next());
		}

		if (ImGui::Button("Clear Score")) {
			ScoreLine1 = 0;
			ScoreLine2 = 0;
			ScoreLine3 = 0;
			ScoreLine4 = 0;
		}

		if (ImGui::Button("New One")) {
			get_new(a,colorNum, next());
		}

		if (ImGui::Button("Pause")) {
			Option["Pause"].not_();
		}

		if (ImGui::Button("Stop")) {
			Option["Stop"].not_();
		}

		ImGui::SliderInt("Chose", &n, 0, 6);
		ImGui::SliderFloat("Waiting Time", &time_waiting, 0.5f, 5.f);
		ImGui::SliderFloat("Delay Time", &delay_time_normal, 0.01f, 1.0f);
		ImGui::SliderFloat("Delay Time speedy", &delay_time_speedy, 0.f, 0.1f);
		ImGui::SliderInt("Time velocity", &time_velocity, 0, 400);

		ImGui::End();

		//////////////////// Score table //////////////////
		ImGui::Begin("Score Tetris");
		ImGui::Text("Stat : %s", b_is_over ? "Over" : "Not Over");
		ImGui::Text("Lines 1 : %d", ScoreLine1);
		ImGui::Text("Lines 2 : %d", ScoreLine2);
		ImGui::Text("Lines 3 : %d", ScoreLine3);
		ImGui::Text("Lines 4 : %d", ScoreLine4);
		ImGui::End();

		// movement
		if (!Option["Pause"]()) {

			for (int i = 0; i < 4; i++) {
				std::this_thread::sleep_for(std::chrono::milliseconds(time_velocity));
				b[i] = a[i]; a[i].x += dx;
			}

			if (!check(a)) for (int i = 0; i < 4; i++) a[i] = b[i];

			// rotation
			if (rotate)
			{
				Point p = a[1]; // center of rotation
				for (int i = 0; i < 4; i++) {
					int x = a[i].y - p.y;
					int y = a[i].x - p.x;
					a[i].x = p.x - x;
					a[i].y = p.y + y;
				}
				//if (!check(a)) for (int i = 0; i < 4; i++) a[i] = b[i];
				rotation_ajust(a, b);
			}


			///// Tick //////////////////////////////////////////////////////

			if (timer > delay)
			{
				for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].y += 1; }

				if (!check(a))
				{
					if (wClock.getElapsedTime().asSeconds() < time_waiting) equal_point(a, b);
					else {
						for (int i = 0; i < 4; i++) field[b[i].y][b[i].x] = colorNum;

						get_new(a, colorNum, next());

						wClock.restart();
					}
				}

				timer = 0;
			}

			// make g = a;
			equal_point(g, a);
			int gi{};
			while (check(g)) {
				a_plusY(g, 1);
			}
			a_plusY(g, -1);
		}

		//////// check line //////////////////
		int k = M - 1;
		int lines{};
		for (int i = M - 1; i > 0; i--)
		{
			int count = 0;
			for (int j = 0; j < N; j++)
			{
				if (field[i][j]) count++;
				field[k][j] = field[i][j];
			}

			if (count < N) k--;
			
			if (count == N) { 
				++lines; 
			}
		}

		// Score ///////////////////////////////
		if (lines != 0) {
			switch (lines) {
			case 1: ScoreLine1 += 1; break;
			case 2: ScoreLine2 += 1; break;
			case 3: ScoreLine3 += 1; break;
			case 4: ScoreLine4 += 1; break;
			}

			lines = 0;
		}


		dx = 0; rotate = false; delay = delay_time_normal;

		if (is_over()) b_is_over = true; else b_is_over = false;

		if (b_is_over) {
			clear(); get_new(a, colorNum, next());
		}

		GameRect.setFillColor(sf::Color(255 * bgcolor[0], 255 * bgcolor[1], 255 * bgcolor[2], 255));

		window.clear();

		window.draw(GameRect);

		for (int i = 0; i < M; ++i)
			for (int j = 0; j < N; ++j)
			{
				if (field[i][j] == 0) continue;
				spr.setTextureRect(sf::IntRect(field[i][j] * BlockWidth, 0, BlockWidth, BlockWidth));
				spr.setPosition(j * BlockWidth, i * BlockWidth);
				window.draw(spr);
			}

		Point np[4]{};
		int x0 = GameRect.getSize().x + 50;
		int y0 = 50;
		piece(np, next.prev());
		for (int i = 0; i < 4; i++)
		{
			spr.setTextureRect(sf::IntRect(colorNum * BlockWidth, 0, BlockWidth, BlockWidth));
			spr.setPosition(a[i].x * BlockWidth, a[i].y * BlockWidth);
			window.draw(spr);
			GhostRect.setPosition(g[i].x* BlockWidth, g[i].y* BlockWidth);
			window.draw(GhostRect);

			spr.setTextureRect(sf::IntRect(colorNum* BlockWidth, 0, BlockWidth, BlockWidth));
			spr.setPosition(np[i].x * BlockWidth + x0, np[i].y * BlockWidth + y0);
			window.draw(spr);
		}


		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}