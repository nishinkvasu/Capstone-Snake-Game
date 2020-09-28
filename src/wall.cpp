#include "wall.h"
#include <cmath>
#include <iostream>
#include <random>

Wall::Wall(int grid_width, int grid_height) : Object(grid_width, grid_height) {
	SDL_Point point;
	std::random_device dev;
	std::mt19937 engine(dev());
	std::uniform_int_distribution<int> random_w(0, static_cast<int>(grid_width/2 - 1));
	std::uniform_int_distribution<int> random_h(0, static_cast<int>(grid_height/2 - 1));

	// Initialize wall body
	//for (size_t i = 0; i < 3; i++) {
		point.x = random_w(engine);
		point.y = random_h(engine);
		body.push_back(point);

		point.x = random_w(engine);
		point.y = (grid_height / 2)  + random_h(engine);
		body.push_back(point);

		point.x = (grid_width / 2) + random_w(engine);
		point.y = random_h(engine);
		body.push_back(point);

		point.x = (grid_width / 2) + random_w(engine);
		point.y = (grid_height / 2) + random_h(engine);
		body.push_back(point);

	//}
}

bool Wall::ObjCell(int x, int y) {
	for (auto const &item : body) {
		if (x == item.x && y == item.y) {
			return true;
		}
	}
	return false;
}