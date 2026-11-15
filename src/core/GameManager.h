#pragma once
#include "config/Config.h"
#include "Window.h"
#include "entities/Player.h"
#include "ui/Interface.h"

struct RunStats {
  int floor = 1;
  int level = 1;
  int xp = 0;
  int xpToNextLevel = 10;
  float time = 0.0f;
};

class GameManager {
  GameConfig config;
  Window window;

  Scene gameScene;
  Scene menuScene;

  std::shared_ptr<Player> player;
  RunStats runStats;

  std::shared_ptr<Interface> ui;

  bool inRun() const { return player != nullptr; }
  Scene& activeScene() { return inRun() ? gameScene : menuScene; }

  float menuTime = 0.0f;

  void startRun();
  void spawnEnemies();
  void pause();
  void resume();
  void endRun();

  void update(float dt);

public:
  GameManager();
  GameManager(const GameManager&) = delete;
  GameManager operator=(const GameManager&) = delete;

  void run();
};
