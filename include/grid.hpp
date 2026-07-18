/*
		GRID OBJECT 
*/

#pragma once

#include "util-sfml.hpp"







namespace util {

    class Grid : public sf::Drawable {

        sf::FloatRect   _rect;
        int             _grid_lines;
        int             _grid_colum;
        sf::Color       _color;

    public:
        Grid(int nlx, int nly, float ulength)
            :_rect(0.f, 0.f, nlx * ulength, nly * ulength)
            ,_grid_lines(nlx)
            ,_grid_colum(nly)
            ,_color(sf::Color::White)
        { }

        Grid(const sf::FloatRect& rect, int lines, int column)
            :_rect(rect)
            , _grid_lines(lines)
            , _grid_colum(column)
            , _color{ sf::Color::White }
        {}

        Grid(const sf::FloatRect& rect, float unit_length)
            :_rect(rect),
            _grid_lines(rect.width / unit_length),
            _grid_colum(rect.height / unit_length),
            _color(sf::Color::White)
        { }

        bool contains(const ivec2& ip) {
            return ip.x >= 0 && ip.x < _grid_lines &&
                ip.y >= 0 && ip.y < _grid_colum;
        }

        void set_color(const sf::Color& color) {
            _color = color;
        }

        void setPosition(float x, float y) {
            _rect.left = x;
            _rect.top = y;
        }

        void setPosition(const fvec2& pos) {
            setPosition(pos.x, pos.y);
        }

        fvec2 getPosition() const {
            return fvec2(_rect.left, _rect.top);
        }

        std::vector<ivec2> getGridPosition(const fvec2& fmouse) const {
            if (!_rect.contains(fmouse)) return {};

            std::vector<ivec2>  vec(1);
            const float     w = _rect.width / (_grid_lines);
            const float     h = _rect.height / (_grid_colum);

            // calculate number of grids
            float xm = fmouse.x - _rect.left;
            float ym = fmouse.y - _rect.top;
            int nx = (int)xm / w;
            int ny = (int)ym / h;

            vec[0].x = nx;
            vec[0].y = ny;
            return vec;
        }

    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {

            sf::VertexArray va(sf::Lines, 2 * _grid_lines + 2);
            sf::VertexArray vb(sf::Lines, 2 * _grid_colum + 2);

            const fvec2     a = _rect.getPosition();
            fvec2     b = a + fvec2(0.f, _rect.height);

            const float     w = _rect.width / (_grid_lines);
            const float     h = _rect.height / (_grid_colum);


            for (uint32_t i(0); i < _grid_lines + 1; ++i)
            {
                va[2 * i].position = a + fvec2(float(i) * w, 0.f);
                va[2 * i].color = _color;

                va[2 * i + 1].position = b + fvec2(float(i) * w, 0.f);
                va[2 * i + 1].color = _color;
            }

            b = a + fvec2(_rect.width, 0.f);

            for (uint32_t i(0); i < _grid_colum + 1; ++i)
            {
                vb[2 * i].position = a + fvec2(0.f, float(i) * h);
                vb[2 * i].color = _color;
                vb[2 * i + 1].position = b + fvec2(0.f, float(i) * h);
                vb[2 * i + 1].color = _color;
            }

            target.draw(va, states);
            target.draw(vb, states);
        }
    };


}