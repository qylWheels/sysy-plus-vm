#pragma once

#include <cstddef>
#include <cstdint>
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
  std::size_t pc_;                    // 程序计数器
  std::stack<CallFrame> callframes_;  // 栈帧
  std::vector<Value> globals_;        // 全局变量
  std::vector<Value> constants_;      // 常量池
  std::vector<Function> funcs_;       // 函数

 private:
  Instruction decode(const std::uint32_t instr) const;
};

struct CallFrame {
  Function func;                  // 该栈帧所属的函数
  std::stack<Value> valuestack;   // 值栈
  std::vector<Value> local_vars;  // 局部变量
};
