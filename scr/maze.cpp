/*
************************* ALMOST MAZE 01 ***********************************************
*			AUTHOR	:		AOUES KADDA
*			DATE	:		12 / 07 / 2026
*
*			DESIGN	:
*							Nx Cell
*				0	 1	  2	   3   ...					Nx - 1
*            +----+----+----+----+----+----+----+----+----+
*		0	 |	0 |  1 |  2 | 3  |    |    |    |    |    |
*            +----+----+----+----+----+----+----+----+----+
*		1	 |Nx  |    |    |    |    |    |    |    |    |
*            +----+----+----+----+----+----+----+----+----+
*		2	 |	  |    |    |    |    |    |    |    |    |  NZ(x,y) = x + Nx * y
*            +----+----+----+----+----+----+----+----+----+
*		3	 |	  |    |    |    |    |    |    |    |    |
*            +----+----+----+----+----+----+----+----+----+
*	Ny-1	 |	  |    |    |    |    |    |    |    |    |
*            +----+----+----+----+----+----+----+----+----+

			if x instead of y : NZ(x,y) = y + x * Ny;

					x-1	   x	x+1
						+----+
				y-1		|	 |
				   +----+----+----+
				y  |	|	 |	  |
				   +----+----+----+
				y+1		|	 |
						+----+

						Wall & Corner Cell Numbering
					  0	  1    2
						+----+
					  7	|    | 3     Wall = 1, 3, 5, 7
						+----+		 Corner = 0, 2, 4, 6
					  6	  5	   4


			: Using BakcSpace to back to possible last movement
			: Use Arrow keys to move red box to make maze
			: Save maze in json file to used.
			: Regenerate randomly a maze.
			: Generation is 100% for this i keep the code
			: add save function to saving in maze file

			SAVE IN FILE:
			maze is saved inside vector of integer size Nx.Ny and each integer
			represent cell stat in first 5 bits :
					  0	  1    2
						+----+
					  7	|    | 3     This cell(x,y) int : first bit for visited
						+----+		 four next bit for wall open or not
					  6	  5	   4	 0 : wall exist, 1 : wall not exist.

					  first bit from right indicate visited or no(0 wall exist, 1 visited)
					  second bit from right indicate wall 1 (1 not exist, 0 not)
					  third bit from right indicate wall 3
					  fourth bit from right indicate wall 5
					  fifth bit from right indicate wall 7

					  to retrive wall from cell(x,y) using function
					  WALL(cell(x,y)) = 0b1101

			FILE FORMAT
			"maze_" + datetimeserialnumber + ".maze" + Nx + "x" + Ny

			TODO :
			1. add costumed walk methode
			2. add costumed chosing neibers
			3. performed name of files

*/


#include "imgui.h"
#include "imgui-SFML.h"

#include <iostream>
#include <stack>
#include <functional>
#include <SFML/Graphics.hpp>
#include "../include/opendialogfile.h"
#include "../include/util-sfml.hpp"
#include "../include/box_s.hpp"
#include "../include/maze.h"



int main() {


	const i32			WinWidth = 1200;
	const i32			WinHeight = 800;

	const i32			Nx_Max{ 30 };
	const i32			Ny_Max{ 25 };

	const size_t		MAX_POISON{ 10 };

	const f32			FontScale = 1.5f;

	color_t				transparent = color_t::Transparent;
	color_t				red = color_t::Red;
	color_t				blue = color_t::Blue;
	color_t				black = color_t::Black;
	int					Nx{ 10 }, Ny{ 10 };
	f32					length{ 30.f };
	f32					thickness{ 2.f };
	f32					ilength = length - 2.f * thickness;

	util::iRG<int>		irand;

	sf::RenderWindow	window(sf::VideoMode(WinWidth, WinHeight), "Cell Automata");
	ImGui::SFML::Init(window);
	ImGui::GetIO().FontGlobalScale = FontScale;

	ImVec2				winPos(Nx_Max * length + 10, 0);
	ImVec2				winSize(WinWidth - winPos.x, WinHeight);
	
	fvec2				fmouse{};
	ivec2				imouse{};
	sf::Clock			iclock;
	sf::Clock			fpsClock;
	sf::Clock			updateClock;
	sf::Clock			delayClock;
	float				delay{ 0.01f };


	float				fps;
	float				max_fps{};
	float				min_fps{};
	float				fps5sec{};
	float				elapsed_time = 1.f;


	util::mapBool		Bool;
	Bool["Restart"] = { false, "restart a game" };
	Bool["Save"] = { false, "Open save dialogue box" };
	Bool["Open"] = { false, "Open new maze" };
	Bool["Resize"] = { false, "Resize the grid" };

	Bool["run"] = { false, "run maze making" };
	Bool["skip"] = { false, "accept only when ok" };
	Bool["play"] = { false, "start playing game" };

	maze::Maze			Maze(Nx, Ny, length, thickness, Bool["run"]._action,
														Bool["skip"]._action);

	std::string			maze_name("maze");
	 
	Box_t				player({ilength, ilength});
	player.setFillColor(color_t::Green);
	Maze.setPosition(player, 1, 1);

	std::vector<Box_t>		poison;
	std::vector<Box_t>	resource;
	int						life_time{ 100 };

	auto Create_Poison = [&](size_t amount) {
		if (amount > MAX_POISON) amount = MAX_POISON;
		poison.resize(amount);
		auto N = Maze.getSize();
		for (auto& p : poison) {
			p.setFillColor(color_t::Red);
			p.setSize({ length - 20.f, length - 20.f });
			int px = irand(1, N.x);
			int py = irand(1, N.y);
			Maze.setPosition(p, px, py);
		}
		};

	auto Create_Resource = [&](size_t amount) {
		if (amount > MAX_POISON) amount = MAX_POISON;
		resource.resize(amount);
		auto N = Maze.getSize();
		for (auto& p : resource) {
			p.setFillColor(color_t::Green);
			p.setSize({ length - 20.f, length - 15.f });
			int px = irand(1, N.x);
			int py = irand(1, N.y);
			Maze.setPosition(p, px, py);
		}
		};
	
	ivec2				p;
	ivec2				oldp;
	int					dir{ -1 };

	// MAIN LOOP ///////////////////////////////////////////////////
	while (window.isOpen()) {
		/// STAR GAME PARAMETER ///
		if (Bool["Restart"]()) {
			Maze.set_dimension(Nx, Ny);
			Bool["Restart"]() = false;
		}


		util::get_maximum(max_fps, fps);
		util::get_minimum(min_fps, fps);
		fps = 1.f / elapsed_time;
		elapsed_time = fpsClock.getElapsedTime().asSeconds();
		fpsClock.restart();

		util::update_every(updateClock, 3.f, [&fps5sec, fps]() {fps5sec = fps; });

		// Make manual delay in loop
		util::waiting<sf::Clock>(delay);

		sf::Event	event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed) window.close();

			imouse = sf::Mouse::getPosition(window);
			fmouse = util::as<float>(imouse);

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Up) dir = 0;
				if (event.key.code == sf::Keyboard::Right) dir = 1;
				if (event.key.code == sf::Keyboard::Down) dir = 2;
				if (event.key.code == sf::Keyboard::Left) dir = 3;
			}

			if (event.type = sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
				}
			}

		} // Event Loop

		ImGui::SFML::Update(window, iclock.restart());


		ImGui::SetNextWindowSize(winSize);
		ImGui::SetNextWindowPos(winPos);

		ImGui::Begin("Maze Option");

		if (ImGui::Button("Close")) window.close();

		if (ImGui::Button("Reset")) {
			Maze.restart();
		}

		if (ImGui::Button("Resize")) {
			Bool["Resize"]() = true;
		}

		if (ImGui::Button("Open")) {
			Bool["Open"]() = true;
		}

		if (ImGui::Button("Save")) {
			Bool["Save"]() = true;
		}

		if (ImGui::Button("Export Image")) {
			Maze.save_as_image(window,maze_name);
		}

		if (ImGui::Button("Redo")) {
			Maze.reset();
			Bool["run"]() = true;
		}

		if (ImGui::Button("Show")) Maze.hide_show_box();

		ImGui::SliderFloat("Delay", &delay, 0.f, 0.1f);

		ImGui::Checkbox("Run", &Bool["run"]());

		if (ImGui::Checkbox("Play", &Bool["play"]())) {
			delay = 0.07f;
			Bool["run"]() = false;
			Maze.hide_show_box();
		}
		ImGui::Separator();
		if (ImGui::Button("Create Poison")) {
			Create_Poison(4);
		}
		if (ImGui::Button("Create Resource")) {
			Create_Resource(8);
		}
		ImGui::Separator();
		ImGui::Text("Life Time [%d]", life_time);
		
		ImGui::End();

		// Window for new Nx, Ny:--------------------------------------
		if (Bool["Resize"]()) {
			Bool["run"]() = false;
			Bool["skip"]() = false;
			ImGui::Begin("Chose Nx, Ny", (bool*)0, ImGuiWindowFlags_NoCollapse);
			ImGui::SliderInt("Nx :", &Nx, 2, Nx_Max);
			ImGui::SliderInt("Ny :", &Ny, 2, Ny_Max);
			if (ImGui::Button("Accept")) {
				Bool["Restart"]() = true;
				Bool["Resize"]() = false;
			}
			ImGui::End();
		}

		// Windows Save -------------------------------------------------
		if (Bool["Save"]()) {
			ImGui::Begin("Save File");
			char buf[256] = "";
			std::string _name = maze_name;

			ImGui::InputText("Enter File Name", buf, sizeof(buf));

			if (ImGui::Button("Save")) {
				if (std::strlen(buf) > 0) {
					_name = std::string(buf);
				}

				_name +=  "_" + util::make_serial_daytime();

				Maze.save_to_file(_name);
				
				Bool["Save"]() = false;
			}

			if (ImGui::Button("Quit")) {
				Bool["Save"]() = false;
			}

			ImGui::End();
			
		}

		// Window Open ------------------------------------------------------
		if (Bool["Open"]()) {
			std::string filename;
			Bool["run"]() = false;
			Bool["skip"]() = false;

			// open file dialogue box
			auto file = opendialog::OpenFile(L"Maze file .maze");
			if (!file.empty()) {
				filename = std::string(file.begin(), file.end());
				Maze.open_maze(filename);
			}

			Bool["Open"]() = false;
		}

		// drawig the maze algorithm -------------------------------
		if (!Bool["play"]()) {
			Maze.run();
			Maze.move();
		}

		// playing the game  ----------------------------------------
		//
		//		Using parameter for direction : dir
		//		
		// 
		// ----------------------------------------------------------

		if (Bool["play"]()) {
			oldp = p;

			dir = Maze.possible_direction(dir, p);

			switch (dir) {
			case 0: --p.y;break;
			case 1: ++p.x;break;
			case 2: ++p.y;break;
			case 3: --p.x;break;
			}

			//dir = -1;

			Maze.setPosition(player, p.x, p.y);

			// eat resource
			for (auto it = resource.begin(); it != resource.end();) {
				auto prs = Maze.getPosition(*it);
				if (prs.x == p.x && prs.y == p.y) {
					++life_time;
					it = resource.erase(it);
				}
				else ++it;
			}

			// died if eat poison
			for (auto it = poison.begin(); it != poison.end(); ) {
				auto pp = Maze.getPosition(*it);
				if (pp.x == p.x && pp.y == p.y) {
					--life_time;
					it = poison.erase(it);
				}
				else ++it;
			}
		}

		window.clear();

		// draw grid
		window.draw(Maze);
		if (Bool["play"]()) {
			window.draw(player);
			for (auto p : poison) window.draw(p);
			for (auto r : resource) window.draw(r);
		}

		ImGui::SFML::Render(window);
		window.display();
	}


	ImGui::SFML::Shutdown();


	return 0;
}