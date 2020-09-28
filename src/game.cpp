#include "game.h"
#include <iostream>
#include <fstream>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)), // if equal to width then food on borders wont be seen
      random_h(0, static_cast<int>(grid_height - 1)) {
  PlaceFood();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;
  int pauseRequest = 0;
  int restartRequest = 0;
  bool bUpdateScore = true; // to track the scoresheet to be updated or not

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake, pauseRequest, restartRequest); // Feat1
	if (pauseRequest != 1) {
		GameState gState = Update(); // feat 2
		if(gState == GameState::kActive)
			renderer.Render(snake, food);
		else
		{
			// dump the scores here
			// one time dump only
			if (bUpdateScore) {
				updateScoreHistory();
				bUpdateScore = false;
			}
			// if user request is 2, then reset the game
			if (restartRequest) {
				restart();
				bUpdateScore = true;
				restartRequest = 0;
			}
		}

	}
    

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count, pauseRequest); // Feat1
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

GameState Game::Update() {
  if (!snake.alive) return GameState::kDead;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }

  return GameState::kActive;
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }

void Game::restart() {
	// reset snake attributes
	snake.restart();
	// make the snake alive
	snake.alive = true;
	// reset the speed and score
	snake.speed = 0.1f;
	score = 0;

}

void Game::updateScoreHistory() {
	// add score to scoresheet file
	std::ofstream scoreFile;
	scoreFile.open("Scoresheet.txt", std::ios::app | std::ios::out);
	scoreFile << " Round " << round++ << " Score  " << GetScore() << "\n";
	scoreFile.close();
}