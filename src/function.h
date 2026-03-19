#pragma once

#include <cstddef>
#include <string>
#include <vector>

// 格式：[function name length:16][function name:var][instruction
// count:32][instructions:var]
struct Function {
  std::string name;
  std::vector<std::uint32_t> instructions;
};
