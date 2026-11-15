#pragma once
#include "Bar.h"
#include "LabeledValue.h"
#include "UIElement.h"
#include "core/GameManager.h"

class Hud : public UIElement {
  const Player& player;
  const RunStats& stats;

  Bar healthBar;
  Bar xpBar;
  Bar slashBar;
  LabeledValue floorLabel;
  LabeledValue timeLabel;

  void drawContents(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
  Hud(const Player& player, const RunStats& stats);
  void update(float deltaTime) override;
};