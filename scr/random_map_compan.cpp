#include <iostream>
#include <numeric>
#include "../include/utility.hpp"


std::vector<int>  make_random_map(int n) {
	util::iRG<int>			irand;

	std::vector<int>		order(n);
	std::iota(order.begin(), order.end(), 0);

	std::vector<int>		random;
	random.reserve(n);

	for (int i = 0; i < n; ++i)
	{
		int k = irand(0, n - 1 - i);
		random.push_back( order[k]);
		order.erase(order.begin() + k);
	}

	return random;
}







int main() {

	std::cout << "Begine Programs " << std::endl;

	auto rand_map = make_random_map(4);

	for (int i = 0; i < rand_map.size(); ++i)
	{
		std::cout << "[ " << i << " ] = " << rand_map[i] << std::endl;
	}

	std::cout << "-------------------- End --------------------- " << std::endl;
	std::cin.get();

	return 0;
}