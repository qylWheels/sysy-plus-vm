#pragma once

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <queue>
#include <stack>
#include <vector>

#include "function.h"
#include "instruction.h"
#include "program.h"

struct CallFrame;

class Vm {
 public:
  Vm();
  void load_program(const Program prog);
  void run();
  void gc();

 private:
  std::size_t pc_;                         // 程序计数器
  std::stack<CallFrame> callframes_;       // 栈帧
  std::vector<Value> globals_;             // 全局变量
  std::vector<Value> constants_;           // 常量池
  std::vector<Function> funcs_;            // 函数
  [[deprecated]] std::queue<Value> args_;  // 函数调用时caller传递给callee的参数
  [[deprecated]] std::queue<Value>
      retvals_;  // 函数返回时callee返回给caller的返回值

  // 数字型类型的值的比较方法分类
  enum class CompareMethod { Less, Le, Eq, Ge, Greater, NotEq };

 private:
  Instruction decode(const std::uint32_t instr) const;
  bool compare(NumberValue a, NumberValue b, CompareMethod m) const;

  // 用于测试的友元类
  FRIEND_TEST(SingleInstructionTest, PushInstruction);
  FRIEND_TEST(SingleInstructionTest, PopInstruction);
  FRIEND_TEST(SingleInstructionTest, LoadInstruction);
  FRIEND_TEST(SingleInstructionTest, StoreInstruction);
  FRIEND_TEST(SingleInstructionTest, NegInstruction);
  FRIEND_TEST(SingleInstructionTest, LogicalNotInstruction);
  FRIEND_TEST(SingleInstructionTest, AddInstruction);
  FRIEND_TEST(SingleInstructionTest, SubInstruction);
  FRIEND_TEST(SingleInstructionTest, MulInstruction);
  FRIEND_TEST(SingleInstructionTest, DivInstruction);
  FRIEND_TEST(SingleInstructionTest, RemInstruction);
  FRIEND_TEST(SingleInstructionTest, LessInstruction);
  FRIEND_TEST(SingleInstructionTest, LeInstruction);
  FRIEND_TEST(SingleInstructionTest, EqInstruction);
  FRIEND_TEST(SingleInstructionTest, GeInstruction);
  FRIEND_TEST(SingleInstructionTest, GreaterInstruction);
  FRIEND_TEST(SingleInstructionTest, NotEqInstruction);
  FRIEND_TEST(SingleInstructionTest, LogicalAndInstruction);
  FRIEND_TEST(SingleInstructionTest, LogicalOrInstruction);
  FRIEND_TEST(SingleInstructionTest, JumpInstruction);
  FRIEND_TEST(SingleInstructionTest, IfLessJumpInstruction);
  FRIEND_TEST(SingleInstructionTest, IfLeJumpInstruction);
  FRIEND_TEST(SingleInstructionTest, IfEqJumpInstruction);
  FRIEND_TEST(SingleInstructionTest, IfGeJumpInstruction);
  FRIEND_TEST(SingleInstructionTest, IfGreaterJumpInstruction);
  FRIEND_TEST(SingleInstructionTest, IfNotEqJumpInstruction);
  FRIEND_TEST(SingleInstructionTest, CallAndReturnInstruction);
};

struct CallFrame {
  Function func;                  // 该栈帧所属的函数
  std::stack<Value> valuestack;   // 值栈
  std::vector<Value> local_vars;  // 局部变量
  std::size_t parent_pc;          // 保存上一个栈帧的pc + 1
};
