#pragma once

#include <set>
#include "util-sfml.hpp"








namespace util {

	class Box_s : public sf::Drawable {
	public:
		enum WALL {
			CLEAR = -1,
			TOP = 1,
			RIT = 3,
			BOT = 5,
			LFT = 7,
		};

		enum CORNER {
			TOP_L,
			TOP_R = 2,
			BOT_R = 4,
			BOT_L = 6,
		};

	private:
		fvec2			_position;
		f32				_thickness;
		f32				_length;
		color_t			_color_wall;
		color_t			_color_box;
		std::set<int>	_dont_draw;

	public:

		Box_s(const fvec2& position, f32 length, f32 thickness)
			:_position(position)
			, _thickness(thickness)
			, _length(length)
			, _color_wall(sf::Color::White)
			, _color_box(sf::Color::Transparent)
		{
		}

		void set_wall(int number_side) {
			if (number_side == WALL::CLEAR) _dont_draw.clear();

			if (number_side >= 0 && number_side < 8)
			{
				if (_dont_draw.contains(number_side))
					_dont_draw.erase(number_side);
				else
					_dont_draw.insert(number_side);
			}
		}

		fvec2 getPosition() const {
			return _position;
		}

		void setPosition(const fvec2& position) {
			_position = position;
		}

		void setWallColor(const color_t& color) {
			_color_wall = color;
		}

		void setFillColor(const color_t color) {
			_color_box = color;
		}

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
			std::vector<sf::RectangleShape>	 side;
			f32		_thick_l = _length - 2.f * _thickness;

			sf::RectangleShape				 box(fvec2(_thick_l, _thick_l));
			box.setFillColor(_color_box);
			box.setPosition(_position + fvec2(_thickness, _thickness));

			side.reserve(8);

			// side 0
			side.emplace_back(fvec2(_thickness, _thickness));
			side.back().setFillColor(_color_wall);
			side.back().setPosition(_position);

			// side 1
			side.emplace_back(fvec2(_thick_l, _thickness));
			side.back().setFillColor(_color_wall);
			side.back().setPosition(_position + fvec2(_thickness, 0.f));

			// side 2
			side.emplace_back(fvec2(_thickness, _thickness));
			side.back().setFillColor(_color_wall);
			side.back().setPosition(_position + fvec2(_thickness + _thick_l, 0.f));

			// side 3
			side.emplace_back(fvec2(_thickness, _thick_l));
			side.back().setFillColor(_color_wall);
			side.back().setPosition(_position + fvec2(_thickness + _thick_l, _thickness));
			// side 4
			side.emplace_back(fvec2(_thickness, _thickness));
			side.back().setFillColor(_color_wall);
			side.back().setPosition(_position + fvec2(_thickness + _thick_l, _thickness + _thick_l));

			// side 5
			side.emplace_back(fvec2(_thick_l, _thickness));
			side.back().setFillColor(_color_wall);
			side.back().setPosition(_position + fvec2(_thickness, _thickness + _thick_l));

			// side 6
			side.emplace_back(fvec2(_thickness, _thickness));
			side.back().setFillColor(_color_wall);
			side.back().setPosition(_position + fvec2(0.f, _thickness + _thick_l));

			// side 7
			side.emplace_back(fvec2(_thickness, _thick_l));
			side.back().setFillColor(_color_wall);
			side.back().setPosition(_position + fvec2(0.f, _thickness));



			// set the walls

			int i{};
			target.draw(box);
			for (auto& s : side) {
				if (!_dont_draw.contains(i))
				target.draw(s, states);
				++i;
			}
		}
	};





} // End UTIL