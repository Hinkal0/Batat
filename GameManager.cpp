#include "GameManager.h"
#include "Parser.h"

GameManager::GameManager() {
  WinConfig wconf = {};
  try {
    Parser parser("config.ini");
    if (!parser.extractConfig(wconf)) wconf = {};
  } catch(std::runtime_error& e) {}

  window = Window(wconf);
}

void GameManager::run() {
  while (window.isOpen()) {
    window.pollEvents();

    window.beginDraw();
    window.endDraw();
  }
}
