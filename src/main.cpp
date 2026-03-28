#include <format>
#include <iostream>
// #include <utility>  // FIXME: 仅用于测试

#include "parse.h"
#include "value.h"  // FIXME: 仅用于测试
#include "vm.h"

Program mock_program() {
  const auto main = Function{.name{"main"},
                             .param_count{0},
                             .instructions{std::vector<std::uint32_t>{
                                 0x02000000,  // load CONSTANT_TABLE[0]
                                 0x02000001,  // load CONSTANT_TABLE[1]
                                 0x1a000001,  // call FUNCTION_TABLE[1]
                             }}};
  const auto sub = Function{.name{"sub"},
                            .param_count{2},
                            .instructions{std::vector<std::uint32_t>{
                                0x02020000,  // load LOCAL_VAR_TABLE[0]
                                0x02020001,  // load LOCAL_VAR_TABLE[1]
                                0x07000000,  // sub
                                0x1b000000,  // return
                            }}};
  const auto prog = Program{.minor_version{1},
                            .major_version{0},
                            .constants{std::vector<Value>{1.1, 2.2, 3.3}},
                            .funcs{main, sub}};
  return prog;
}

int main(int argc, char* argv[]) {
  // auto parser = Parser();
  // const auto prog = parser.parse(argv[1]);
  const auto prog = mock_program();
  auto vm = Vm();
  vm.load_program(prog);
  vm.run();

  return 0;
}
