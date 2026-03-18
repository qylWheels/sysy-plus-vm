#pragma once

#include <variant>

enum class OpCode {

};

using NumberLiteralOperand = double;

using IndexOperand = std::size_t;

using Operand = std::variant<NumberLiteralOperand, IndexOperand>;

struct Instruction {
  OpCode opcode;
  Operand Operand;
};
