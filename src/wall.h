#ifndef WALL_H
#define WALL_H

#include <vector>
#include "object.h"
#include "SDL.h"

class Wall : public Object {
 public:
	 Wall(int grid_width, int grid_height);

	 bool Wall::ObjCell(int x, int y);   
private:

};

#endif