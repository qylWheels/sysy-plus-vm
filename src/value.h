#pragma once

#include <cstdint>
#include <variant>

// 字节码文件中值的类型的定义
enum class ValueType : std::uint8_t { Number };

// 格式：[tag:8][value:64]
using NumberValue = double;

// 格式：字节码文件中的常量池里不会有这个玩意。只有在运行时才有它
struct FunctionValue {
  std::uint16_t index;  // 函数在函数表中的下标
};

using Value = std::variant<NumberValue, FunctionValue>;
