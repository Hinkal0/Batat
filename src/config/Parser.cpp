#include "Parser.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

const char* DEFAULT_CONFIG = "config/config.default.ini";

static std::string trim(const std::string& s) {
  size_t begin = s.find_first_not_of(" \t\r");
  if (begin == std::string::npos) return "";
  size_t end = s.find_last_not_of(" \t\r");
  return s.substr(begin, end - begin + 1);
}

template <typename T>
static bool parseValue(const std::string& text, T& out) {
  std::stringstream ss(text);
  T value;
  if (!(ss >> value)) return false;
  ss >> std::ws;
  if (!ss.eof()) return false;
  out = value;
  return true;
}

void Parser::read(const char* filename) {
  values.clear();
  std::ifstream file(filename);
  if (!file.is_open()) {
    if (filename == DEFAULT_CONFIG) {
      throw std::runtime_error("Could not open default config file!");
    }
    read(DEFAULT_CONFIG);
    return;
  }

  std::string line, section;
  while (std::getline(file, line)) {
    line = trim(line);
    if (line.empty() || line[0] == '#') continue;

    if (line.front() == '[' && line.back() == ']') {
      section = trim(line.substr(1, line.size() - 2));
      continue;
    }

    size_t eq = line.find('=');
    if (eq == std::string::npos) continue;
    values[section][trim(line.substr(0, eq))] = trim(line.substr(eq + 1));
  }
}

const std::string* Parser::find(const std::string& section, const std::string& key) {
  auto s = values.find(section);
  if (s == values.end()) return nullptr;
  auto k = s->second.find(key);
  if (k == s->second.end()) return nullptr;
  return &k->second;
}

bool Parser::extractConfig(GameConfig& config) {
  bool f = true;

  if (const std::string* size = find("Window", "size")) {
    size_t comma = size->find(',');
    f = comma != std::string::npos &&
        parseValue(size->substr(0, comma), config.window.w) && parseValue(size->substr(comma + 1), config.window.h) &&
        config.window.w > 0 && config.window.h > 0;
  }

  if (const std::string* fullscreen = find("Window", "fullscreen")) {
    f = parseValue(*fullscreen, config.window.fullscreen);
  }

  if (const std::string* title = find("Window", "title")) {
    f = !title->empty();
    if (f) config.window.title = *title;
  }

  if (const std::string* volume = find("Audio", "volume")) {
    f = parseValue(*volume, config.volume);
    config.volume = std::clamp(config.volume, 0, 100);
  }

  config.bestTime.reset();
  if (const std::string* bestTime = find("Run", "best_time_ms")) {
    std::chrono::milliseconds::rep milliseconds;
    if (parseValue(*bestTime, milliseconds) && milliseconds >= 0) {
      config.bestTime = std::chrono::milliseconds(milliseconds);
    } else {
      f = false;
    }
  }

  return f;
}

bool Parser::save(const char* filename, const GameConfig& config) {
  std::ofstream file(filename);
  if (!file.is_open()) return false;

  file << "[Window]\n"
       << "  size=" << config.window.w << "," << config.window.h << "\n"
       << "  fullscreen=" << (config.window.fullscreen ? 1 : 0) << "\n"
       << "  title=" << config.window.title << "\n"
       << "[Audio]\n"
       << "  volume=" << config.volume << "\n";
  if (config.bestTime) {
    file << "[Run]\n"
         << "  best_time_ms=" << config.bestTime->count() << "\n";
  }

  return true;
}