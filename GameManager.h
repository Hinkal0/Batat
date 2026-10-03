#pragma once
#include "Window.h"
#include "Player.h"

class GameManager {
  Window window;

  Scene gameScene;

  std::shared_ptr<Player> player;

public:
  GameManager();
  GameManager(const GameManager&) = delete;
  GameManager operator=(const GameManager&) = delete;

  void run();
};