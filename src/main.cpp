#include <format>
#include <iostream>

#include "parse.h"
#include "vm.h"

Program mock_program() {
  const auto func = Function{.name{"main"},
                             .instructions{0b00000001000000000000000000000000}};
  const auto prog = Program{.magic_number{},
                            .minor_version{},
                            .major_version{},
                            .constant_count{},
                            .constants{},
                            .func_count{1},
                            .funcs{func}};
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
