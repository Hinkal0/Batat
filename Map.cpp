#include "Map.h"
#include "Resources.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <queue>
#include <limits>

static const int MIN_LEAF = 12;
static const int MAX_LEAF = 26;
static const int MIN_ROOM = 6;
static const int CORRIDOR_WIDTH = 3;

enum WallShapeBits : uint8_t {
  FLOOR_BELOW       = 1 << 0,
  FLOOR_LEFT        = 1 << 1,
  FLOOR_RIGHT       = 1 << 2,
  FLOOR_BELOW_LEFT  = 1 << 3,
  FLOOR_BELOW_RIGHT = 1 << 4,
};

enum class Tilemap {
  TOP = 0,
  SIDE_LEFT = 1,
  SIDE_RIGHT = 2,
  SIDE_BOTH = 3,
  FRONT = 4,
  CORNER_LEFT = 8,
  CORNER_RIGHT = 9,
  CORNER_BOTH = 10,
  SIDE_LEFT_CORNER_RIGHT = 11,
  SIDE_RIGHT_CORNER_LEFT = 12,
  FLOOR = 13
};

static const int TILEMAP_WIDTH = 4;
static const uint8_t NO_TILE = 0xFF;

static uint8_t getWallTile(uint8_t shape) {
  bool left = shape & FLOOR_LEFT;
  bool right = shape & FLOOR_RIGHT;
  
  uint8_t sides = left * (uint8_t)Tilemap::SIDE_LEFT + right * (uint8_t)Tilemap::SIDE_RIGHT;
  
  if (shape & FLOOR_BELOW) return (uint8_t)Tilemap::FRONT + sides;
  
  if (left && right) return (uint8_t)Tilemap::SIDE_BOTH;
  
  bool leftCorner = shape & FLOOR_BELOW_LEFT;
  bool rightCorner = shape & FLOOR_BELOW_RIGHT;
  if (left) {
    if (rightCorner) return (uint8_t)Tilemap::SIDE_LEFT_CORNER_RIGHT;
    return (uint8_t)Tilemap::SIDE_LEFT;
  }
  if (right) {
    if (leftCorner) return (uint8_t)Tilemap::SIDE_RIGHT_CORNER_LEFT;
    return (uint8_t)Tilemap::SIDE_RIGHT;
  }

  if (leftCorner && rightCorner) return (uint8_t)Tilemap::CORNER_BOTH;
  if (leftCorner) return (uint8_t)Tilemap::CORNER_LEFT;
  if (rightCorner) return (uint8_t)Tilemap::CORNER_RIGHT;
  return (uint8_t)Tilemap::TOP;
}

static int randInt(int lo, int hi) {
  if (hi <= lo) return lo;
  return rand() % (hi-lo+1) + lo;
}

Map::Map(sf::Vector2u size, float tileSize) 
: size(size), tileSize(tileSize) {}

sf::IntRect Map::tileRect(uint8_t tile) const {
  int tileW = (int)Textures::tilemap->getSize().x / TILEMAP_WIDTH;
  return sf::IntRect({tile % TILEMAP_WIDTH * tileW, tile / TILEMAP_WIDTH * tileW}, {tileW, tileW});
}

void Map::generate() {
  tiles.assign(size.x * size.y, Tile::Wall);
  rooms.clear();

  splitAndCarve(sf::IntRect({1, 1}, {(int)size.x - 2, (int)size.y - 2}));

  buildVisuals();
}

sf::Vector2i Map::splitAndCarve(sf::IntRect area) {
  bool canSplitX = area.size.x >= 2 * MIN_LEAF;
  bool canSplitY = area.size.y >= 2 * MIN_LEAF;
  bool smallEnough = area.size.x <= MAX_LEAF && area.size.y <= MAX_LEAF;

  if ((!canSplitX && !canSplitY) || (smallEnough && randInt(0, 3) == 0)) {
    int w = randInt(MIN_ROOM, area.size.x - 2);
    int h = randInt(MIN_ROOM, area.size.y - 2);
    int x = randInt(area.position.x + 1, area.position.x + area.size.x - 1 - w);
    int y = randInt(area.position.y + 1, area.position.y + area.size.y - 1 - h);

    sf::IntRect room({x, y}, {w, h});
    carveRect(room);
    rooms.push_back(room);
    return {x + w / 2, y + h / 2};
  }

  bool splitX;
  if (canSplitX && canSplitY) {
    if (area.size.x > area.size.y * 5 / 4) splitX = true;
    else if (area.size.y > area.size.x * 5 / 4) splitX = false;
    else splitX = randInt(0, 1);
  } else {
    splitX = canSplitX;
  }

  sf::IntRect a = area, b = area;
  if (splitX) {
    int cut = randInt(MIN_LEAF, area.size.x - MIN_LEAF);
    a.size.x = cut;
    b.position.x += cut;
    b.size.x -= cut;
  } else {
    int cut = randInt(MIN_LEAF, area.size.y - MIN_LEAF);
    a.size.y = cut;
    b.position.y += cut;
    b.size.y -= cut;
  }

  sf::Vector2i pa = splitAndCarve(a);
  sf::Vector2i pb = splitAndCarve(b);
  carveCorridor(pa, pb);

  return randInt(0, 1) ? pa : pb;
}

void Map::carveRect(sf::IntRect rect) {
  for (int x = rect.position.x; x < rect.position.x + rect.size.x; x++) {
    for (int y = rect.position.y; y < rect.position.y + rect.size.y; y++) {
      tiles[y * size.x + x] = Tile::Floor;
    }
  }
}

void Map::carveLine(sf::Vector2i from, sf::Vector2i to) {
  int half = CORRIDOR_WIDTH / 2;
  int x0 = std::min(from.x, to.x) - half;
  int y0 = std::min(from.y, to.y) - half;
  int x1 = std::max(from.x, to.x) + half;
  int y1 = std::max(from.y, to.y) + half;
  carveRect(sf::IntRect({x0, y0}, {x1 - x0 + 1, y1 - y0 + 1}));
}

void Map::carveCorridor(sf::Vector2i from, sf::Vector2i to) {
  sf::Vector2i corner = randInt(0, 1) ? sf::Vector2i{to.x, from.y}
                                      : sf::Vector2i{from.x, to.y};
  carveLine(from, corner);
  carveLine(corner, to);
}

Map::Tile Map::getTile(int x, int y) const {
  if (x < 0 || y < 0 || x >= (int)size.x || y >= (int)size.y) return Tile::Wall;
  return tiles[y * size.x + x];
}

bool Map::collides(const sf::FloatRect& box) const {
  const float e = 0.001f;
  int x0 = (int)std::floor((box.position.x + e) / tileSize);
  int y0 = (int)std::floor((box.position.y + e) / tileSize);
  int x1 = (int)std::floor((box.position.x + box.size.x - e) / tileSize);
  int y1 = (int)std::floor((box.position.y + box.size.y - e) / tileSize);

  for (int y = y0; y <= y1; y++) {
    for (int x = x0; x <= x1; x++) {
      if (getTile(x, y) == Tile::Wall) return true;
    }
  }
  return false;
}

sf::Vector2f Map::resolveMovement(sf::FloatRect box, sf::Vector2f movement) const {
  const sf::Vector2f start = box.position;

  box.position.x += movement.x;
  if (collides(box)) {
    if (movement.x > 0.0f) {
      float tile = std::floor((box.position.x + box.size.x) / tileSize);
      box.position.x = tile * tileSize - box.size.x;
    } else {
      float tile = std::floor(box.position.x / tileSize);
      box.position.x = (tile + 1) * tileSize;
    }
  }

  box.position.y += movement.y;
  if (collides(box)) {
    if (movement.y > 0.0f) {
      float tile = std::floor((box.position.y + box.size.y) / tileSize);
      box.position.y = tile * tileSize - box.size.y;
    } else {
      float tile = std::floor(box.position.y / tileSize);
      box.position.y = (tile + 1) * tileSize;
    }
  }

  return box.position - start;
}

sf::Vector2f Map::getSpawnPoint() const {
  const sf::IntRect& room = rooms.front();
  return {
    (room.position.x + room.size.x / 2.0f) * tileSize,
    (room.position.y + room.size.y / 2.0f) * tileSize
  };
}

void Map::buildVisuals() {
  visuals.assign(size.x * size.y, NO_TILE);

  for (int y = 0; y < (int)size.y; y++) {
    for (int x = 0; x < (int)size.x; x++) {
      int index = y * size.x + x;
      if (getTile(x, y) == Tile::Floor) {
        visuals[index] = (uint8_t)Tilemap::FLOOR;
        continue;
      }

      bool touchesFloor = false;
      for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
          if (getTile(x + i, y + j) == Tile::Floor) touchesFloor = true;
      if (!touchesFloor) continue;

      uint8_t shape = 0;
      if (getTile(x, y - 1) == Tile::Floor) shape |= FLOOR_BELOW;
      if (getTile(x - 1, y) == Tile::Floor) shape |= FLOOR_LEFT;
      if (getTile(x + 1, y) == Tile::Floor) shape |= FLOOR_RIGHT;
      if (getTile(x - 1, y - 1) == Tile::Floor) shape |= FLOOR_BELOW_LEFT;
      if (getTile(x + 1, y - 1) == Tile::Floor) shape |= FLOOR_BELOW_RIGHT;

      visuals[index] = getWallTile(shape);
    }
  }
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  if (visuals.empty()) return;

  sf::Sprite sprite(*Textures::tilemap);
  sf::Vector2i rectSize = tileRect(0).size;
  if (rectSize.x == 0 || rectSize.y == 0) return;

  for (int y = 0; y < size.y; y++) {
    for (int x = 0; x < size.x; x++) {
      uint8_t tile = visuals[y * size.x + x];
      if (tile == NO_TILE) continue;
      sprite.setTextureRect(tileRect(tile));

      sf::Vector2f scale = {tileSize / rectSize.x, -tileSize / rectSize.y};
    
      sprite.setScale(scale);
      sprite.setPosition({x * tileSize, (y + 1) * tileSize});

      target.draw(sprite, states);
    }
  }
}

bool Map::hasLineOfSight(sf::Vector2f from, sf::Vector2f to) const {
  auto cell = [this](sf::Vector2f point) {
    return sf::Vector2i((int)std::floor(point.x / tileSize),
                        (int)std::floor(point.y / tileSize));
  };
  sf::Vector2i current = cell(from), goal = cell(to);
  sf::Vector2f offset = to - from;
  
  int stepX = offset.x > 0 ? 1 : -1, stepY = offset.y > 0 ? 1 : -1;
  const float infinity = std::numeric_limits<float>::infinity();
  float deltaX = offset.x != 0 ? tileSize / std::abs(offset.x) : infinity;
  float deltaY = offset.y != 0 ? tileSize / std::abs(offset.y) : infinity;
  float nextX = offset.x != 0 ? ((current.x + (stepX > 0)) * tileSize - from.x) / offset.x : infinity;
  float nextY = offset.y != 0 ? ((current.y + (stepY > 0)) * tileSize - from.y) / offset.y : infinity;
  while (true) {
    if (getTile(current.x, current.y) == Tile::Wall) return false;
    if (current == goal) return true;
    if (nextX == nextY) {
      if (getTile(current.x + stepX, current.y) == Tile::Wall ||
          getTile(current.x, current.y + stepY) == Tile::Wall) return false;

      current += {stepX, stepY};
      nextX += deltaX;
      nextY += deltaY;
    } else if (nextX < nextY) {
      current.x += stepX;
      nextX += deltaX;
    } else {
      current.y += stepY;
      nextY += deltaY;
    }
  }
}

sf::Vector2f Map::nextWaypoint(sf::Vector2f from, sf::Vector2f to, sf::Vector2f bodySize) const {
  auto cell = [this](sf::Vector2f point) {
    return sf::Vector2i((int)std::floor(point.x / tileSize),
                        (int)std::floor(point.y / tileSize));
  };
  auto valid = [this](sf::Vector2i c) {
    return c.x >= 0 && c.y >= 0 && c.x < (int)size.x && c.y < (int)size.y;
  };
  auto center = [this](sf::Vector2i c) {
    return sf::Vector2f((c.x + 0.5f) * tileSize, (c.y + 0.5f) * tileSize);
  };

  sf::Vector2i start = cell(from), goal = cell(to);
  if (!valid(start) || !valid(goal) || getTile(start.x, start.y) == Tile::Wall ||
      getTile(goal.x, goal.y) == Tile::Wall) return from;
  if (start == goal) return to;

  auto index = [this](sf::Vector2i c) { return c.y * (int)size.x + c.x; };
  std::vector<int> previous(size.x * size.y, -1);
  std::queue<sf::Vector2i> bfs;
  bfs.push(start);
  previous[index(start)] = index(start);
  const sf::Vector2i directions[8] = {{1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1}};
  while (!bfs.empty() && previous[index(goal)] == -1) {
    auto current = bfs.front(); bfs.pop();
    for (auto direction : directions) {
      if (direction.x != 0 && direction.y != 0) {
        if (getTile(current.x + direction.x, current.y) == Tile::Wall ||
            getTile(current.x, current.y + direction.y) == Tile::Wall) continue;
      }

      auto next = current + direction;
      if (!valid(next) || previous[index(next)] != -1 ||
          collides(sf::FloatRect(center(next) - bodySize / 2.0f, bodySize))) continue;
      previous[index(next)] = index(current);
      bfs.push(next);
    }
  }

  int current = index(goal);
  if (previous[current] == -1) return from;
  while (previous[current] != index(start)) current = previous[current];
  sf::Vector2f origin = center(start);
  sf::Vector2f next = center({current % (int)size.x, current / (int)size.x});

  if ((next != origin) && (origin-from).length() > 1.0f) return origin;
  return next;
}
