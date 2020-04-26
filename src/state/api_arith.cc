//
// Created by 方泓睿 on 2020/4/8.
//

#include <cmath>
#include <functional>
#include <array>

#include <embryo/utils/inspection.h>
#include <embryo/utils/numbers.h>
#include <embryo/state/lua_state.h>

namespace embryo::state {
namespace {
template<typename T>
T Add(T a, T b) { return a + b; }

template<typename T>
T Sub(T a, T b) { return a - b; }

template<typename T>
T Mul(T a, T b) { return a * b; }

template<typename T>
T Pow(T a, T b) { return std::pow(a, b); }

template<typename T>
T Div(T a, T b) { return a / b; }

template<typename T>
T IDiv(T a, T b);

template<typename T>
T Mod(T a, T b);

template<>
double Mod<double>(double a, double b) { return utils::Mod(a, b); }

template<>
int64_t Mod<int64_t>(int64_t a, int64_t b) { return utils::Mod(a, b); }

template<>
double IDiv<double>(double a, double b) {
  return utils::FloorDiv(a, b);
}

template<>
int64_t IDiv<int64_t>(int64_t a, int64_t b) {
  return utils::FloorDiv(a, b);
}

int64_t BAnd(int64_t a, int64_t b) { return a & b; } // NOLINT(hicpp-signed-bitwise)

int64_t BOr(int64_t a, int64_t b) { return a | b; } // NOLINT(hicpp-signed-bitwise)

int64_t BXor(int64_t a, int64_t b) { return a ^ b; } // NOLINT(hicpp-signed-bitwise)

int64_t ShL(int64_t a, int64_t b) { return utils::ShiftLeft(a, b); }

int64_t ShR(int64_t a, int64_t b) { return utils::ShiftRight(a, b); }

template<typename T>
T UnM(T a, T placeholder) { return -a; }

int64_t BNot(int64_t a, int64_t placeholder) { return ~a; } // NOLINT(hicpp-signed-bitwise)

struct Operator {
  std::string meta_method;
  std::function<int64_t(int64_t, int64_t)> i_func;
  std::function<double(double, double)> f_func;
};

const std::array<Operator, 14> kOperators = { // NOLINT(cert-err58-cpp)
    Operator{"__add", Add<int64_t>, Add<double>},
    Operator{"__sub", Sub<int64_t>, Sub<double>},
    Operator{"__mul", Mul<int64_t>, Mul<double>},
    Operator{"__mod", Mod<int64_t>, Mod<double>},
    Operator{"__pow", nullptr, Pow<double>},
    Operator{"__div", Div<int64_t>, Div<int64_t>},
    Operator{"__idiv", IDiv<int64_t>, IDiv<double>},
    Operator{"__band", BAnd, nullptr},
    Operator{"__bor", BOr, nullptr},
    Operator{"__bxor", BXor, nullptr},
    Operator{"__shl", ShL, nullptr},
    Operator{"__shr", ShR, nullptr},
    Operator{"__unm", UnM<int64_t>, UnM<double>},
    Operator{"__bnot", BNot, nullptr}
};

LuaValue ArithInternal(const LuaValue &a, const LuaValue &b, const Operator &op_func) {
  if (!op_func.f_func) {
    auto[x, x_ok]= state::ToInteger(a);
    auto[y, y_ok]= state::ToInteger(b);
    if (x_ok && y_ok)
      return op_func.i_func(x, y);
  } else {
    if (op_func.i_func) {
      auto x_ok = std::holds_alternative<int64_t>(a.value());
      auto y_ok = std::holds_alternative<int64_t>(b.value());
      if (x_ok && y_ok) {
        auto x = std::get<int64_t>(a.value());
        auto y = std::get<int64_t>(b.value());
        return op_func.i_func(x, y);
      }
    }
    auto[x, x_ok]= state::ToFloat(a);
    auto[y, y_ok]= state::ToFloat(b);
    if (x_ok && y_ok)
      return op_func.f_func(x, y);
  }
  return {};
}
}

void LuaState::Arith(api::ArithOp op) {
  LuaValue a{nullptr}, b{nullptr};
  b = stack_->Pop();
  if (op != api::ArithOp::kUnM && op != api::ArithOp::kBNot)
    a = stack_->Pop();
  else a = b;

  auto op_func = kOperators[static_cast<size_t >(op)];
  {
    auto res = ArithInternal(a, b, op_func);
    if (res) {
      stack_->Push(res);
      return;
    }
  }
  {
    auto meta_method = op_func.meta_method;
    auto res = state::CallMetaMethod(a, b, meta_method, shared_from_this());
    if (res) {
      stack_->Push(res);
      return;
    }
  }
  Error(MSG_WITH_FUNC_NAME("arithmetic error"));
}
}