#include <gtest/gtest.h>

#include <string>
#include <variant>
#include <vector>

#include "../src/function.h"
#include "../src/program.h"
#include "../src/value.h"
#include "../src/vm.h"

class SingleInstructionTest : public testing::Test {
 protected:
  Vm vm;

  SingleInstructionTest() : vm{} {}

  void load_and_run(const Program prog) {
    vm.load_program(prog);
    vm.run();
  }

  Program generate_program(const std::vector<Function> funcs,
                           std::vector<Value> constants) {
    const auto prog = Program{
        .minor_version{1},
        .major_version{0},
        .constants{constants},
        .funcs{funcs},
    };
    return prog;
  }

  Function generate_function(std::string name, std::uint8_t param_count,
                             std::vector<std::uint32_t> instrs) {
    const auto func =
        Function{.name{name}, .param_count{param_count}, .instructions{instrs}};
    return func;
  }
};

TEST_F(SingleInstructionTest, PushInstruction) {
  const auto func = generate_function("main", 0, {0x00000000});
  const auto prog = generate_program({func}, {1.0});
  load_and_run(prog);

  const auto& valuestack = this->vm.callframes_.top().valuestack;
  EXPECT_EQ(std::get<NumberValue>(valuestack.top()), 1.0);
}

TEST_F(SingleInstructionTest, PopInstruction) {
  const auto func = generate_function("main", 0, {0x00000000, 0x01000000});
  const auto prog = generate_program({func}, {1.0});
  load_and_run(prog);

  const auto& valuestack = this->vm.callframes_.top().valuestack;
  EXPECT_EQ(valuestack.size(), 0);
}

TEST_F(SingleInstructionTest, LoadInstruction) {
  const auto func = generate_function("main", 0, {0x02000000});
  const auto prog = generate_program({func}, {1.0});
  load_and_run(prog);

  const auto& valuestack = this->vm.callframes_.top().valuestack;
  EXPECT_EQ(std::get<NumberValue>(valuestack.top()), 1.0);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
