#include "Parser.h"
#include <string>
#include <sstream>

Parser::Parser(const char* filename) : file(filename) {
  if (!file.is_open()) {
    throw std::runtime_error("Could not open config file!");
  }
}

bool Parser::extractConfig(WinConfig& config) {
  std::string line;
  while (line != "[Window]" && file) {
    std::getline(file, line);
  }

  if (line != "[Window]") return 0;
  bool s = 1;
  for (int i = 0; i < 3 && file; i++) {
    std::getline(file, line);
    std::stringstream ss(line);
    if (line.find("size") != std::string::npos) {
      ss.ignore(line.size(), '=');
      if (!(ss >> config.w)) s = 0;
      ss.ignore(line.size(), ',');
      if (!(ss >> config.h)) s = 0;
    } else if (line.find("title") != std::string::npos) {
      ss.ignore(line.size(), '=');
      if (!(ss >> config.title)) s = 0;
    } else if (line.find("fullscreen") != std::string::npos) {
      ss.ignore(line.size(), '=');
      if (!(ss >> config.fullscreen)) s = 0;
    }
  }

  return s;
}