#pragma once

#include <cstdint>
#include <vector>

#include "function.h"
#include "value.h"

// sysy-plus字节码文件结构：
// [4字节] magic number: 0x53 0x59 0x73 0x79 ("SYsy")
// [2字节] 次版本号
// [2字节] 主版本号
// [4字节] 常量池大小（条目数量）
// [sizeof(constant_value，定长) * N] 常量池
// [4字节] 函数个数
// [sizeof(function，变长) * N] 函数

struct Program {
  std::uint32_t magic_number;
  std::uint16_t minor_version;
  std::uint16_t major_version;
  std::uint32_t constant_count;
  std::vector<Value> constants;
  std::uint32_t func_count;
  std::vector<Function> funcs;
};