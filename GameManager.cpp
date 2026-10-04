#include "GameManager.h"
#include "Parser.h"
#include <chrono>
#include <random>

static float deltaTime = 0.0f;
static float renderDelta = 0.0f;
const static float FRAMERATE = 120.0f;

static sf::Texture texture;

GameManager::GameManager() {
  WinConfig wconf = {};
  try {
    Parser parser("config.ini");
    if (!parser.extractConfig(wconf)) wconf = {};
  } catch(std::runtime_error& e) {}

  window = Window(wconf);
  
  uint32_t seed = time(nullptr);
  srand(seed);

  gameScene.map = std::make_shared<Map>(sf::Vector2u{100, 70}, 64.0f);
  gameScene.map->generate();

  gameScene.layers.push_back({});

  sf::Image image({100, 100}, sf::Color::White);
  texture = sf::Texture(image);
  player = std::make_shared<Player>(texture, gameScene.map);
  player->setPosition(gameScene.map->getSpawnPoint());
  gameScene.layers[0].insert(player);
}

void GameManager::run() {
  auto start = std::chrono::high_resolution_clock::now();
  while (window.isOpen()) {
    window.pollEvents();

    auto dt = (std::chrono::high_resolution_clock::now()-start);
    deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(dt).count()/1000.0f;
    renderDelta += deltaTime;
    start = std::chrono::high_resolution_clock::now();
    
    if (!window.isPaused()) {
      gameScene.update(deltaTime);
      
      gameScene.cameraPos = (
        window.getMousePos()+player->getPosition().componentWiseMul({2,2}))
        .componentWiseDiv({3,3});
    }

    if (1000.0f/renderDelta <= FRAMERATE) {
      window.draw(gameScene);
      renderDelta = 0.0f;
    }
  }
}
