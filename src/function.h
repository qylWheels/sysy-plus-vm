#pragma once

#include <cstddef>
#include <string>
#include <vector>

// 格式：
// [function name length:8][function name:var]
// [param count:8]
// [local variable count:16]
// [instruction count:32][instructions:var]
struct Function {
  std::string name;
  std::uint8_t param_count;
  std::uint16_t local_var_count;
  std::vector<std::uint32_t> instructions;
};
