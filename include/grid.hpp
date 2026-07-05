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

        Grid(const sf::FloatRect& rect, int lines, int column)
            :_rect(rect)
            , _grid_lines(lines)
            , _grid_colum(column)
            , _color{ sf::Color::White }
        {}

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