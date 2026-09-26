#include "GameManager.h"

GameManager::GameManager() {
  WinConfig wconf = {"Batat", 800, 600, 600, 600, 0};
  window = Window(wconf);
}

void GameManager::run() {
  while (window.isOpen()) {
    window.pollEvents();

    window.beginDraw();
    window.endDraw();
  }
}
