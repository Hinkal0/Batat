#pragma once
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <vector>

class Map : public sf::Drawable {
public:
  enum class Tile {
    Wall,
    Floor
  };

private:
  sf::Vector2u size;
  float tileSize;

  std::vector<Tile> tiles;
  std::vector<uint8_t> visuals;
  std::vector<sf::IntRect> rooms;

  sf::Vector2i splitAndCarve(sf::IntRect area);
  void carveRect(sf::IntRect rect);
  void carveCorridor(sf::Vector2i from, sf::Vector2i to);
  void carveLine(sf::Vector2i from, sf::Vector2i to);

  void buildVisuals();
  sf::IntRect tileRect(uint8_t tile) const;
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:

  Map(sf::Vector2u size, float tileSize);

  void generate();

  Tile getTile(int x, int y) const;

  bool collides(const sf::FloatRect& box) const;

  sf::Vector2f resolveMovement(sf::FloatRect box, sf::Vector2f movement) const;

  sf::Vector2f getSpawnPoint() const;

  const std::vector<sf::IntRect>& getRooms() const { return rooms; }
};
