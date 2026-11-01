#include "Hud.h"
#include "Text.h"
#include "Window.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

static const float MARGIN = 8.0f;
static const float ROW_STEP = 12.0f;
static const float BAR_X = 44.0f;
static const float BAR_WIDTH = 64.0f;
static const float BAR_HEIGHT = 5.0f;

static const sf::Color PANEL_COLOR(10, 8, 16, 150);
static const sf::Color OUTLINE_COLOR(8, 6, 12);
static const sf::Color LABEL_COLOR(170, 170, 190);
static const sf::Color VALUE_COLOR(255, 255, 255);

static const sf::Color HEALTH_COLOR(224, 62, 62);
static const sf::Color XP_COLOR(84, 200, 232);
static const sf::Color BAR_EMPTY_COLOR(34, 34, 48);
static const sf::Color SLASH_READY_COLOR(245, 240, 220);
static const sf::Color SLASH_CHARGING_COLOR(120, 118, 140);

static void rect(sf::RenderTarget& target, sf::Vector2f position, sf::Vector2f size,
    sf::Color color, bool outline = false) {
  sf::RectangleShape shape(size);
  shape.setPosition(position);
  shape.setFillColor(color);
  if (outline) {
    shape.setOutlineThickness(1.0f);
    shape.setOutlineColor(OUTLINE_COLOR);
  }
  target.draw(shape);
}

static void bar(sf::RenderTarget& target, sf::Vector2f position, float fill,
    sf::Color color) {
  fill = std::clamp(fill, 0.0f, 1.0f);
  rect(target, position, {BAR_WIDTH, BAR_HEIGHT}, BAR_EMPTY_COLOR, true);
  float width = std::round(BAR_WIDTH * fill);
  if (width > 0.0f) {
    rect(target, position, {width, BAR_HEIGHT}, color);
    rect(target, position, {width, 1.0f}, color + sf::Color(40, 40, 40));
  }
}

static std::string formatTime(float ms) {
  int total = (int)(ms / 1000.0f);
  int hours = total / 3600, minutes = total / 60 % 60, seconds = total % 60;
  char buffer[128];
  std::snprintf(buffer, sizeof(buffer), "%d:%02d:%02d", hours, minutes, seconds);
  return buffer;
}

static void labeledValue(sf::RenderTarget& target, float y,
    const std::string& label, const std::string& value) {
  float right = Window::UI_SIZE.x - MARGIN;
  Text::draw(target, value, {right, y}, 1, VALUE_COLOR, Text::Align::Right);
  Text::draw(target, label, {right - Text::width(value) - 5.0f, y}, 1,
    LABEL_COLOR, Text::Align::Right);
}

void Hud::draw(sf::RenderTarget& target, sf::RenderStates) const {
  float y0 = MARGIN, y1 = MARGIN + ROW_STEP, y2 = MARGIN + 2 * ROW_STEP;
  float panelHeight = 2 * ROW_STEP + Text::GLYPH_HEIGHT + 8.0f;

  rect(target, {MARGIN - 4.0f, MARGIN - 4.0f},
    {BAR_X + BAR_WIDTH + 36.0f - MARGIN, panelHeight}, PANEL_COLOR);

  Text::draw(target, "HP", {MARGIN, y0}, 1, LABEL_COLOR);
  float healthFill = (float)player.getHealth() / player.getMaxHealth();
  bar(target, {BAR_X, y0 + 1.0f}, healthFill, HEALTH_COLOR);
  Text::draw(target, std::to_string(player.getHealth()) + "/" + std::to_string(player.getMaxHealth()),
    {BAR_X + BAR_WIDTH + 5.0f, y0}, 1, VALUE_COLOR);

  Text::draw(target, "LV" + std::to_string(stats.level), {MARGIN, y1}, 1, LABEL_COLOR);
  float xpFill = stats.xpToNextLevel != 0 ? (float)stats.xp / stats.xpToNextLevel : 0.0f;
  bar(target, {BAR_X, y1 + 1.0f}, xpFill, XP_COLOR);
  Text::draw(target, std::to_string(stats.xp) + "/" + std::to_string(stats.xpToNextLevel),
    {BAR_X + BAR_WIDTH + 5.0f, y1}, 1, VALUE_COLOR);

  float slash = player.getSlashReadiness();
  bool ready = slash >= 1.0f;
  Text::draw(target, "SLASH", {MARGIN, y2}, 1, ready ? VALUE_COLOR : LABEL_COLOR);
  bar(target, {BAR_X, y2 + 1.0f}, slash, ready ? SLASH_READY_COLOR : SLASH_CHARGING_COLOR);

  float panelWidth = 116.0f;
  rect(target, {Window::UI_SIZE.x - MARGIN - panelWidth + 4.0f, MARGIN - 4.0f},
    {panelWidth, panelHeight}, PANEL_COLOR);

  labeledValue(target, y0, "SCORE", std::to_string(stats.score));
  labeledValue(target, y1, "FLOOR", std::to_string(stats.floor));
  labeledValue(target, y2, "TIME", formatTime(stats.time));
}
