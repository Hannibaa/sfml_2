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

			FILE FORMAT
			"maze_" + datetimeserialnumber + ".maze" + Nx + "x" + Ny

*/

#pragma once

#define WALL1(x)		((x >> 1) & 0b1)
#define WALL2(x)		((x >> 2) & 0b1)
#define WALL3(x)		((x >> 3) & 0b1)
#define WALL4(x)		((x >> 4) & 0b1)
#define WALL(x,n)		((x >> n) & 0b1)


const int WALLS_UNVISITED = 0;
const int WALLS_VISITED = 1;


using Grid_t = std::vector<util::Box_s>;
using Stat_t = std::vector<int>;



namespace maze {

	static void make_grid(
		Grid_t& grid,
		const fvec2& pos,
		color_t wall_color,
		color_t fill_color,
		f32	ulength,
		f32 wall_thickness,
		u32	Nx,
		u32 Ny
	)
	{
		grid.clear();
		grid.reserve(Nx * Ny);
		for (u32 i = 0; i < Nx; ++i)
			for (u32 j = 0; j < Ny; ++j)
			{
				grid.push_back(util::Box_s(pos + fvec2(f32(i), f32(j)) * ulength, ulength, wall_thickness));
				grid.back().setFillColor(fill_color);
				grid.back().setWallColor(wall_color);
			}
	}

	void clear(Grid_t& grid, color_t color = color_t::Transparent) {
		for (auto& cell : grid) {
			cell.set_wall(-1);
			cell.setFillColor(color);
		}
	}

	class Maze {

		int						_Nx;
		int						_Ny;
		fvec2					_pos;

		f32						_ulength;
		f32						_wthickness;
		f32						_ilength = _ulength - 2.0f * _wthickness;
		color_t					_wall_color{ color_t::White };
		color_t					_color = color_t::Transparent;

		Grid_t					_grid;
		Stat_t					_maze;
		std::stack<iCoord>		_prev;

		int						nVisited;
		int						nVisitedNeibor;
		int						nx;
		int						ny;
		int						oldnx;
		int						oldny;

		util::mapBool			iBool;  // internal bool usage
		util::mapBool&			xBool;	// external bool import or export

		std::function<void(int&, int&)>		_walk_func;       // function for walk function design
		std::function<int(int, int)>		_visited_neibors; // function for visitation critiria

		// neibors criteria.
		enum {
			ONE_NEIBORS = 1,
			TWO_NEIBORS ,
			THREE_NEIBORS,
			MAX_NEIBORS 
		};

		void init() {
			// Set the boolean game 
			iBool["validate"] = { false, "start run game" };
			iBool["run"] = { true, "start run game" };

			make_grid(_grid, _pos, _wall_color, _color, _ulength, _wthickness, _Nx, _Ny);

			reset_stat();

			_prev.push({ 0,0 });
			nVisited = 0;
			nVisitedNeibor = 0;
			nx = 0;
			ny = 0;
			oldnx = 0;
			oldny = 0;
			//Bool["Skip"]() = true;
			//Bool["SetGame"]() = false;
		}

	public:
		Maze(int Nx, int Ny, f32 length, f32 wall_thickness, util::mapBool& ext_Bool):
			_Nx(Nx),
			_Ny(Ny),
			_pos(10.f, 10.f),
			_ulength(length),
			_wthickness(wall_thickness),
			xBool(ext_Bool)
		{
			init();
		}

		void reset_stat() {
			_maze.resize(_Nx * _Ny);
			util::reset(_maze, WALLS_UNVISITED);
			_maze[0] = WALLS_VISITED;
		}

		void remake_grid() {
			make_grid(_grid, _pos, _wall_color, _color, _ulength, _wthickness, _Nx, _Ny);
		}

		void reset_grid() {
			clear(_grid);
		}

		void draw_maze() {
			remake_grid();
			int k{};
			for (auto& cell : _grid) {
				if (WALL1(_maze[k])) cell.unset_wall(1);
				if (WALL2(_maze[k])) cell.unset_wall(3);
				if (WALL3(_maze[k])) cell.unset_wall(5);
				if (WALL4(_maze[k])) cell.unset_wall(7);

				++k;
			}
		}

		bool in_bound(int x, int y) const {
			return x >= 0 && x < _Nx && y >= 0 && y < _Ny;
		}

		int NZ(int x, int y) const {
			if (in_bound(x, y)) return y + x * _Ny;
		}

		int& stat(int x, int y) {
			 return _maze[NZ(x, y)];
		}

		int stat(int x, int y) const {
			return _maze[NZ(x, y)];
		}

		Grid_t::value_type& cell(int x, int y) {
			return _grid[NZ(x, y)];
		}

		int visited_neibors(int x, int y) {
			return (stat(x - 1, y) & 0b1) + (stat(x + 1, y) & 0b1) +
				(stat(x, y - 1) & 0b1) + (stat(x, y + 1) & 0b1);
		}

		int wall_adjacent(int x, int y, int oldx, int oldy) {
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
		}

		void remove_adjacent_wall(int x, int y, int oldx, int oldy) {
			int wall = wall_adjacent(x, y, oldx, oldy);
			switch (wall) {
			case -1: return;
			case 1: // other = 5
				cell(x, y).unset_wall(1);
				cell(oldx, oldy).unset_wall(5);
				stat(x, y) |= 0b10;
				stat(oldx, oldy) |= 0b1000;
				break;
			case 5:
				cell(x, y).unset_wall(5);
				cell(oldx, oldy).unset_wall(1);
				stat(x, y) |= 0b1000;
				stat(oldx, oldy) |= 0b10;
				break;
			case 7:
				cell(x, y).unset_wall(7);
				cell(oldx, oldy).unset_wall(3);
				stat(x, y) |= 0b10000;
				stat(oldx, oldy) |= 0b100;
				break;
			case 3:
				cell(x, y).unset_wall(3);
				cell(oldx, oldy).unset_wall(7);
				stat(x, y) |= 0b100;
				stat(oldx, oldy) |= 0b10000;
				break;
			}

		}

		void walk_function(int& nx, int& ny) {
			util::iRG<int>		irand;
			int walk = irand(0, 3);

			switch (walk) {
			case 0:++nx; break;
			case 1:--nx; break;
			case 2:++ny; break;
			case 3:--ny; break;
			}
		}

		void run() {
			if (iBool["run"]()) {
				// random function
				util::iRG<int>		irand;
				// calculate visit neibors
				int visitedNeibor = visited_neibors(nx, ny);

				// checking 
				if (visitedNeibor == MAX_NEIBORS) {
					_prev.pop();
					if (!_prev.empty()) {
						nx = _prev.top().x;
						ny = _prev.top().y;
					}
					else {
						_prev.push({ 0,0 });
					}
					iBool["validate"]() = true;
				}
				else {
					walk_function(nx, ny);

					iBool["validate"]() = true;

					util::clamp(nx, 0, _Nx - 1);
					util::clamp(ny, 0, _Ny - 1);
				}
			}

		}

		void move() {
			if (iBool["validate"]()) {
				if (nVisited < _Nx * _Ny) {
					// Create a matrix of visited cells
					if ((stat(nx,ny) & 0b1) == 0) {
						stat(nx,ny) |= 1;
						// memorized top previous visited cell to checked after
						int oldx = _prev.top().x;
						int oldy = _prev.top().y;

						// push new visited cell
						_prev.push({ nx,ny });
						++nVisited;

						//  remove wall between adjacent cells
						remove_adjacent_wall(nx, ny, oldx, oldy);
					}
					else {
						nx = _prev.top().x;
						ny = _prev.top().y;
					}

					// paint visited cell
					if ((stat(nx,ny) & 0b1) == 1) {
						cell(nx,ny).setFillColor(color_t::Black);
					}
				}
				else
					iBool["run"]() = false;

				iBool["validate"]() = false;
			}
		}

	};


}