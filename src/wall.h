#ifndef WALL_H
#define WALL_H

#include <vector>
#include "object.h"
#include "SDL.h"

/*
	The Wall object contains the various walls / obstacles that are present in the game.
	The Wall inherits the Object class which is what the Snake inherits from too.
	The common features of these were moved to a base class and thus create a inheritance model in the game.
*/

class Wall : public Object {
 public:
	 Wall(int grid_width, int grid_height);

	 bool ObjCell(int x, int y);   
private:

};

#endif