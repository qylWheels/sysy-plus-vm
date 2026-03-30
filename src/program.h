#pragma once

#include <cstdint>
#include <vector>

#include "function.h"
#include "value.h"

// sysy-plus字节码文件结构：
// [4字节] magic number: 0x53 0x59 0x73 0x79 ("SYsy")
// [2字节] 次版本号
// [2字节] 主版本号
// [4字节] 常量池中的常量个数
// [sizeof(constant，变长) * N] 常量
// [4字节] 全局变量表长度
// [4字节] 函数个数
// [sizeof(function，变长) * N] 函数

struct Program {
  std::uint16_t minor_version;
  std::uint16_t major_version;
  std::vector<Value> constants;
  std::uint32_t global_table_size;
  std::vector<Function> funcs;
};