#pragma once
#include <functional>
#include <string>
#include <vector>
#include "UIElement.h"

class Menu : public UIElement {
  struct Item {
    std::string label;
    std::function<void()> press;
    std::function<std::string()> value;
    std::function<void(int)> change;   
    std::function<bool()> enabled;     
    std::function<bool(int)> canChange;

    bool isEnabled() const { return !enabled || enabled(); }
    bool canStep(int direction) const { return !canChange || canChange(direction); }
    void step(int direction) const { if (change && canStep(direction)) change(direction); }
  };

  std::vector<Item> items;
  int selected = 0;

  std::function<void()> back;

  sf::FloatRect itemRect(int index) const;
  int itemAt(sf::Vector2f point) const;
  void select(int index, int direction);

  void drawContents(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
  std::string title;

  explicit Menu(sf::Vector2f position, std::string title = "");

  void addButton(std::string label, std::function<void()> press);
  void addOption(std::string label, std::function<std::string()> value,
    std::function<void(int)> change, std::function<bool()> enabled = nullptr,
    std::function<bool(int)> canChange = nullptr);
  void onBack(std::function<void()> callback);

  void reset();

  void onKeyPressed(sf::Keyboard::Key key);
  void onMouseMoved(sf::Vector2f mousePosition);
  void onMousePressed(sf::Mouse::Button button, sf::Vector2f mousePosition);
};
