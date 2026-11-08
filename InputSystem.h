#pragma once 
#include "Window.h"

class InputSystem {
  static inline const Window* window = nullptr;
public:
    
  static void init(const Window& window);
    
  static sf::Vector2f getMousePos(const Scene& scene);
  static sf::Vector2f getUIMousePos(void);
    
  static inline Event<sf::Keyboard::Key> onKeyPressed;
  static inline Event<sf::Keyboard::Key> onKeyReleased;
  static inline Event<sf::Vector2f> onMouseMoved;
  static inline Event<sf::Mouse::Button> onMousePressed;
  static inline Event<sf::Mouse::Button> onMouseReleased;
};