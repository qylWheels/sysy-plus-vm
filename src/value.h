#pragma once

#include <cstdint>
#include <variant>

using NumberValue = double;

struct FunctionValue {
  std::uint16_t index;  // 函数在函数表中的下标
};

using Value = std::variant<NumberValue, FunctionValue>;
