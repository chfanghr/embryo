//
// Created by 方泓睿 on 2020/4/9.
//

#include <embryo/vm/inst_table.h>
#include <embryo/vm/inst_operators.h>
#include <embryo/vm/inst_load.h>
#include <embryo/vm/inst_misc.h>
#include <embryo/vm/opcodes.h>
#include <embryo/vm/inst_call.h>
#include <embryo/vm/inst_up_value.h>

namespace embryo::vm {
const std::vector<Opcode> kOpcodes{ // NOLINT(cert-err58-cpp)
    {0, 1, OpArg::kR, OpArg::kN, OpMode::kABC, "MOVE", Move},
    {0, 1, OpArg::kK, OpArg::kN, OpMode::kABx, "LOADK", LoadK},
    {0, 1, OpArg::kN, OpArg::kN, OpMode::kABx, "LOADKX", LoadKx},
    {0, 1, OpArg::kU, OpArg::kU, OpMode::kABC, "LOADBOOL", LoadBool},
    {0, 1, OpArg::kU, OpArg::kN, OpMode::kABC, "LOADNIL", LoadNil},
    {0, 1, OpArg::kU, OpArg::kN, OpMode::kABC, "GETUPVAL", GetUpVal},
    {0, 1, OpArg::kU, OpArg::kK, OpMode::kABC, "GETTABUP", GetTabUp},
    {0, 1, OpArg::kR, OpArg::kK, OpMode::kABC, "GETTABLE", GetTable},
    {0, 0, OpArg::kK, OpArg::kK, OpMode::kABC, "SETTABUP", SetTabUp},
    {0, 0, OpArg::kU, OpArg::kN, OpMode::kABC, "SETUPVAL", SetUpVal},
    {0, 0, OpArg::kK, OpArg::kK, OpMode::kABC, "SETTABLE", SetTable},
    {0, 1, OpArg::kU, OpArg::kU, OpMode::kABC, "NEWTABLE", NewTable},
    {0, 1, OpArg::kR, OpArg::kK, OpMode::kABC, "SELF", Self},
    {0, 1, OpArg::kK, OpArg::kK, OpMode::kABC, "ADD", Add},
    {0, 1, OpArg::kK, OpArg::kK, OpMode::kABC, "SUB", Sub},
    {0, 1, OpArg::kK, OpArg::kK, OpMode::kABC, "MUL", Mul},
    {0, 1, OpArg::kK, OpArg::kK, OpMode::kABC, "MOD", Mod},
    {0, 1, OpArg::kK, OpArg::kK, OpMode::kABC, "POW", Pow},
    {0, 1, OpArg::kK, OpArg::kK, OpMode::kABC, "DIV", Div},
    {0, 1, OpArg::kK, OpArg::kK, OpMode::kABC, "IDIV", IDiv},
    {0, 1, OpArg::kK, OpArg::kK, OpMode::kABC, "BAND", BAnd},
    {0, 1, OpArg::kK, OpArg::kK, OpMode::kABC, "BOR", BOr},
    {0, 1, OpArg::kK, OpArg::kK, OpMode::kABC, "BXOR", BXor},
    {0, 1, OpArg::kK, OpArg::kK, OpMode::kABC, "SHL", ShL},
    {0, 1, OpArg::kK, OpArg::kK, OpMode::kABC, "SHR", ShR},
    {0, 1, OpArg::kR, OpArg::kN, OpMode::kABC, "UNM", UnM},
    {0, 1, OpArg::kR, OpArg::kN, OpMode::kABC, "BNOT", BNot},
    {0, 1, OpArg::kR, OpArg::kN, OpMode::kABC, "NOT", Not},
    {0, 1, OpArg::kR, OpArg::kN, OpMode::kABC, "LEN", Len},
    {0, 1, OpArg::kR, OpArg::kR, OpMode::kABC, "CONCAT", Concat},
    {0, 0, OpArg::kR, OpArg::kN, OpMode::kAsBx, "JMP", Jmp},
    {1, 0, OpArg::kK, OpArg::kK, OpMode::kABC, "EQ", EQ},
    {1, 0, OpArg::kK, OpArg::kK, OpMode::kABC, "LT", LT},
    {1, 0, OpArg::kK, OpArg::kK, OpMode::kABC, "LE", LE},
    {1, 0, OpArg::kN, OpArg::kU, OpMode::kABC, "TEST", Test},
    {1, 1, OpArg::kR, OpArg::kU, OpMode::kABC, "TESTSET", TestSet},
    {0, 1, OpArg::kU, OpArg::kU, OpMode::kABC, "CALL", Call},
    {0, 1, OpArg::kU, OpArg::kU, OpMode::kABC, "TAILCALL", TailCall},
    {0, 0, OpArg::kU, OpArg::kN, OpMode::kABC, "RETURN", Return},
    {0, 1, OpArg::kN, OpArg::kN, OpMode::kAsBx, "FORLOOP", ForLoop},
    {0, 1, OpArg::kR, OpArg::kN, OpMode::kAsBx, "FORPREP", ForPrep},
    {0, 0, OpArg::kN, OpArg::kU, OpMode::kABC, "TFORCALL", TForCall},
    {0, 1, OpArg::kR, OpArg::kN, OpMode::kAsBx, "TFORLOOP", TForLoop},
    {0, 0, OpArg::kU, OpArg::kU, OpMode::kABC, "SETLIST", SetList},
    {0, 1, OpArg::kU, OpArg::kN, OpMode::kABx, "CLOSURE", Closure},
    {0, 1, OpArg::kU, OpArg::kN, OpMode::kABC, "VARARG", VarArg},
    {0, 0, OpArg::kU, OpArg::kU, OpMode::kAx, "EXTRAARG", nullptr},
};

}