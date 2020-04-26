//
// Created by 方泓睿 on 2020/4/8.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_API_CONSTANTS_H_
#define EMBRYO_INCLUDE_EMBRYO_API_CONSTANTS_H_

#include <cstdlib>
#include <string>

namespace embryo::api {
enum class LuaValueType {
  kNil,
  kBoolean,
  kLightUserData,
  kNumber,
  kString,
  kTable,
  kFunction,
  kUserData,
  kThread,
  kNone = -1,
};

enum class ArithOp {
  kAdd, // +
  kSub, // -
  kMul, // *
  kMod, // %
  kPow, // ^
  kDiv, // /
  kIDiv, // //
  kBAnd, // &
  kBOr, // |
  kBXor, // ~
  kShL, // <<
  kShR, // >>
  kUnM, // -
  kBNot // ~
};

enum class CompareOp {
  kEQ,
  kLT,
  kLE
};

enum class Status {
  kOk,
  kYield,
  kErrRun,
  kErrSyntax,
  kErrMem,
  kErrGCMM,
  kErrErr,
  kErrFile
};

const int64_t kMultert = -1;
const int64_t kLuaMinStack = 20;
const int64_t kLuaIMaxStack = 1000000;
const int64_t kLuaRegistryIndex = -kLuaIMaxStack - 1000;
const int64_t kLuaRIdxGlobals = 2;
const int64_t kLuaRIdxMainThread = 1;
}
#endif //EMBRYO_INCLUDE_EMBRYO_API_CONSTANTS_H_
