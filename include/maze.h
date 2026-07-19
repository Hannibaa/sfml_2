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

			TODO :
			1. save maze as image is not working
			2. make player inside maze
			3. make function of position any object inside maze and set the contrain
			physic for that object (size object should be not exced a cell size).
			4. 
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

	void clear(Stat_t& stat) {
		util::reset(stat, WALLS_UNVISITED);
		stat[0] = WALLS_VISITED;
	}

	class Maze : public sf::Drawable {

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
		Box_t					b_Index;

		int						nVisited;
		int						nVisitedNeibor;
		int						nx;
		int						ny;
		int						oldnx;
		int						oldny;

		util::mapBool			iBool;  // internal bool usage
		bool					_b_box_show;
		bool&					_b_run;
		bool&					_b_skip;

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
			iBool["restart"] = { false, "restart game" };
			_b_box_show = true;

			make_grid(_grid, _pos, _wall_color, _color, _ulength, _wthickness, _Nx, _Ny);

			reset_stat();
			b_Index.setSize({ _ilength, _ilength });
			b_Index.setFillColor(color_t::Red);
			util::clear(_prev);
			_prev.push({ 0,0 });
			nVisited = 0;
			nVisitedNeibor = 0;
			nx = 0;
			ny = 0;
			oldnx = 0;
			oldny = 0;
			iBool["validate"]() = true;
		}

		void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override {
			

			for (auto& g : _grid) {
				target.draw(g, states);
			}

			if (_b_box_show)
			target.draw(b_Index, states);
		}

		bool has_maze_extension(const std::string& filename) {
			return std::filesystem::path(filename).extension() == ".maze";
		}

		std::string make_maze_extension(std::string filename) {
			std::filesystem::path  path(filename);

			if (path.extension() != ".maze") path.replace_extension(".maze");

			return path.string();
		}

		std::string finger_print_name(const std::string& filename) {
			std::string text = "__" + std::to_string(_Nx) + "x" + std::to_string(_Ny);
			std::filesystem::path path(filename);
			path.replace_filename(path.stem().string() + text + path.extension().string());

			return path.string();
		}

	public:
		Maze(int Nx, int Ny, f32 length, f32 wall_thickness, bool& brun, bool& bskip):
			_Nx(Nx),
			_Ny(Ny),
			_pos(10.f, 10.f),
			_ulength(length),
			_wthickness(wall_thickness),
			_b_run(brun),
			_b_skip(bskip)
		{
			init();
		}

		ivec2 getSize() const {
			return ivec2(_Nx, _Ny);
		}

		bool save_as_image(sf::RenderWindow& window, const std::string& filename) {
			return util::save_window_interior(window, sf::IntRect(0, 0, _Nx * _ulength, _Ny * _ulength), filename);
		}

		bool save_to_file(std::string filename) {

			filename = make_maze_extension(filename);
			filename = finger_print_name(filename);
			
			std::ofstream	file(filename, std::ios::binary);

			if (!file) {
				std::cerr << "Error: Cannot open file for writing : " << filename << '\n';
				return false;
			}

			// 1. write Nx and Ny and check if it's size of vector.
			file.write(reinterpret_cast<const char*>(&_Nx), sizeof(_Nx));
			file.write(reinterpret_cast<const char*>(&_Ny), sizeof(_Ny));

			// 2. write vector
			file.write(reinterpret_cast<const char*>(_maze.data()), _Nx * _Ny * sizeof(int));

			return true;
		}

		bool open_maze(const std::string& filename) {

			if (!has_maze_extension(filename)) return false;

			std::ifstream		file(filename, std::ios::binary);

			if (!file) {
				std::cerr << "Error : Cannot open file for reading : " << filename << '\n';
				return false;
			}

			// 1. read size of maze _Nx, _Ny, 
			file.read(reinterpret_cast<char*>(&_Nx), sizeof(_Nx));
			file.read(reinterpret_cast<char*>(&_Ny), sizeof(_Ny));

			// 2. read data to vector
			_maze.clear();
			_maze.resize(_Nx * _Ny);

			file.read(reinterpret_cast<char*>(_maze.data()), _Nx * _Ny * sizeof(int));
			draw_maze();

			return true;

		}

		void set_boolean(const std::string& name_bool) {
			if (iBool.contains(name_bool))
			iBool[name_bool]() = true;
		}

		void restart()  {
			if (iBool["restart"]()) {
				init();
				iBool["restart"]() = false;
			}
		}

		void hide_show_box() {
			_b_box_show = !_b_box_show;
		}

		void set_dimension(int Nx, int Ny) {
			_Nx = Nx;
			_Ny = Ny;
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

		void reset() {
			clear(_grid);
			clear(_maze);
			util::clear(_prev);
			_prev.push({ 0,0 });
			nx = 0;
			ny = 0;
			oldnx = 0;
			oldny = 0;
			nVisited = 0;
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

		bool in_bound(const ivec2 p) const {
			return in_bound(p.x, p.y);
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
			if (_b_run) {
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
					_b_skip = true;
				}
				else {
					walk_function(nx, ny);

					_b_skip = true;

					util::clamp(nx, 0, _Nx - 1);
					util::clamp(ny, 0, _Ny - 1);
				}
			}

		}

		void move() {
			if (_b_skip) {
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
					_b_run = false;

				_b_skip = false;
			}

			b_Index.setPosition(_pos + fvec2(_wthickness, _wthickness) + fvec2(nx, ny) * _ulength);
		}

		// object type should be have function getPosition
		template<typename OBJECT>
		void setPosition(OBJECT& b, int x, int y) {
			f32 offset = (_ulength - b.getSize().x) / 2.f;
			b.setPosition(_pos + fvec2(offset, offset) + fvec2(x, y) * _ulength);
		}

		template<typename OBJECT>
		ivec2 getPosition(OBJECT& b) {
			f32 offset = (_ulength - b.getSize().x) / 2.f;
			fvec2 p = b.getPosition();
			return util::as<int>((1.f / f32(_ulength)) * (p - _pos - fvec2(offset, offset)));
		}

		template<typename OBJECT>
		void setPosition(OBJECT& b, const ivec2& p) {
			b.setPosition(b, p.x, p.y);
		}

		bool check(const ivec2& p, const ivec2& oldp) {
			return in_bound(p) && in_bound(oldp);
		}

		int possible_direction(int dir, const ivec2& p) {
			auto s = stat(p.x, p.y);
			switch (dir) {
			case 0: if (WALL1(s) == 0)  return -1;else return dir;
			case 1: if (WALL2(s) == 0)  return -1; else return dir;
			case 2: if (WALL3(s) == 0)  return -1; else return dir;
			case 3: if (WALL4(s) == 0)  return -1; else return dir;
			}

			return -1;
		}
	};


}

