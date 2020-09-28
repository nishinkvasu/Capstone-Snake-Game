#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include "SDL.h"

class Object {
 public:

  Object(int grid_width, int grid_height)
      : grid_width(grid_width),
        grid_height(grid_height) {
  }

  virtual bool ObjCell(int x, int y) = 0; // common
  
  std::vector<SDL_Point> body; // common

 protected:
  int grid_width; // common
  int grid_height; // common
};

#endif