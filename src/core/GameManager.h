#pragma once
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <vector>
#include "config/Config.h"
#include "Window.h"
#include "entities/Player.h"
#include "ui/Interface.h"

class Enemy;

struct RunStats {
  static constexpr int FLOOR_COUNT = 10;

  int floor = 1;
  int level = 1;
  int xp = 0;
  int xpToNextLevel = 10;
  std::chrono::steady_clock::duration time{};
  std::size_t enemiesRemaining = 0;
  bool isExitOpen = false;
};

class GameManager {
  GameConfig config;
  Window window;

  Scene gameScene;
  Scene menuScene;

  std::shared_ptr<Player> player;
  RunStats runStats;
  std::vector<std::vector<std::shared_ptr<Enemy>>> roomEnemies;
  std::optional<std::size_t> activeRoom;

  std::shared_ptr<Interface> ui;

  bool inRun() const { return player != nullptr; }
  Scene& activeScene() { return inRun() ? gameScene : menuScene; }

  float menuTime = 0.0f;

  void startRun();
  void loadFloor();
  void finishRun();
  void spawnEnemies();
  void beginEncounter(std::size_t roomIndex);
  void updateEncounter();
  void pause();
  void resume();
  void endRun();

  void update(std::chrono::steady_clock::duration elapsed);

public:
  GameManager();
  GameManager(const GameManager&) = delete;
  GameManager operator=(const GameManager&) = delete;

  void run();
};
