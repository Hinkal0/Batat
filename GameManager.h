#pragma once
#include "Window.h"

class GameManager {
  Window window;

public:
  GameManager();
  GameManager(const GameManager&) = delete;
  GameManager operator=(const GameManager&) = delete;

  void run();
};