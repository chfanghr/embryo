//
// Created by 方泓睿 on 2020/4/9.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_VM_CONSTANTS_H_
#define EMBRYO_INCLUDE_EMBRYO_VM_CONSTANTS_H_

#include <embryo/utils/types.h>

namespace embryo::vm {
enum OpMode : Byte {
  kABC,
  kABx,
  kAsBx,
  kAx
};

enum class Op : int32_t {
  kMove,
  kLoadK,
  kLoadKx,
  kLoadBool,
  kLoadNil,
  kGetUpVal,
  kGetTabUp,
  kGetTable,
  kSetTabUp,
  kSetUpVal,
  kSetTable,
  kNewTable,
  kSelf,
  kAdd,
  kSub,
  kMul,
  kMod,
  kPow,
  kDiv,
  kIDiv,
  kBAnd,
  kBOr,
  kBXor,
  kShL,
  kShR,
  kUnM,
  kBNot,
  kNot,
  kLen,
  kConcat,
  kJmp,
  kEQ,
  kLT,
  kLE,
  kTest,
  kTestSet,
  kCall,
  kTailCall,
  kReturn,
  kForLoop,
  kForPrep,
  kTForCall,
  kTForLoop,
  kSetList,
  kClosure,
  kVarArg,
  kExtraArg
};

enum OpArg : Byte { // argument
  kN, // not used
  kU, // used
  kR, // a register or a jump offset
  kK // constant or register/constant
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_VM_CONSTANTS_H_
