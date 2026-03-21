#pragma once

#include <cstdint>
#include <variant>

// 字节码文件常量池中值的类型的定义
enum class ValueType : std::uint8_t { Boolean, Number, Function };

// 格式: [tag:8][reserved:7][value:1]
using BooleanValue = bool;

// 格式: [tag:8][value:64]
using NumberValue = double;

// 格式：[tag:8][index:16]
struct FunctionValue {
  std::uint16_t index;  // 函数在函数表（不论是文件中的还是vm中的）中的下标
};

using Value = std::variant<BooleanValue, NumberValue, FunctionValue>;
