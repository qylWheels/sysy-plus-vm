#include <gtest/gtest.h>

#include <cmath>
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

  // TODO: 取消对generate_program和generate_function的调用
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

TEST_F(SingleInstructionTest, StoreInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x03010000, 0x02000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{1.0},
      .global_table_size{1},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(std::get<NumberValue>(this->vm.globals_[0]), 1.0);
  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().local_vars[0]), 1.0);
}

TEST_F(SingleInstructionTest, NegInstruction) {
  const auto func = Function{.name{"main"},
                             .param_count{0},
                             .local_var_count{1},
                             .instructions{0x02000000, 0x04000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{1.0},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().local_vars[0]), -1.0);
}

TEST_F(SingleInstructionTest, LogicalNotInstruction) {
  const auto func = Function{.name{"main"},
                             .param_count{0},
                             .local_var_count{1},
                             .instructions{0x02000000, 0x05000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{false},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_EQ(std::get<BooleanValue>(this->vm.callframes_.top().local_vars[0]),
            true);
}

TEST_F(SingleInstructionTest, AddInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000001, 0x06000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{2.3, 1.1},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().local_vars[0]), 3.4);
}

TEST_F(SingleInstructionTest, SubInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000001, 0x07000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{2.3, 1.1},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().local_vars[0]), 1.2);
}

TEST_F(SingleInstructionTest, MulInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000001, 0x08000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{2.3, 1.1},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().local_vars[0]),
      2.3 * 1.1);
}

TEST_F(SingleInstructionTest, DivInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000001, 0x09000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{2.3, 1.1},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().local_vars[0]),
      2.3 / 1.1);
}

TEST_F(SingleInstructionTest, RemInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000001, 0x0a000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{2.3, 1.1},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().local_vars[0]),
      std::fmod(2.3, 1.1));
}

TEST_F(SingleInstructionTest, LessInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000002, 0x0b000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{43.9, 62.1, 5.9},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_EQ(std::get<BooleanValue>(this->vm.callframes_.top().local_vars[0]),
            false);
}

TEST_F(SingleInstructionTest, LeInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000001, 0x0c000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{43.9, 43.9, 5.9},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_EQ(std::get<BooleanValue>(this->vm.callframes_.top().local_vars[0]),
            true);
}

TEST_F(SingleInstructionTest, EqInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000001, 0x0d000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{43.9, 43.9, 5.9},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_EQ(std::get<BooleanValue>(this->vm.callframes_.top().local_vars[0]),
            true);
}

TEST_F(SingleInstructionTest, GeInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000001, 0x0e000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{43.9, 43.9, 5.9},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_EQ(std::get<BooleanValue>(this->vm.callframes_.top().local_vars[0]),
            true);
}

TEST_F(SingleInstructionTest, GreaterInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000001, 0x0f000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{43.9000001, 43.9},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_EQ(std::get<BooleanValue>(this->vm.callframes_.top().local_vars[0]),
            true);
}

TEST_F(SingleInstructionTest, NotEqInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000001, 0x10000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{43.9, 43.900000001},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_EQ(std::get<BooleanValue>(this->vm.callframes_.top().local_vars[0]),
            true);
}

TEST_F(SingleInstructionTest, LogicalAndInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000001, 0x11000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{true, false},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_EQ(std::get<BooleanValue>(this->vm.callframes_.top().local_vars[0]),
            false);
}

TEST_F(SingleInstructionTest, LogicalOrInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{1},
               .instructions{0x02000000, 0x02000001, 0x12000000, 0x03020000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{true, false},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_EQ(std::get<BooleanValue>(this->vm.callframes_.top().local_vars[0]),
            true);
}

TEST_F(SingleInstructionTest, JumpInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{0},
               .instructions{0x13000001, 0x03020000, 0x02000001, 0x02000000}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{true, false},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_EQ(std::get<BooleanValue>(this->vm.callframes_.top().valuestack.top()),
            true);
}

TEST_F(SingleInstructionTest, IfLessJumpInstruction) {
  const auto func = Function{.name{"main"},
                             .param_count{0},
                             .local_var_count{0},
                             .instructions{0x02000000, 0x02000001, 0x14000001,
                                           0x03020000, 0x02000001}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{1.0, 2.0},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().valuestack.top()), 2.0);
}

TEST_F(SingleInstructionTest, IfLeJumpInstruction) {
  const auto func = Function{.name{"main"},
                             .param_count{0},
                             .local_var_count{0},
                             .instructions{0x02000000, 0x02000001, 0x15000001,
                                           0x03020000, 0x02000001}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{1.0, 1.0},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().valuestack.top()), 1.0);
}

TEST_F(SingleInstructionTest, IfEqJumpInstruction) {
  const auto func =
      Function{.name{"main"},
               .param_count{0},
               .local_var_count{0},
               .instructions{0x02000000, 0x02000001, 0x16000001, 0x02000001}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{1.0, 1.000000001},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().valuestack.top()),
      1.000000001);
}

TEST_F(SingleInstructionTest, IfGeJumpInstruction) {
  const auto func = Function{.name{"main"},
                             .param_count{0},
                             .local_var_count{0},
                             .instructions{0x02000000, 0x02000001, 0x17000001,
                                           0x03020000, 0x02000001}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{1.000000008, 1.000000008},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().valuestack.top()),
      1.000000008);
}

TEST_F(SingleInstructionTest, IfGreaterJumpInstruction) {
  const auto func = Function{.name{"main"},
                             .param_count{0},
                             .local_var_count{0},
                             .instructions{0x02000000, 0x02000001, 0x18000001,
                                           0x03020000, 0x02000001}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{1.000000008, 1.0000000075},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().valuestack.top()),
      1.0000000075);
}

TEST_F(SingleInstructionTest, IfNotEqJumpInstruction) {
  const auto func = Function{.name{"main"},
                             .param_count{0},
                             .local_var_count{0},
                             .instructions{0x02000000, 0x02000001, 0x19000001,
                                           0x03020000, 0x02000001}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{1.000000008, 1.0000000081},
      .global_table_size{0},
      .funcs{func},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().valuestack.top()),
      1.0000000081);
}

TEST_F(SingleInstructionTest, CallAndReturnInstruction) {
  const auto main = Function{.name{"main"},
                             .param_count{0},
                             .local_var_count{0},
                             .instructions{
                                 0x02000000,
                                 0x02000001,
                                 0x1a000001,
                             }};
  const auto add =
      Function{.name{"div"},
               .param_count{2},
               .local_var_count{0},
               .instructions{0x02020000, 0x02020001, 0x09000000, 0x1b000001}};
  const auto prog = Program{
      .minor_version{1},
      .major_version{0},
      .constants{46.0, 23.4},
      .global_table_size{0},
      .funcs{main, add},
  };
  ;
  load_and_run(prog);

  EXPECT_DOUBLE_EQ(
      std::get<NumberValue>(this->vm.callframes_.top().valuestack.top()),
      46.0 / 23.4);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
