#pragma once
#include <fstream>
#include "Window.h"

class Parser {
  std::ifstream file;
public:
  Parser(const char* filename);
  Parser(const Parser&) = delete;
  Parser operator=(const Parser&) = delete;

  bool extractConfig(WinConfig& config);
};