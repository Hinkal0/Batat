#pragma once
#include <unordered_map>
#include <string>
#include "Config.h"

class Parser {
  static inline std::unordered_map<std::string, std::unordered_map<std::string, std::string>> values;

  static const std::string* find(const std::string& section, const std::string& key);

public:
  static void read(const char* filename);

  static bool extractConfig(GameConfig& config);

  static bool save(const char* filename, const GameConfig& config);
};