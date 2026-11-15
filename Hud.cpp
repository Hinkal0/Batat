#include "Hud.h"
#include "GameManager.h"
#include "Text.h"
#include "Window.h"
#include <cstdio>
#include <string>

static constexpr float MARGIN = 8.0f;
static constexpr float ROW_STEP = 12.0f;
static constexpr float HEALTH_Y = MARGIN;
static constexpr float XP_Y = MARGIN + ROW_STEP;
static constexpr float SLASH_Y = MARGIN + 2.0f * ROW_STEP;
static constexpr float BAR_X = 44.0f;
static constexpr sf::Vector2f BAR_SIZE(64.0f, 5.0f);
static constexpr float RIGHT_EDGE = Window::UI_SIZE.x - MARGIN;

static const sf::Color PANEL_COLOR(10, 8, 16, 150);
static const sf::Color LABEL_COLOR(170, 170, 190);
static const sf::Color VALUE_COLOR(255, 255, 255);

static const sf::Color HEALTH_COLOR(224, 62, 62);
static const sf::Color XP_COLOR(84, 200, 232);
static const sf::Color SLASH_READY_COLOR(245, 240, 220);
static const sf::Color SLASH_CHARGING_COLOR(120, 118, 140);

Hud::Hud(const Player& player, const RunStats& stats)
  : player(player), stats(stats),
    healthBar({BAR_X, HEALTH_Y + 1.0f}, BAR_SIZE, 0.0f, HEALTH_COLOR),
    xpBar({BAR_X, XP_Y + 1.0f}, BAR_SIZE, 0.0f, XP_COLOR),
    slashBar({BAR_X, SLASH_Y + 1.0f}, BAR_SIZE, 0.0f, SLASH_CHARGING_COLOR),
    floorLabel({RIGHT_EDGE, HEALTH_Y}, "FLOOR", "", LABEL_COLOR, VALUE_COLOR),
    timeLabel({RIGHT_EDGE, XP_Y}, "TIME", "", LABEL_COLOR, VALUE_COLOR) {
  update(0.0f);
}

static void drawPanel(sf::RenderTarget& target, sf::Vector2f position, sf::Vector2f size,
    sf::RenderStates states) {
  sf::RectangleShape panel(size);
  panel.setPosition(position);
  panel.setFillColor(PANEL_COLOR);
  target.draw(panel, states);
}

static std::string formatTime(float ms) {
  int total = (int)(ms / 1000.0f);
  int hours = total / 3600, minutes = total / 60 % 60, seconds = total % 60;
  char buffer[128];
  std::snprintf(buffer, sizeof(buffer), "%d:%02d:%02d", hours, minutes, seconds);
  return buffer;
}

void Hud::update(float) {
  healthBar.fill = static_cast<float>(player.getHealth()) / player.getMaxHealth();
  xpBar.fill = stats.xpToNextLevel != 0 ? static_cast<float>(stats.xp) / stats.xpToNextLevel : 0.0f;
  slashBar.fill = player.getSlashReadiness();
  slashBar.color = slashBar.fill >= 1.0f ? SLASH_READY_COLOR : SLASH_CHARGING_COLOR;
  floorLabel.value = std::to_string(stats.floor);
  timeLabel.value = formatTime(stats.time);
}

void Hud::drawContents(sf::RenderTarget& target, sf::RenderStates states) const {
  float panelHeight = ROW_STEP + Text::GLYPH_HEIGHT + 8.0f;
  float barValueX = BAR_X + BAR_SIZE.x + 5.0f;

  drawPanel(target, {MARGIN - 4.0f, MARGIN - 4.0f},
    {BAR_X + BAR_SIZE.x + 36.0f - MARGIN, panelHeight}, states);

  Text::draw(target, "HP", {MARGIN, HEALTH_Y}, 1, LABEL_COLOR, Text::Align::Left, true, states);
  target.draw(healthBar, states);
  Text::draw(target, std::to_string(player.getHealth()) + "/" + std::to_string(player.getMaxHealth()),
    {barValueX, HEALTH_Y}, 1, VALUE_COLOR, Text::Align::Left, true, states);

  Text::draw(target, "LV" + std::to_string(stats.level), {MARGIN, XP_Y}, 1,
    LABEL_COLOR, Text::Align::Left, true, states);
  target.draw(xpBar, states);
  Text::draw(target, std::to_string(stats.xp) + "/" + std::to_string(stats.xpToNextLevel),
    {barValueX, XP_Y}, 1, VALUE_COLOR, Text::Align::Left, true, states);

  Text::draw(target, "SLASH", {MARGIN, SLASH_Y}, 1, slashBar.fill >= 1.0f ? VALUE_COLOR : LABEL_COLOR,
    Text::Align::Left, true, states);
  target.draw(slashBar, states);

  float panelWidth = 72.0f;
  drawPanel(target, {RIGHT_EDGE - panelWidth + 4.0f, MARGIN - 4.0f},
    {panelWidth, panelHeight}, states);

  target.draw(floorLabel, states);
  target.draw(timeLabel, states);
}
