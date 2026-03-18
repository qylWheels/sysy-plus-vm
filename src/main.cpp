#include <format>
#include <iostream>

#include "parse.h"
#include "vm.h"

int main(int argc, char *argv[]) {
  auto parser = Parser();
  auto prog = parser.parse(argv[1]);
  auto vm = Vm();
  vm.load_program(prog);
  vm.run();

  return 0;
}
