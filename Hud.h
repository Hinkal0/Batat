#pragma once
#include <SFML/Graphics.hpp>
#include "GameManager.h"
#include "UIElement.h"

class Hud : public UIElement {
  const Player& player;
  const RunStats& stats;

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
  Hud(const Player& player, const RunStats& stats) : player(player), stats(stats) {}
};