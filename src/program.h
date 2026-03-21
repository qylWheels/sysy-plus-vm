#pragma once

#include <cstdint>
#include <vector>

#include "function.h"
#include "value.h"

// sysy-plus字节码文件结构：
// [4字节] magic number: 0x53 0x59 0x73 0x79 ("SYsy")
// [2字节] 次版本号
// [2字节] 主版本号
// [4字节] 函数个数
// [4字节 * N] 每个函数的函数名长度
// [4字节 * N] 每个函数的指令条数
// [sizeof(function，变长) * N] 函数
// - [变长] 函数名
// - [变长] 函数指令

struct Program {
  std::uint16_t minor_version;
  std::uint16_t major_version;
  std::vector<Function> funcs;

  // TODO: 考虑全局变量相关字段
};