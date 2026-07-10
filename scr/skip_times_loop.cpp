/*
			OBJECTIVE OF THIS FUNCTION TO MAKE FUNCTION THAT SKIP LOOP INSIDE LOOP ABOUT 
			NUMBER OF TIMES
*/

#include <iostream>
#include "../../mylib/include/console/win_utility.h"





int main() {
	std::cout << "Start of Program" << std::endl;

	bool is_true = true;

	int number_loops = 100;

	int fps{};
	int fps100{};
	int i{};
	// Main Loop
	while (is_true) {
		++fps;
		++i;

		// manual escap of loop
		if (get_out()) break;


		if (i > number_loops) {
			++fps100;
			i = 0;
		}



		std::cout << "FPS : " << fps << "   FPS100 " << fps100 << "\r";
	}





	return 0;
}