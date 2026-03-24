#include "vm.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <ranges>
#include <stack>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "instruction.h"

Vm::Vm()
    : pc_{0},
      callframes_{},
      globals_{},
      constants_{},
      funcs_{},
      args_{},
      retvals_{} {}

void Vm::load_program(const Program prog) {
  this->constants_ = prog.constants;
  this->funcs_ = prog.funcs;
}

void Vm::run() {
  // 找到main函数
  const auto main_func = std::ranges::find_if(
      this->funcs_, [](Function f) { return f.name == "main"; });

  // 创建栈帧
  this->callframes_.push(
      CallFrame{.func = *main_func, .valuestack{}, .local_vars{}});

  // 执行指令
  const auto main_func_len = main_func->instructions.size();
  while (true) {
    // 如果pc指向main的最后一条指令的后一条指令，则退出循环
    if (this->callframes_.top().func.name == "main" &&
        this->pc_ >= main_func_len) {
      break;
    }

    // 获取当前正在执行的函数
    const auto& curr_func = this->callframes_.top().func;

    // 取指
    const auto instr_bytecode = curr_func.instructions[this->pc_];

    // pc自增
    this->pc_ += 1;

    // 译码
    const auto instr = this->decode(instr_bytecode);

    // 执行
    std::visit(
        [this](const auto& instr) {
          using InstrType = std::decay_t<decltype(instr)>;
          if constexpr (std::is_same_v<InstrType, StackOperationInstruction>) {
            std::visit(
                [this](const auto& instr) {
                  using InstrType = std::decay_t<decltype(instr)>;
                  if constexpr (std::is_same_v<InstrType, PushInstruction>) {
                    const auto table = instr.table;
                    const auto index = instr.index;
                    Value v;
                    switch (table) {
                      case TableSelectionOperand::ConstantTable: {
                        v = this->constants_[index];
                        this->callframes_.top().valuestack.push(v);
                        break;
                      }
                      case TableSelectionOperand::GlobalTable: {
                        v = this->globals_[index];
                        this->callframes_.top().valuestack.push(v);
                        break;
                      }
                      case TableSelectionOperand::LocalVariableTable: {
                        v = this->callframes_.top().local_vars[index];
                        this->callframes_.top().valuestack.push(v);
                        break;
                      }
                    }
                  } else if constexpr (std::is_same_v<InstrType,
                                                      PopInstruction>) {
                    this->callframes_.top().valuestack.pop();
                  }
                },
                instr);
          } else if constexpr (std::is_same_v<InstrType,
                                              LoadAndStoreInstruction>) {
            std::visit(
                [this](const auto& instr) {
                  using InstrType = std::decay_t<decltype(instr)>;
                  if constexpr (std::is_same_v<InstrType, LoadInstruction>) {
                    const auto table = instr.table;
                    const auto index = instr.index;
                    switch (table) {
                      case TableSelectionOperand::ConstantTable: {
                        const Value v = this->constants_[index];
                        this->callframes_.top().valuestack.push(v);
                        break;
                      }
                      case TableSelectionOperand::GlobalTable: {
                        const Value v = this->globals_[index];
                        this->callframes_.top().valuestack.push(v);
                        break;
                      }
                      case TableSelectionOperand::LocalVariableTable: {
                        const Value v =
                            this->callframes_.top().local_vars[index];
                        this->callframes_.top().valuestack.push(v);
                        break;
                      }
                    }
                  } else if constexpr (std::is_same_v<InstrType,
                                                      StoreInstruction>) {
                    const auto table = instr.table;
                    const auto index = instr.index;
                    switch (table) {
                      case TableSelectionOperand::ConstantTable: {
                        const Value v =
                            this->callframes_.top().valuestack.top();
                        this->callframes_.top().valuestack.pop();
                        this->constants_[index] = v;
                        break;
                      }
                      case TableSelectionOperand::GlobalTable: {
                        const Value v =
                            this->callframes_.top().valuestack.top();
                        this->callframes_.top().valuestack.pop();
                        this->globals_[index] = v;
                        break;
                      }
                      case TableSelectionOperand::LocalVariableTable: {
                        const Value v =
                            this->callframes_.top().valuestack.top();
                        this->callframes_.top().valuestack.pop();
                        this->callframes_.top().local_vars[index] = v;
                        break;
                      }
                    }
                  }
                },
                instr);
          } else if constexpr (std::is_same_v<InstrType,
                                              UnaryOperationInstruction>) {
            std::visit(
                [this](const auto& instr) {
                  using InstrType = std::decay_t<decltype(instr)>;
                  auto& valuestack = this->callframes_.top().valuestack;
                  auto v = valuestack.top();
                  valuestack.pop();
                  if constexpr (std::is_same_v<InstrType, NegInstruction>) {
                    auto v_inner = std::get<NumberValue>(v);
                    v = -v_inner;
                  } else if constexpr (std::is_same_v<InstrType,
                                                      LogicalNotInstruction>) {
                    auto v_inner = std::get<BooleanValue>(v);
                    v = !v_inner;
                  }
                  valuestack.push(v);
                },
                instr);
          } else if constexpr (std::is_same_v<InstrType,
                                              BinaryOperationInstruction>) {
            std::visit(
                [this](const auto& instr) {
                  using InstrType = std::decay_t<decltype(instr)>;
                  auto& valuestack = this->callframes_.top().valuestack;
                  const auto epsilon = std::numeric_limits<double>::epsilon();

                  // 注意rhs后被入栈，所以第一个出栈的应该是rhs
                  // dirty work应该交给vm内部来做，而非交给生成字节码的用户
                  const auto rhs = valuestack.top();
                  valuestack.pop();
                  const auto lhs = valuestack.top();
                  valuestack.pop();
                  Value result;
                  if constexpr (std::is_same_v<InstrType, AddInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    result = lhs_inner + rhs_inner;
                  } else if constexpr (std::is_same_v<InstrType,
                                                      SubInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    result = lhs_inner - rhs_inner;
                  } else if constexpr (std::is_same_v<InstrType,
                                                      MulInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    result = lhs_inner * rhs_inner;
                  } else if constexpr (std::is_same_v<InstrType,
                                                      DivInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    result = lhs_inner / rhs_inner;
                  } else if constexpr (std::is_same_v<InstrType,
                                                      RemInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    result = std::fmod(lhs_inner, rhs_inner);
                  } else if constexpr (std::is_same_v<InstrType,
                                                      LessInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    result = this->compare(lhs_inner, rhs_inner,
                                           CompareMethod::Less);
                  } else if constexpr (std::is_same_v<InstrType,
                                                      LeInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    result =
                        this->compare(lhs_inner, rhs_inner, CompareMethod::Le);
                  } else if constexpr (std::is_same_v<InstrType,
                                                      EqInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    result =
                        this->compare(lhs_inner, rhs_inner, CompareMethod::Eq);
                  } else if constexpr (std::is_same_v<InstrType,
                                                      GeInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    result =
                        this->compare(lhs_inner, rhs_inner, CompareMethod::Ge);
                  } else if constexpr (std::is_same_v<InstrType,
                                                      GreaterInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    result = this->compare(lhs_inner, rhs_inner,
                                           CompareMethod::Greater);
                  } else if constexpr (std::is_same_v<InstrType,
                                                      NotEqInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    result = this->compare(lhs_inner, rhs_inner,
                                           CompareMethod::NotEq);
                  } else if constexpr (std::is_same_v<InstrType,
                                                      LogicalAndInstruction>) {
                    const auto lhs_inner = std::get<BooleanValue>(lhs);
                    const auto rhs_inner = std::get<BooleanValue>(rhs);
                    result = (lhs_inner && rhs_inner);
                  } else if constexpr (std::is_same_v<InstrType,
                                                      LogicalOrInstruction>) {
                    const auto lhs_inner = std::get<BooleanValue>(lhs);
                    const auto rhs_inner = std::get<BooleanValue>(rhs);
                    result = (lhs_inner || rhs_inner);
                  }
                  valuestack.push(result);
                },
                instr);
          } else if constexpr (std::is_same_v<InstrType,
                                              ControlFlowInstruction>) {
            std::visit(
                [this](const auto& instr) {
                  using InstrType = std::decay_t<decltype(instr)>;

                  // 无条件跳转、call、return指令单独处理
                  if constexpr (std::is_same_v<InstrType, JumpInstruction>) {
                    const auto offset = instr.offset;
                    this->pc_ += offset;
                    return;
                  } else if constexpr (std::is_same_v<InstrType,
                                                      CallInstruction>) {
                    const auto index = instr.index;
                    const auto func = this->funcs_[index];
                    const auto new_callframe =
                        CallFrame{.func{func},
                                  .valuestack{},
                                  .local_vars{},
                                  .parent_pc{this->pc_ + 1}};
                    this->callframes_.push(new_callframe);
                    this->pc_ = 0;
                    return;
                  } else if constexpr (std::is_same_v<InstrType,
                                                      ReturnInstruction>) {
                    const auto parent_pc = this->callframes_.top().parent_pc;
                    this->callframes_.pop();
                    this->pc_ = parent_pc;
                    return;
                  }

                  auto& valuestack = this->callframes_.top().valuestack;
                  const auto rhs = valuestack.top();
                  valuestack.pop();
                  const auto lhs = valuestack.top();
                  valuestack.pop();
                  if constexpr (std::is_same_v<InstrType,
                                               IfLessJumpInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    if (this->compare(lhs_inner, rhs_inner,
                                      CompareMethod::Less)) {
                      this->pc_ += instr.offset;
                    }
                  } else if constexpr (std::is_same_v<InstrType,
                                                      IfLeJumpInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    if (this->compare(lhs_inner, rhs_inner,
                                      CompareMethod::Le)) {
                      this->pc_ += instr.offset;
                    }
                  } else if constexpr (std::is_same_v<InstrType,
                                                      IfEqJumpInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    if (this->compare(lhs_inner, rhs_inner,
                                      CompareMethod::Eq)) {
                      this->pc_ += instr.offset;
                    }
                  } else if constexpr (std::is_same_v<InstrType,
                                                      IfGeJumpInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    if (this->compare(lhs_inner, rhs_inner,
                                      CompareMethod::Ge)) {
                      this->pc_ += instr.offset;
                    }
                  } else if constexpr (std::is_same_v<
                                           InstrType,
                                           IfGreaterJumpInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    if (this->compare(lhs_inner, rhs_inner,
                                      CompareMethod::Greater)) {
                      this->pc_ += instr.offset;
                    }
                  } else if constexpr (std::is_same_v<InstrType,
                                                      IfNotEqJumpInstruction>) {
                    const auto lhs_inner = std::get<NumberValue>(lhs);
                    const auto rhs_inner = std::get<NumberValue>(rhs);
                    if (this->compare(lhs_inner, rhs_inner,
                                      CompareMethod::NotEq)) {
                      this->pc_ += instr.offset;
                    }
                  }
                },
                instr);
          } else if constexpr (std::is_same_v<InstrType,
                                              ArgAndRetValInstruction>) {
            std::visit(
                [this](const auto& instr) {
                  using InstrType = std::decay_t<decltype(instr)>;
                  auto& valuestack = this->callframes_.top().valuestack;

                  if constexpr (std::is_same_v<InstrType,
                                               PushParamInstruction>) {
                    const auto param = valuestack.top();
                    valuestack.pop();
                    this->args_.push(param);
                  } else if constexpr (std::is_same_v<InstrType,
                                                      PopParamInstruction>) {
                    const auto param = this->args_.front();
                    this->args_.pop();
                    this->callframes_.top().valuestack.push(param);
                  } else if constexpr (std::is_same_v<InstrType,
                                                      PushRetValInstruction>) {
                    const auto retval = valuestack.top();
                    valuestack.pop();
                    this->retvals_.push(retval);
                  } else if constexpr (std::is_same_v<InstrType,
                                                      PopRetValInstruction>) {
                    const auto retval = this->retvals_.front();
                    this->retvals_.pop();
                    this->callframes_.top().valuestack.push(retval);
                  }
                },
                instr);
          }
        },
        instr);
  }
}

Instruction Vm::decode(const std::uint32_t instr) const {
  const std::uint16_t low_16_mask = 0xffff;
  const std::uint8_t low_3_mask = 0b111;

  const auto opcode = static_cast<OpCode>(instr >> 24);
  switch (opcode) {
    // 栈操作指令
    case OpCode::Push: {
      const auto table_kind =
          static_cast<TableSelectionOperand>((instr >> 16) & low_3_mask);
      const auto index = static_cast<IndexOperand>(instr & low_16_mask);
      return PushInstruction{.table = table_kind, .index = index};
    }
    case OpCode::Pop: {
      return PopInstruction{};
    }

    // 存取指令
    case OpCode::Load: {
      const auto table_kind =
          static_cast<TableSelectionOperand>((instr >> 16) & low_3_mask);
      const auto index = static_cast<IndexOperand>(instr & low_16_mask);
      return LoadInstruction{.table = table_kind, .index = index};
    }
    case OpCode::Store: {
      const auto table_kind =
          static_cast<TableSelectionOperand>((instr >> 16) & low_3_mask);
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

    // 函数参数和返回值相关指令
    case OpCode::PushParam: {
      return PushParamInstruction{};
    }
    case OpCode::PopParam: {
      return PopParamInstruction{};
    }
    case OpCode::PushRetVal: {
      return PushRetValInstruction{};
    }
    case OpCode::PopRetVal: {
      return PopRetValInstruction{};
    }

    default:
      std::unreachable();
  }
}

bool Vm::compare(NumberValue a, NumberValue b, CompareMethod m) const {
  const auto epsilon = std::numeric_limits<NumberValue>::epsilon();
  switch (m) {
    case CompareMethod::Less:
      return ((a - b) < -epsilon);
    case CompareMethod::Le:
      return ((a - b) <= -epsilon);
    case CompareMethod::Eq:
      return (std::abs(a - b) < epsilon);
    case CompareMethod::Ge:
      return ((a - b) >= epsilon);
    case CompareMethod::Greater:
      return ((a - b) > epsilon);
    case CompareMethod::NotEq:
      return (std::abs(a - b) >= epsilon);

    default:
      std::unreachable();
  }
}
