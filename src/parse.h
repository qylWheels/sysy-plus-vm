#pragma once

#include <filesystem>

#include "program.h"

namespace fs = std::filesystem;

class Parser {
 public:
  Program parse(fs::path path);
};
