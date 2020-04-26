//
// Created by 方泓睿 on 2020/4/8.
//

#include <stdexcept>

#include <embryo/utils/inspection.h>
#include <embryo/state/lua_state.h>

namespace embryo::state {
namespace {
bool EQInternal(const LuaValue &a, const LuaValue &b, const std::shared_ptr<LuaState> &state) {
  switch (TypeOf(a)) {
    case api::LuaValueType::kNil:return TypeOf(b) == api::LuaValueType::kNil;
    case api::LuaValueType::kBoolean:return ToBoolean(a) == ToBoolean(b);
    case api::LuaValueType::kString: {
      auto[val, ok]=TryCast<std::string>(b);
      return ok && val == Cast<std::string>(a);
    }
    case api::LuaValueType::kNumber: {
      auto[a_i, a_d]=OptionalNumber(a);
      double a_val = a_i ? a_i.value() : a_d.value();
      auto[b_i, b_d]=OptionalNumber(b);
      if (!(b_i || b_d))
        return false;
      double b_val = b_i ? b_i.value() : b_d.value();
      return a_val == b_val;
    }
    case api::LuaValueType::kTable: {
      auto x = Cast<LuaTablePtr>(a);
      auto[y, ok]= TryCast<LuaTablePtr>(b);
      if (x != y) {
        if (ok && state) {
          auto res = CallMetaMethod(a, b, "__eq", state);
          if (res)
            return state::ToBoolean(res);
        }
      } else return true;
      return a == b;
    }
    default:
      throw std::runtime_error(
          MSG_WITH_FUNC_NAME("EQ comparision unsupported on this type"));
  }
}

bool LTInternal(const LuaValue &a, const LuaValue &b, const std::shared_ptr<LuaState> &state) {
  switch (TypeOf(a)) {
    case api::LuaValueType::kString: {
      auto[val, ok]=TryCast<std::string>(b);
      return ok && Cast<std::string>(a) < val;
    }
    case api::LuaValueType::kNumber: {
      auto[a_i, a_d]=OptionalNumber(a);
      double a_val = a_i ? a_i.value() : a_d.value();
      auto[b_i, b_d]=OptionalNumber(b);
      if (!(b_i || b_d))
        return false;
      double b_val = b_i ? b_i.value() : b_d.value();
      return a_val < b_val;
    }
    case api::LuaValueType::kTable: {
      auto x = Cast<LuaTablePtr>(a);
      auto[y, ok]= TryCast<LuaTablePtr>(b);
      if (ok) {
        auto res = CallMetaMethod(a, b, "__lt", state);
        if (res)
          return state::ToBoolean(res);
      }
    }
    default:;
  }
  throw std::runtime_error(
      MSG_WITH_FUNC_NAME("LT comparision unsupported on this type"));
}

bool LEInternal(const LuaValue &a, const LuaValue &b, const std::shared_ptr<LuaState> &state) {
  switch (TypeOf(a)) {
    case api::LuaValueType::kString: {
      auto[val, ok]=TryCast<std::string>(b);
      return ok && Cast<std::string>(a) <= val;
    }
    case api::LuaValueType::kNumber: {
      auto[a_i, a_d]=OptionalNumber(a);
      double a_val = a_i ? a_i.value() : a_d.value();
      auto[b_i, b_d]=OptionalNumber(b);
      if (!(b_i || b_d))
        return false;
      double b_val = b_i ? b_i.value() : b_d.value();
      return a_val <= b_val;
    }
    case api::LuaValueType::kTable: {
      auto x = Cast<LuaTablePtr>(a);
      auto[y, ok]= TryCast<LuaTablePtr>(b);
      if (ok) {
        auto res = CallMetaMethod(a, b, "__le", state);
        if (res)
          return state::ToBoolean(res);
      }
    }
    default:;
  }
  throw std::runtime_error(
      MSG_WITH_FUNC_NAME("LE comparision unsupported on this type"));
}
}

bool LuaState::Compare(int64_t idx_1, int64_t idx_2, api::CompareOp op) {
  auto a = stack_->Get(idx_1);
  auto b = stack_->Get(idx_2);

  switch (op) {
    case api::CompareOp::kEQ:return EQInternal(a, b, shared_from_this());
    case api::CompareOp::kLT:return LTInternal(a, b, shared_from_this());
    case api::CompareOp::kLE:return LEInternal(a, b, shared_from_this());
    default:Error(MSG_WITH_FUNC_NAME("unknown comparision op"));
  }
}

bool LuaState::RawEqual(int64_t idx_1, int64_t idx_2) {
  if (!stack_->IsValidIndex(idx_1) || !stack_->IsValidIndex(idx_2))
    return false;

  auto a = stack_->Get(idx_1);
  auto b = stack_->Get(idx_2);

  return EQInternal(a, b, nullptr);
}
}