#pragma once
#include <SFML/Graphics.hpp>
#include <cstddef>
#include <cstdint>
#include <optional>
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
  std::vector<std::vector<sf::IntRect>> roomDoors;
  std::optional<std::size_t> lockedRoom;
  std::optional<sf::FloatRect> exitBounds;
  bool exitOpen = false;

  sf::Vector2i splitAndCarve(sf::IntRect area);
  void carveRect(sf::IntRect rect);
  void carveCorridor(sf::Vector2i from, sf::Vector2i to);
  void carveLine(sf::Vector2i from, sf::Vector2i to);

  sf::Vector2f roomCenter(const sf::IntRect& room) const;
  bool isClosedDoor(sf::Vector2i tile) const;
  void buildDoorBounds();
  void buildVisuals();
  void drawClosedDoors(sf::RenderTarget& target, sf::RenderStates states) const;
  void drawExit(sf::RenderTarget& target, sf::RenderStates states) const;
  sf::IntRect tileRect(uint8_t tile) const;
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:

  Map(sf::Vector2u size, float tileSize);

  void generate(bool withExit = false);
  void lockRoom(std::size_t index) { lockedRoom = index; }
  void unlockRoom() { lockedRoom.reset(); }
  std::optional<std::size_t> getRoomContaining(const sf::FloatRect& body) const;

  void unlockExit() { exitOpen = true; }
  bool isExitOpen() const { return exitOpen; }
  bool canUseExit(sf::Vector2f position) const;

  Tile getTile(int x, int y) const;

  bool collides(const sf::FloatRect& box) const;

  sf::Vector2f resolveMovement(sf::FloatRect box, sf::Vector2f movement) const;

  sf::Vector2f getSpawnPoint() const;
  float getTileSize() const { return tileSize; }
  bool hasLineOfSight(sf::Vector2f from, sf::Vector2f to) const;
  sf::Vector2f nextWaypoint(sf::Vector2f from, sf::Vector2f to,
      sf::Vector2f bodySize) const;

  const std::vector<sf::IntRect>& getRooms() const { return rooms; }
};
