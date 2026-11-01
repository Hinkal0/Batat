#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>

class Menu : public sf::Drawable {
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

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
  std::string title;
  float top = 110.0f;

  Menu(std::string title = "");

  void addButton(std::string label, std::function<void()> press);
  void addOption(std::string label, std::function<std::string()> value,
    std::function<void(int)> change, std::function<bool()> enabled = nullptr,
    std::function<bool(int)> canChange = nullptr);
  void onBack(std::function<void()> callback);

  void reset();

  void handleEvent(const sf::Event& event, sf::Vector2f uiMouse);
};
