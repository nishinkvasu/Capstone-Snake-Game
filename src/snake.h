#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include "object.h"
#include "wall.h"
#include "SDL.h"

class Snake : public Object {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  Snake(int grid_width, int grid_height)
      : Object(grid_width, grid_height),
        head_x(grid_width / 2),
        head_y(grid_height / 2) {}

  void Update(Wall &wall);
  void restart(); // Used to reset the snake attributes for a restart
  void GrowBody();

  bool ObjCell(int x, int y);

  Direction direction = Direction::kUp;

  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;

 private:
  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell, Wall &wall);
  bool growing{false};

};

#endif