#pragma once

#include <cstddef>
#include <string>
#include <vector>

// 格式：
// [function name length:8][function name:var][param count:8]
// [instruction count:32][instructions:var]
struct Function {
  std::string name;
  std::uint8_t param_count;
  std::vector<std::uint32_t> instructions;
};
