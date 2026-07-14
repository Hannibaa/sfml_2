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
					  0	  1    2
						+----+
					  7	|    | 3     This cell(x,y) int : first bit for visited
						+----+		 four next bit for wall open or not
					  6	  5	   4

					  first bit from right indicate visited or no(0 not, 1 visited)
					  second bit from right indicate wall 1 (1 wall exist, 0 not)
					  third bit from right indicate wall 3
					  fourth bit from right indicate wall 5
					  fifth bit from right indicate wall 7

					  to retrive wall from cell(x,y) using function
					  WALL(cell(x,y)) = 0b1101
*/


#include "imgui.h"
#include "imgui-SFML.h"

#include <iostream>
#include <stack>
#include <SFML/Graphics.hpp>
#include "../include/util-sfml.hpp"
#include "../include/box_s.hpp"

#define VISITOR(x)		(x & 0b1)
#define WALL1(x)		((x >> 1) & 1b)
#define WALL2(x)		((x >> 2) & 1b)
#define WALL3(x)		((x >> 3) & 1b)
#define WALL4(x)		((x >> 4) & 1b)
#define WALL(x,n)		((x >> n) & 1b)
#define SETVISITOR(x)   

#define SETWALLS(x)     

using Grid_t = std::vector<util::Box_s>;

void make_grid(
	Grid_t& grid,
	const fvec2& pos,
	color_t wall_color,
	f32	ulength,
	f32 wall_thickness,
	u32	nx,
	u32 ny
)
{
	for (u32 i = 0; i < nx; ++i)
		for (u32 j = 0; j < ny; ++j)
		{
			grid.push_back(util::Box_s(pos + fvec2(f32(i), f32(j)) * ulength, ulength, wall_thickness));
		}
}


void reset_grid_color(Grid_t& grid) {
	for (auto& b : grid) {
		b.setFillColor(sf::Color::Transparent);
		b.set_wall(util::Box_s::WALL::CLEAR);
	}
}



int main() {


	const i32			WinWidth = 1200;
	const i32			WinHeight = 800;

	const i32			MAX_X{ 30 };
	const i32			MAX_Y{ 25 };

	const f32			FontScale = 1.5f;

	color_t				transparent = color_t::Transparent;
	color_t				red = color_t::Red;
	color_t				blue = color_t::Blue;
	color_t				black = color_t::Black;
	int								Nx{ 15 }, Ny{ 20 };

	//std::cout << "Enter number of column : ";
	//std::cin >> Nx;
	//std::cout << "Enter number of lines : ";
	//std::cin >> Ny;

	util::clamp(Nx, 5, MAX_X);
	util::clamp(Ny, 10, MAX_Y);

	util::iRG<int>		irand;

	sf::RenderWindow	window(sf::VideoMode(WinWidth, WinHeight), "Cell Automata");
	ImGui::SFML::Init(window);
	ImGui::GetIO().FontGlobalScale = FontScale;

	// Bool action
	util::mapBool		Bool;
	Bool["ShowOption"] = { true, "Show option menu" };
	Bool["Run"] = { true, "Run the strategy" };
	Bool["Skip"] = { true, "skip the loop" };
	Bool["Back"] = { false,"Backspace or to get possible case" };
	Bool["ShowBox"] = { true, "Show a box index" };

	// box that make maze
	Box_t							box;
	// grid 
	Grid_t							grid;
	// maze stat cell
	std::vector<int>				maze;
	// stack of path visited cell
	std::stack<iCoord>				prev;
	int prevx{}, prevy{};

	int								nVisited{ 1 };
	color_t							color(sf::Color::White);
	int								nx{}, ny{};
	int								oldnx{}, oldny{};


	float							ulength = 30.f;
	float							thickness = 2.f;
	float							ilength = ulength - thickness;
	fvec2							gridPos(10.f, 10.f);

	box.setSize(fvec2(ulength - thickness, ulength - thickness));
	box.setFillColor(red);


	make_grid(grid, gridPos, color, ulength, thickness, Nx, Ny);
	grid[0].setFillColor(blue);
	const int WALLS_UNVISITED = 30;
	const int WALLS_VISITED = 31;
	maze.resize(Nx * Ny,WALLS_UNVISITED);
	maze[0] = WALLS_VISITED;
	prev.push({ 0,0 });

	// check boundary of coordinate grid
	auto in_bound = [=](int x, int y) {
		return x >= 0 && x < Nx && y >= 0 && y < Ny;
		};

	// Coordinate grid to linear coordinate of maze
	auto NZ = [=](int x, int y) {
		if (in_bound(x, y)) return y + x * Ny;
		};

	// get safe maze stat
	auto MAZE = [&](int x, int y) {
		if (in_bound(x, y)) return maze[NZ(x, y)];
		else return 1;
		};

	// this return 4 all close, < 4 there one open path
	auto visited_neibor = [&](int x, int y) {
		return (MAZE(x - 1, y) & 0b1) + (MAZE(x + 1, y) & 0b1) +
			(MAZE(x, y - 1) & 0b1) + (MAZE(x, y + 1) & 0b1);
		};

	int visitedNeibor{};

	// get wall number of two adjacent cell -1 for no matching
	auto wall_adjacent = [](int x, int y, int oldx, int oldy) {
		// there are four possible case 
		if (x == oldx && y == oldy + 1) {
			return 1;
		}
		if (x == oldx && y == oldy - 1) {
			return 5;
		}
		if (x == oldx + 1 && y == oldy) {
			return 7;
		}
		if (x == oldx - 1 && y == oldy) {
			return 3;
		}

		return -1;
		};

	// remove wall of two cell adjacent
	auto remove_adjacent_wall = [&](int x, int y, int oldx, int oldy) {
		int wall = wall_adjacent(x, y, oldx, oldy);
		switch (wall) {
		case -1: return;
		case 1: // other = 5
			grid[NZ(x, y)].unset_wall(1);
			grid[NZ(oldx, oldy)].unset_wall(5);
			maze[NZ(x, y)] |= 0b10;
			maze[NZ(oldx, oldy)] |= 0b1000;
			break;
		case 5:
			grid[NZ(x, y)].unset_wall(5);
			grid[NZ(oldx, oldy)].unset_wall(1);
			maze[NZ(x, y)] |= 0b1000;
			maze[NZ(oldx, oldy)] |= 0b10;
			break;
		case 7:
			grid[NZ(x, y)].unset_wall(7);
			grid[NZ(oldx, oldy)].unset_wall(3);
			maze[NZ(x, y)] |= 0b10000;
			maze[NZ(oldx, oldy)] |= 0b100;
			break;
		case 3:
			grid[NZ(x, y)].unset_wall(3);
			grid[NZ(oldx, oldy)].unset_wall(7);
			maze[NZ(x, y)] |= 0b100;
			maze[NZ(oldx, oldy)] |= 0b10000;
			break;
		}
		};


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


	// MAIN LOOP
	while (window.isOpen()) {
		util::get_maximum(max_fps, fps);
		util::get_minimum(min_fps, fps);
		fps = 1.f / elapsed_time;
		elapsed_time = fpsClock.getElapsedTime().asSeconds();
		fpsClock.restart();

		util::update_every(updateClock, 3.f, [&fps5sec, fps]() {fps5sec = fps; });

		// Make manual delay in loop
		util::waiting<sf::Clock>(delay);
		oldnx = nx;
		oldny = ny;

		sf::Event	event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed) window.close();

			imouse = sf::Mouse::getPosition(window);
			fmouse = util::as<float>(imouse);

			if (event.type == sf::Event::KeyPressed) {

				if (event.key.code == sf::Keyboard::A && event.key.control) {
					Bool["ShowOption"].not_();
				}


			}

			if (event.type = sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
				}
			}

		} // Event Loop


		// Random runnig of maze
		if (Bool["Run"]()) {
			// calculate visit neibors
			visitedNeibor = visited_neibor(nx, ny);

			// checking 
			if (visitedNeibor == 4) {
				prev.pop();
				if (!prev.empty()) {
					nx = prev.top().x;
					ny = prev.top().y;
				}
				else {
					prev.push({ 0,0 });
				}
				Bool["Skip"]() = true;
			}
			else {
				int walk = irand(0, 3);

				switch (walk) {
				case 0:++nx; break;
				case 1:--nx; break;
				case 2:++ny; break;
				case 3:--ny; break;
				}

				Bool["Skip"]() = true;

				util::clamp(nx, 0, Nx - 1);
				util::clamp(ny, 0, Ny - 1);
			}
		}


		ImGui::SFML::Update(window, iclock.restart());

		// ALL IMGUI Menu here Begin
		if (!prev.empty()) {
			prevx = prev.top().x;
			prevy = prev.top().y;
		}

		ImGui::Begin("Option", &Bool["ShowOption"]());
		if (ImGui::Button("Close")) window.close();
		ImGui::Text("FPS[ %.2f ] ELAPSED[%2.4f]", fps, elapsed_time);
		ImGui::Text("FPS[ %.2f ] ", fps5sec);
		//ImGui::Text("Grid Number[%d][%d]", gnum.x, gnum.y);
		ImGui::Text("Max[%.2f]   Min[%.2f]", max_fps, min_fps);
		ImGui::Text("Visited %d", nVisited);
		ImGui::Text("[%d][%d] stat[%d]", nx, ny, maze[ny + Ny * nx]);
		ImGui::Text("[%d]stack[%d][%d] stat[%d]", prev.size(), prevx, prevy, maze[prevy + Ny * prevx]);
		ImGui::Text("Visited Neibors [%d]", visitedNeibor);
		if (ImGui::Button("RESETMAZE")) {
			util::reset(maze, WALLS_UNVISITED);
			maze[0] = WALLS_VISITED;
			reset_grid_color(grid);
			util::clear(prev);
			prev.push({ 0,0 });
			nVisited = 0;
			nx = 0;
			ny = 0;
			oldnx = 0;
			oldny = 0;
			Bool["Skip"]() = true;
			grid[0].setFillColor(blue);
		}

		ImGui::Checkbox("Run", &Bool["Run"]());
		ImGui::Checkbox("Showbox", &Bool["ShowBox"]());

		ImGui::SliderFloat("Delay", &delay, 0.f, 0.1f);

		ImGui::End();


		// ALL IMGUI Menu here End



		// Check if box is visited only when you performed a movement 
		if (Bool["Skip"]()) {
			if (nVisited < Nx * Ny) {
				// Create a matrix of visited cells
				if ((maze[ny + Ny * nx] & 0b1) == 0) {
					maze[ny + Ny * nx] |= 1;
					// memorized top previous visited cell to checked after
					int oldx = prev.top().x;
					int oldy = prev.top().y;
					// push new visited cell
					prev.push({ nx,ny });
					++nVisited;

					//  remove wall between adjacent cells
					remove_adjacent_wall(nx, ny, oldx, oldy);
				}
				else {
					nx = prev.top().x;
					ny = prev.top().y;
				}

				// paint visited cell
				if ((maze[ny + Ny * nx] & 0b1) == 1) {
					grid[ny + Ny * nx].setFillColor(black);
				}
			}

			Bool["Skip"]() = false;
		}


		// set position of box red
		if (in_bound(nx, ny)) {
			box.setPosition(gridPos + fvec2(thickness, thickness) + fvec2(nx, ny) * ulength);
		}



		window.clear();




		for (auto& b : grid) {
			window.draw(b);
		}

		if (Bool["ShowBox"]())
			window.draw(box);

		ImGui::SFML::Render(window);
		window.display();
	}


	ImGui::SFML::Shutdown();






	return 0;
}