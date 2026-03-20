#pragma once

#include <cstdint>
#include <variant>

enum class OpCode : std::uint8_t {
  // 栈操作
  Push,
  Pop,

  // 存取
  Load,
  Store,

  // 一元运算
  Neg,
  LogicalNot,

  // 二元算术运算
  Add,
  Sub,
  Mul,
  Div,
  Rem,

  // 二元比较运算
  Less,
  Le,
  Eq,
  Ge,
  Greater,
  NotEq,

  // 二元逻辑运算
  LogicalAnd,
  LogicalOr,

  // 无条件跳转
  Jump,

  // 条件分支
  IfLessJump,
  IfLeJump,
  IfEqJump,
  IfGeJump,
  IfGreaterJump,
  IfNotEqJump,

  // 函数调用与返回
  Call,
  Return,
};

using BoolLiteralOperand = bool;

using IndexOperand = std::uint16_t;

using OffsetOperand = std::int16_t;

// 确定指令选择的是常量表还是全局表等等
enum class TableSelectionOperand : std::uint8_t {
  ConstantTable,
  GlobalTable,
  LocalVariableTable,
};

// 栈操作指令
struct PushInstruction {
  // 格式：[opcode:8][reserved:8][index operand:16]
  IndexOperand index;
};
struct PopInstruction {
  // 格式：[opcode:8][reserved:24]
};
using StackOperationInstruction = std::variant<PushInstruction, PopInstruction>;

// 存取指令
struct LoadInstruction {
  // 格式：[opcode:8][reserved:6][table kind:2][index:16]
  TableSelectionOperand table;
  IndexOperand index;
};
struct StoreInstruction {
  // 格式：[opcode:8][reserved:6][table kind:2][index:16]
  TableSelectionOperand table;
  IndexOperand index;
};
using LoadAndStoreInstruction = std::variant<LoadInstruction, StoreInstruction>;

// 一元运算指令
struct NegInstruction {
  // 格式：[opcode:8][reserved:24]
};
struct LogicalNotInstruction {
  // 格式：[opcode:8][reserved:24]
};
using UnaryOperationInstruction =
    std::variant<NegInstruction, LogicalNotInstruction>;

// 二元算数运算指令
struct AddInstruction {
  // 格式：[opcode:8][reserved:24]
};
struct SubInstruction {
  // 格式：[opcode:8][reserved:24]
};
struct MulInstruction {
  // 格式：[opcode:8][reserved:24]
};
struct DivInstruction {
  // 格式：[opcode:8][reserved:24]
};
struct RemInstruction {
  // 格式：[opcode:8][reserved:24]
};

// 二元比较运算指令
struct LessInstruction {
  // 格式：[opcode:8][reserved:24]
};
struct LeInstruction {
  // 格式：[opcode:8][reserved:24]
};
struct EqInstruction {
  // 格式：[opcode:8][reserved:24]
};
struct GeInstruction {
  // 格式：[opcode:8][reserved:24]
};
struct GreaterInstruction {
  // 格式：[opcode:8][reserved:24]
};
struct NotEqInstruction {
  // 格式：[opcode:8][reserved:24]
};

// 二元逻辑运算指令
struct LogicalAndInstruction {
  // 格式：[opcode:8][reserved:24]
};
struct LogicalOrInstruction {
  // 格式：[opcode:8][reserved:24]
};

using BinaryOperationInstruction =
    std::variant<AddInstruction, SubInstruction, MulInstruction, DivInstruction,
                 RemInstruction, LessInstruction, LeInstruction, EqInstruction,
                 GeInstruction, GreaterInstruction, NotEqInstruction,
                 LogicalAndInstruction, LogicalOrInstruction>;

// 无条件分支指令
struct JumpInstruction {
  // 格式：[opcode:8][reserved:8][offset:16]
  OffsetOperand offset;
};

// 条件分支指令
struct IfLessJumpInstruction {
  // 格式：[opcode:8][reserved:8][offset:16]
  OffsetOperand offset;
};
struct IfLeJumpInstruction {
  // 格式：[opcode:8][reserved:8][offset:16]
  OffsetOperand offset;
};
struct IfEqJumpInstruction {
  // 格式：[opcode:8][reserved:8][offset:16]
  OffsetOperand offset;
};
struct IfGeJumpInstruction {
  // 格式：[opcode:8][reserved:8][offset:16]
  OffsetOperand offset;
};
struct IfGreaterJumpInstruction {
  // 格式：[opcode:8][reserved:8][offset:16]
  OffsetOperand offset;
};
struct IfNotEqJumpInstruction {
  // 格式：[opcode:8][reserved:8][offset:16]
  OffsetOperand offset;
};

// 函数调用与返回指令
struct CallInstruction {
  // 格式：[opcode:8][reserved:8][index:16]
  IndexOperand index;
};
struct ReturnInstruction {
  // 格式：[opcode:8][reserved:23][boolean:1]
  BoolLiteralOperand has_something_to_return;
};

using ControlFlowInstruction =
    std::variant<JumpInstruction, IfLessJumpInstruction, IfLeJumpInstruction,
                 IfEqJumpInstruction, IfGeJumpInstruction,
                 IfGreaterJumpInstruction, IfNotEqJumpInstruction,
                 CallInstruction, ReturnInstruction>;

using Instruction =
    std::variant<StackOperationInstruction, LoadAndStoreInstruction,
                 UnaryOperationInstruction, BinaryOperationInstruction,
                 ControlFlowInstruction>;
