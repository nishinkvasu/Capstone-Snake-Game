#include "game.h"
#include <iostream>
#include <fstream>

#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
	  wall(grid_width, grid_height),
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
  GameState gState;
  int frame_count = 0;
  bool running = true;
  int pauseRequest = 0;
  bool restartRequest = false;
  bool bUpdateScore = true; // to track the scoresheet to be updated or not

  std::thread timerThread(&Game::timerThreadFunction, this, std::ref(running));
  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake, pauseRequest, restartRequest); // Feat1
	if (pauseRequest != 1) {
		gState = Update(); // feat 2 - tracking the status of the game
		if(gState == GameState::kActive)
			renderer.Render(snake, food, wall); // add obstacle here
		else
		{
			// dump the scores here
			// one time dump only per game
			if (bUpdateScore) {
				updateScoreHistory();
				bUpdateScore = false;
			}
			// if restart request is set, then reset the game
			if (restartRequest) {
				restart(); // reset the game parameters

				// locking the update of the boolean variable 
				// which controls the condition variable which 
				// in turn controls the lifetime of the slow mode
				// thread
				std::unique_lock<std::mutex> ulock(_mutex);
				slowSpeed = true;
				_condvar.notify_one();
				slowSpeed = false;
				ulock.unlock();

				bUpdateScore = true; // updateScore flag also set to true to record for new game
				restartRequest = false; // reset the flag to monitor the reset request status
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
      renderer.UpdateWindowTitle(score, frame_count, snake.speed, pauseRequest, (gState != GameState::kActive)); // Feat1
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
  // ensuring the slow speed thread comes out of its wait state 
  // and terminates
  slowSpeed = true;
  _condvar.notify_one();
  timerThread.join();
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake / wall object before placing
    // food.
    if (!snake.ObjCell(x, y)) {
		// added check to ensure that food is not where there is a wall or obstacle 
		if (!wall.ObjCell(x, y)) { 
			food.x = x;
			food.y = y;
			return;
		}
    }
  }
}

GameState Game::Update() {
  if (!snake.alive) return GameState::kDead;

  snake.Update(wall);

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
	// lock here
	std::lock_guard<std::mutex> lck(_mutex);
	score++;
    snake.speed += 0.02;
	if (score == 10) {
		_condvar.notify_one();
		slowSpeed = true;
	}
	// unlock
  }

  return GameState::kActive;
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }

/*  Game::restart
	Added a small state machine to allow continuous play of the game.
	The restart method is used to reset the variables which contribute to the overall 
	state of the game
*/
void Game::restart() {
	// reset snake attributes
	snake.restart();
	// make the snake alive
	snake.alive = true;
	// reset the speed and score
	snake.speed = 0.1f;
	score = 0;

}
/*
Game::updateScoreHistory will record in a test file all the score history.
This can be further developed to show the last scores on the screen.
Right now it just records the data on a file
*/
void Game::updateScoreHistory() {
	// add score to scoresheet file
	std::ofstream scoreFile;
	scoreFile.open("Scoresheet.txt", std::ios::app | std::ios::out);
	scoreFile << " Round " << round++ << " Score  " << GetScore() << "\n";
	scoreFile.close();
}

/*
	Game::timerThreadFunction is a function that will run in a separate thread from the main 
	it is used to provide a feature where the snake will slow down for 10 seconds 
	after the score crosses 10.

	The slowSpeed variable is used as the predicate for the condition variable which is notified
	when the score equals 10. The unique lock is used here to make use of in a condition variable
	and also to allow to unlock for a period when the thread goes to sleep (for 10 seconds).
	The lock is then again locked in -order to restore the speed to close to the original value

*/

void Game::timerThreadFunction(bool &running) {
	std::cout << "Inside Timer thread function \n ";
	while(running){ 
		std::unique_lock<std::mutex> uLock(_mutex);
		_condvar.wait(uLock, [this] {return slowSpeed; });
		std::cout << "Inside Timer thread function - condvar notified\n ";

		if (score == 10) {
			// Idea here is to start a timer when the score passes 10 (signalled through a condition variable)
			// and before the timer starts we will update the snake speed in a mutex
			std::cout << "Slow speed - start\n ";
			snake.speed -= 0.15f;
			// and go to sleep for some time
			uLock.unlock();
			std::this_thread::sleep_for(std::chrono::seconds(10));
			// on waking up we will restore the speed
			std::cout << "Slow speed - end\n ";
			uLock.lock();
			snake.speed += 0.1f; // should increase speed only if old game still continuing
			slowSpeed = false;
		}
		
	}
		
}