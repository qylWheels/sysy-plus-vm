#pragma once

#include <cstddef>
#include <variant>

using NumberValue = double;

using PointerValue = std::size_t;

struct ObjectValue {};

struct ArrayValue {};

struct TableValue {};

using Value = std::variant<NumberValue, PointerValue, ObjectValue, ArrayValue,
                           TableValue>;
