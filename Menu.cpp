#include "Menu.h"
#include <utility>
#include "Text.h"

static const float BUTTON_WIDTH = 120.0f;
static const float OPTION_WIDTH = 230.0f;
static const float ROW_HEIGHT = 13.0f;
static const float ROW_STEP = 17.0f;
static const float VALUE_WIDTH = 96.0f;
static const float TITLE_OFFSET = 34.0f;

static const sf::Color ROW_COLOR(16, 14, 24, 190);
static const sf::Color ROW_SELECTED_COLOR(58, 44, 24, 230);
static const sf::Color BORDER_COLOR(60, 56, 78);
static const sf::Color BORDER_SELECTED_COLOR(240, 196, 90);
static const sf::Color TEXT_COLOR(200, 200, 212);
static const sf::Color TEXT_SELECTED_COLOR(255, 222, 120);
static const sf::Color TEXT_DISABLED_COLOR(96, 94, 110);

Menu::Menu(sf::Vector2f position, std::string title)
  : UIElement(position), title(std::move(title)) {}

void Menu::addButton(std::string label, std::function<void()> press) {
  items.push_back({std::move(label), std::move(press), nullptr, nullptr, nullptr, nullptr});
}

void Menu::addOption(std::string label, std::function<std::string()> value,
    std::function<void(int)> change, std::function<bool()> enabled,
    std::function<bool(int)> canChange) {
  items.push_back({std::move(label), nullptr, std::move(value), std::move(change),
    std::move(enabled), std::move(canChange)});
}

void Menu::onBack(std::function<void()> callback) {
  back = std::move(callback);
}

void Menu::reset() {
  selected = 0;
}

sf::FloatRect Menu::itemRect(int index) const {
  float width = items[index].value ? OPTION_WIDTH : BUTTON_WIDTH;
  return {{-width / 2.0f, index * ROW_STEP}, {width, ROW_HEIGHT}};
}

int Menu::itemAt(sf::Vector2f point) const {
  for (int i = 0; i < (int)items.size(); i++) {
    if (itemRect(i).contains(point)) return i;
  }
  return -1;
}

void Menu::select(int index, int direction) {
  int count = (int)items.size();
  for (int step = 0; step < count; step++) {
    int i = ((index + step * direction) % count + count) % count;
    if (items[i].isEnabled()) {
      selected = i;
      return;
    }
  }
}

void Menu::onKeyPressed(sf::Keyboard::Key key) {
  if (!visible || items.empty()) return;

  if (!items[selected].isEnabled()) select(selected, 1);

  Item& item = items[selected];
  switch (key) {
  case sf::Keyboard::Key::Up:
  case sf::Keyboard::Key::W:
    select(selected - 1, -1);
    break;
  case sf::Keyboard::Key::Down:
  case sf::Keyboard::Key::S:
    select(selected + 1, 1);
    break;
  case sf::Keyboard::Key::Left:
  case sf::Keyboard::Key::A:
    item.step(-1);
    break;
  case sf::Keyboard::Key::Right:
  case sf::Keyboard::Key::D:
    item.step(1);
    break;
  case sf::Keyboard::Key::Enter:
  case sf::Keyboard::Key::Space:
    if (item.press) item.press();
    else item.step(1);
    break;
  case sf::Keyboard::Key::Escape:
    if (back) back();
    break;
  default:
    break;
  }
}

void Menu::onMouseMoved(sf::Vector2f mousePos) {
  if (!visible) return;
  int index = itemAt(mousePos - position);
  if (index >= 0 && items[index].isEnabled()) selected = index;
}

void Menu::onMousePressed(sf::Mouse::Button button, sf::Vector2f mousePos) {
  if (!visible || button != sf::Mouse::Button::Left) return;

  sf::Vector2f localMouse = mousePos - position;
  int index = itemAt(localMouse);
  if (index < 0 || !items[index].isEnabled()) return;
  selected = index;

  Item& item = items[index];
  if (item.press) {
    item.press();
  } else if (item.change) {
    sf::FloatRect rect = itemRect(index);
    float valueStart = rect.position.x + rect.size.x - VALUE_WIDTH;
    bool left = localMouse.x >= valueStart && localMouse.x < valueStart + VALUE_WIDTH / 2.0f;
    item.step(left ? -1 : 1);
  }
}

void Menu::drawContents(sf::RenderTarget& target, sf::RenderStates states) const {
  if (!title.empty()) {
    Text::draw(target, title, {0.0f, -TITLE_OFFSET}, 2,
      sf::Color::White, Text::Align::Center, true, states);
  }

  for (int i = 0; i < (int)items.size(); i++) {
    const Item& item = items[i];
    bool enabled = item.isEnabled();
    bool active = i == selected && enabled;
    sf::FloatRect rect = itemRect(i);

    sf::RectangleShape row(rect.size);
    row.setPosition(rect.position);
    row.setFillColor(active ? ROW_SELECTED_COLOR : ROW_COLOR);
    row.setOutlineThickness(1.0f);
    row.setOutlineColor(active ? BORDER_SELECTED_COLOR : BORDER_COLOR);
    target.draw(row, states);

    sf::Color color = !enabled ? TEXT_DISABLED_COLOR : active ? TEXT_SELECTED_COLOR : TEXT_COLOR;
    float textY = rect.position.y + 3.0f;

    if (!item.value) {
      Text::draw(target, item.label, {0.0f, textY}, 1, color, Text::Align::Center, true, states);
      continue;
    }

    float right = rect.position.x + rect.size.x;
    float valueStart = right - VALUE_WIDTH;
    Text::draw(target, item.label, {rect.position.x + 6.0f, textY}, 1,
      color, Text::Align::Left, true, states);
    Text::draw(target, "<", {valueStart + 4.0f, textY}, 1,
      enabled && !item.canStep(-1) ? TEXT_DISABLED_COLOR : color, Text::Align::Left, true, states);
    Text::draw(target, item.value(), {valueStart + VALUE_WIDTH / 2.0f, textY}, 1,
      color, Text::Align::Center, true, states);
    Text::draw(target, ">", {right - 8.0f, textY}, 1,
      enabled && !item.canStep(1) ? TEXT_DISABLED_COLOR : color, Text::Align::Left, true, states);
  }
}
