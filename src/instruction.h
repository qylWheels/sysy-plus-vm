#pragma once

#include <cstdint>
#include <variant>

using NumberLiteralOperand = double;

using BoolLiteralOperand = bool;

using IndexOperand = std::size_t;

using OffsetOperand = std::int64_t;

// 确定指令选择的是常量表还是全局表等等
enum class TableSelectionOperand {
  ConstantTable,
  GlobalTable,
};

// 栈操作指令
struct PushInstruction {
  NumberLiteralOperand val;
};
struct PopInstruction;
using StackOperationInstruction = std::variant<PushInstruction, PopInstruction>;

// 存取指令
struct LoadInstruction {
  TableSelectionOperand table;
  IndexOperand index;
};
struct StoreInstruction {
  TableSelectionOperand table;
  IndexOperand index;
};
using LoadAndStoreInstruction = std::variant<LoadInstruction, StoreInstruction>;

// 一元运算指令
struct NegInstruction;
struct LogicalNotInstruction;
using UnaryOperationInstruction =
    std::variant<NegInstruction, LogicalNotInstruction>;

// 二元算数运算指令
struct AddInstruction;
struct SubInstruction;
struct MulInstruction;
struct DivInstruction;
struct RemInstruction;

// 二元比较运算指令
struct LessInstruction;
struct LeInstruction;
struct EqInstruction;
struct GeInstruction;
struct GreaterInstruction;
struct NotEqInstruction;

// 二元逻辑运算指令
struct LogicalAndInstruction;
struct LogicalOrInstruction;

using BinaryOperationInstruction =
    std::variant<AddInstruction, SubInstruction, MulInstruction, DivInstruction,
                 RemInstruction, LessInstruction, LeInstruction, EqInstruction,
                 GeInstruction, GreaterInstruction, NotEqInstruction,
                 LogicalAndInstruction, LogicalOrInstruction>;

// 无条件分支指令
struct JumpInstruction {
  OffsetOperand offset;
};

// 条件分支指令
struct IfLessJumpInstruction {
  OffsetOperand offset;
};
struct IfLeJumpInstruction {
  OffsetOperand offset;
};
struct IfEqJumpInstruction {
  OffsetOperand offset;
};
struct IfGeJumpInstruction {
  OffsetOperand offset;
};
struct IfGreaterJumpInstruction {
  OffsetOperand offset;
};
struct IfNotEqJumpInstruction {
  OffsetOperand offset;
};

// 函数调用与返回指令
struct CallInstruction {
  IndexOperand index;
};
struct ReturnInstruction {
  bool has_something_to_return;
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
