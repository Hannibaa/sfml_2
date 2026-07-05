#include <iostream>
#include "../include/utility.hpp"


template<typename T>
class IsValueChanged {
	T	old_value;

public:
	IsValueChanged(const T& value) :old_value(value) {}

	bool operator()(T value) {
		if (value == old_value) return false;
		else {
			old_value = value;
			return true;
		}
	}
};



int main() {

	std::cout << "Begin Program\m";

	float v = util::get_random(0, 10);

	IsValueChanged<int>   is_value_changed(v);

	int		vi{};

	for (int i = 0; i < 10; ++i) {
		v = util::get_random_float(12.f, 10.f);
		if (is_value_changed(v)) {
			std::cout << " v is changed "  << v << std::endl;
		}
		else
			std::cout << " v isnot changed " << std::endl;

	}







	return 0;
}