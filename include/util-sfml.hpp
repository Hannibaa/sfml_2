/*
		UTILITY FOR SFML LIBRARY
*/
#pragma once

#include "utility.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics.hpp>


using fvec2 = sf::Vector2f;
using ivec2 = sf::Vector2i;
using VSprites = std::vector<sf::Sprite>;
using Box_t = sf::RectangleShape;
using Circle_t = sf::CircleShape;
using color_t = sf::Color;


namespace util {

	/// CONVERT COORDINAT TYPE
	template<typename T, typename U>
	sf::Vector2<T> as(const sf::Vector2<U>& v) {
		sf::Vector2<T> w;
		w.x = static_cast<T>(v.x);
		w.y = static_cast<T>(v.y);
		return w;
	}

	struct Quadra {

	private:
		sf::Vector2f    a[4];

	public:
		Quadra() = default;

		Quadra(const sf::FloatRect& rect) {
			get_quadra(rect);
		}

		void get_quadra(const sf::FloatRect& rect) {
			a[0] = sf::Vector2f(rect.left, rect.top);
			a[1] = sf::Vector2f(rect.left + rect.width, rect.top);
			a[2] = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);
			a[3] = sf::Vector2f(rect.left, rect.top + rect.height);
		}

		auto operator[](unsigned int index) {
			if (index >= 0 && index < 4)
				return a[index];
		}

		auto operator[](unsigned int index) const {
			if (index >= 0 && index < 4)
				return a[index];
		}
	};


	bool inf2d(const fvec2& v, const fvec2& w, float epsilon) {
		return inf(v.x, w.x, epsilon) && inf(v.y, w.y, epsilon);
	}


	bool is_vector_close(const sf::Vector2f& a, const sf::Vector2f& b, float epsilon)
	{
		if (inf(a.x, b.x, epsilon) && inf(a.y, b.y, epsilon)) {
			return true;
		}

		return false;
	}


	sf::Vector2i get_random_vector(int xmin, int xmax, int ymin, int ymax) {
		iRG<int>	irand;
		return { irand(xmin,xmax), irand(ymin,ymax) };
	}

	sf::Vector2f get_random_vector(float xmin, float xmax, float ymin, float ymax) {
		fRG<float>	frand;
		return { frand(xmin,xmax), frand(ymin,ymax) };
	}

	sf::IntRect	get_random_recti(int min, int max) {

		return {};
	}

	float get_angle(const fvec2& v1, const fvec2& v2)
	{
		const float dot = v1.x * v2.x + v1.y * v2.y;
		const float det = v1.x * v2.y - v1.y * v2.x;
		return atan2(det, dot);
	}

	float length(const fvec2& v) {
		return sqrt(v.x * v.x + v.y * v.y);
	}

	float length2(const fvec2 v) {
		return (v.x * v.x + v.y * v.y);
	}

	bool save_window_interior(sf::RenderWindow& window, const sf::IntRect& rect,
		const std::string& filename)
	{
		// Capture the whole window
		sf::Texture	tex;
		tex.create(window.getSize().x, window.getSize().y);
		tex.update(window);

		// Convert to image
		sf::Image	fullimage = tex.copyToImage();

		// Create the cropped image
		sf::Image	cropped;
		cropped.create(rect.width, rect.height);
		
		// Copy the selected rectangle
		cropped.copy(fullimage, 0, 0, rect, false);

		return cropped.saveToFile(filename);
	}

} // END OF UTIL NAMESPACE