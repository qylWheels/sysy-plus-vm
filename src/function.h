#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct Function {
  std::string name;
  std::vector<std::byte> instructions;
};
