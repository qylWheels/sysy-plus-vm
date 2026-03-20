#include "vm.h"

#include <cstdint>
#include <variant>

#include "instruction.h"

void Vm::run() {}

Instruction Vm::decode(const std::uint32_t instr) const {
  const std::uint16_t low_16_mask = 0xffff;
  const std::uint8_t low_2_mask = 0x3;

  const auto opcode = static_cast<OpCode>(instr >> 24);
  switch (opcode) {
    // 栈操作指令
    case OpCode::Push: {
      const auto index = static_cast<IndexOperand>(instr & low_16_mask);
      return PushInstruction{.index = index};
    }
    case OpCode::Pop: {
      return PopInstruction{};
    }

    // 存取指令
    case OpCode::Load: {
      const auto table_kind =
          static_cast<TableSelectionOperand>((instr >> 16) & low_2_mask);
      const auto index = static_cast<IndexOperand>(instr & low_16_mask);
      return LoadInstruction{.table = table_kind, .index = index};
    }
    case OpCode::Store: {
      const auto table_kind =
          static_cast<TableSelectionOperand>((instr >> 16) & low_2_mask);
      const auto index = static_cast<IndexOperand>(instr & low_16_mask);
      return StoreInstruction{.table = table_kind, .index = index};
    }

    // 一元运算指令
    case OpCode::Neg: {
      return NegInstruction{};
    }
    case OpCode::LogicalNot: {
      return LogicalNotInstruction{};
    }

    // 二元算术运算指令
    case OpCode::Add: {
      return AddInstruction{};
    }
    case OpCode::Sub: {
      return SubInstruction{};
    }
    case OpCode::Mul: {
      return MulInstruction{};
    }
    case OpCode::Div: {
      return DivInstruction{};
    }
    case OpCode::Rem: {
      return RemInstruction{};
    }

    // 二元比较运算指令
    case OpCode::Less: {
      return LessInstruction{};
    }
    case OpCode::Le: {
      return LeInstruction{};
    }
    case OpCode::Eq: {
      return EqInstruction{};
    }
    case OpCode::Ge: {
      return GeInstruction{};
    }
    case OpCode::Greater: {
      return GreaterInstruction{};
    }
    case OpCode::NotEq: {
      return NotEqInstruction{};
    }

    // 二元逻辑运算
    case OpCode::LogicalAnd: {
      return LogicalAndInstruction{};
    }
    case OpCode::LogicalOr: {
      return LogicalOrInstruction{};
    }

    // 无条件分支指令
    case OpCode::Jump: {
      const auto offset = static_cast<OffsetOperand>(instr & low_16_mask);
      return JumpInstruction{.offset = offset};
    }

    // 条件分支指令
    case OpCode::IfLessJump: {
      const auto offset = static_cast<OffsetOperand>(instr & low_16_mask);
      return IfLessJumpInstruction{.offset = offset};
    }
    case OpCode::IfLeJump: {
      const auto offset = static_cast<OffsetOperand>(instr & low_16_mask);
      return IfLeJumpInstruction{.offset = offset};
    }
    case OpCode::IfEqJump: {
      const auto offset = static_cast<OffsetOperand>(instr & low_16_mask);
      return IfEqJumpInstruction{.offset = offset};
    }
    case OpCode::IfGeJump: {
      const auto offset = static_cast<OffsetOperand>(instr & low_16_mask);
      return IfGeJumpInstruction{.offset = offset};
    }
    case OpCode::IfGreaterJump: {
      const auto offset = static_cast<OffsetOperand>(instr & low_16_mask);
      return IfGreaterJumpInstruction{.offset = offset};
    }
    case OpCode::IfNotEqJump: {
      const auto offset = static_cast<OffsetOperand>(instr & low_16_mask);
      return IfNotEqJumpInstruction{.offset = offset};
    }

    // 函数调用与返回指令
    case OpCode::Call: {
      const auto index = static_cast<IndexOperand>(instr & low_16_mask);
      return CallInstruction{.index = index};
    }
    case OpCode::Return: {
      const auto has_something_to_return =
          static_cast<BoolLiteralOperand>(instr & 0x1);
      return ReturnInstruction{.has_something_to_return =
                                   has_something_to_return};
    }
  }
}
