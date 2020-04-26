//
// Created by 方泓睿 on 2020/4/20.
//

#include <cmath>
#include <limits>
#include <random>

#include <embryo/utils/numbers.h>
#include <embryo/std_lib/lib_math.h>

namespace embryo::std_lib {
int64_t kRandomSeed = -1;

api::FuncReg kMathLib{ // NOLINT(cert-err58-cpp)
    {"random", math::Random},
    {"randomseed", math::RandomSeed},
    {"max", math::Max},
    {"min", math::Min},
    {"exp", math::Exp},
    {"log", math::Log},
    {"deg", math::Deg},
    {"rad", math::Rad},
    {"sin", math::Sin},
    {"cos", math::Cos},
    {"tan", math::Tan},
    {"asin", math::Asin},
    {"acos", math::Acos},
    {"atan", math::Atan},
    {"ceil", math::Ceil},
    {"floor", math::Floor},
    {"fmod", math::FMod},
    {"modf", math::ModF},
    {"abs", math::Abs},
    {"sqrt", math::Sqrt},
    {"ult", math::Ult},
    {"tointeger", math::ToInteger},
    {"type", math::Type},

    {"pi", nullptr},
    {"huge", nullptr},
    {"maxinteger", nullptr},
    {"mininteger", nullptr}
};

int32_t OpenMathLib(const std::shared_ptr<api::LuaState> &state) {
  state->NewLib(kMathLib);
  state->PushNumber(double(M_PI));
  state->SetField(-2, "pi");
  state->PushNumber(math::kHuge);
  state->SetField(-2, "huge");
  state->PushInteger(math::kMaxInteger);
  state->SetField(-2, "maxinteger");
  state->PushInteger(math::kMinInteger);
  state->SetField(-2, "mininteger");
  return 1;
}

namespace math {
namespace {
template<typename T>
T Random(T lo = std::numeric_limits<T>::lowest(),
         T hi = std::numeric_limits<T>::max()) {
  std::random_device dev;
  std::mt19937_64 rng(kRandomSeed >= 0 ? kRandomSeed : dev());
  std::uniform_real_distribution<std::mt19937::result_type>
      dist(lo, hi);
  return T(dist(rng));
}
}

int32_t Random(const std::shared_ptr<api::LuaState> &state) {
  int64_t lo{}, hi{};

  switch (state->GetTop()) {
    case 0: {
      state->PushNumber(Random<double>(0, 1));
      return 1;
    }
    case 1: {
      lo = 1;
      hi = state->CheckInteger(1);
      break;
    }
    case 2: {
      lo = state->CheckInteger(1);
      hi = state->CheckInteger(2);
      break;
    }
    default:return state->Error2("wrong number of arguments");
  }

  state->ArgCheck(lo <= hi, 1, "interval is empty");
  state->ArgCheck(lo >= 0 || hi <= std::numeric_limits<int64_t>::max() + lo, 1, "interval too large");

  if (hi - lo == std::numeric_limits<int64_t>::max())
    state->PushInteger(lo + Random<int64_t>(0));
  else
    state->PushInteger(lo + Random<int64_t>(0, hi - lo + 1));
  return 1;
}

int32_t RandomSeed(const std::shared_ptr<api::LuaState> &state) {
  auto x = state->CheckNumber(1);
  kRandomSeed = x;
  return 0;
}

int32_t Max(const std::shared_ptr<api::LuaState> &state) {
  auto n = state->GetTop();
  int64_t i_max = 1;
  state->ArgCheck(n >= 1, 1, "value expected");
  for (int64_t i = 2; i <= n; i++) {
    if (state->Compare(i_max, i, api::CompareOp::kLT))
      i_max = i;
  }
  state->PushValue(i_max);
  return 1;
}

int32_t Min(const std::shared_ptr<api::LuaState> &state) {
  auto n = state->GetTop();
  int64_t i_max = 1;
  state->ArgCheck(n >= 1, 1, "value expected");
  for (int64_t i = 2; i <= n; i++) {
    if (state->Compare(i, i_max, api::CompareOp::kLT))
      i_max = i;
  }
  state->PushValue(i_max);
  return 1;
}

int32_t Exp(const std::shared_ptr<api::LuaState> &state) {
  auto x = state->CheckNumber(1);
  state->PushNumber(std::exp(x));
  return 1;
}

int32_t Log(const std::shared_ptr<api::LuaState> &state) {
  auto x = state->CheckNumber(1);
  double res{};

  if (state->IsNoneOrNil(2))
    res = std::log(x);
  else {
    auto base = state->ToNumber(2);
    if (base == 2)
      res = std::log2(x);
    else if (base == 10)
      res = std::log10(x);
    else
      res = std::log(x) / std::log(base);
  }

  state->PushNumber(res);
  return 1;
}

int32_t Deg(const std::shared_ptr<api::LuaState> &state) {
  auto x = state->CheckNumber(1);
  state->PushNumber(x * 180 / M_PI);
  return 1;
}

int32_t Rad(const std::shared_ptr<api::LuaState> &state) {
  auto x = state->CheckNumber(1);
  state->PushNumber(x * M_PI / 180);
  return 1;
}

int32_t Sin(const std::shared_ptr<api::LuaState> &state) {
  auto x = state->CheckNumber(1);
  state->PushNumber(std::sin(x));
  return 1;
}

int32_t Cos(const std::shared_ptr<api::LuaState> &state) {
  auto x = state->CheckNumber(1);
  state->PushNumber(std::cos(x));
  return 1;
}

int32_t Tan(const std::shared_ptr<api::LuaState> &state) {
  auto x = state->CheckNumber(1);
  state->PushNumber(std::tan(x));
  return 1;
}

int32_t Asin(const std::shared_ptr<api::LuaState> &state) {
  auto x = state->CheckNumber(1);
  state->PushNumber(std::asin(x));
  return 1;
}

int32_t Acos(const std::shared_ptr<api::LuaState> &state) {
  auto x = state->CheckNumber(1);
  state->PushNumber(std::acos(x));
  return 1;
}

int32_t Atan(const std::shared_ptr<api::LuaState> &state) {
  auto y = state->CheckNumber(1);
  auto x = state->OptNumber(2, 1.0);
  state->PushNumber(std::atan2(y, x));
  return 1;
}

namespace {
void PushNumInt(const std::shared_ptr<api::LuaState> &state, double d) {
  auto[i, ok]= utils::FloatToInteger(d);
  if (ok)
    state->PushInteger(i);
  else
    state->PushNumber(d);
}
}

int32_t Ceil(const std::shared_ptr<api::LuaState> &state) {
  if (state->IsInteger(1))
    state->SetTop(1);
  else {
    auto x = state->CheckNumber(1);
    PushNumInt(state, std::ceil(x));
  }
  return 1;
}

int32_t Floor(const std::shared_ptr<api::LuaState> &state) {
  if (state->IsInteger(1))
    state->SetTop(1);
  else {
    auto x = state->CheckNumber(1);
    PushNumInt(state, std::floor(x));
  }
  return 1;
}

int32_t FMod(const std::shared_ptr<api::LuaState> &state) {
  if (state->IsInteger(1) && state->IsInteger(2)) {
    auto d = state->ToInteger(2);
    if (uint64_t(d) + 1 <= 1) {
      state->ArgCheck(d != 0, 2, "zero");
      state->PushInteger(0);
    } else
      state->PushInteger(state->ToInteger(1) % d);
  } else {
    auto x = state->CheckNumber(1);
    auto y = state->CheckNumber(2);
    state->PushNumber(x - std::trunc(x / y) * y);
  }
  return 1;
}

int32_t ModF(const std::shared_ptr<api::LuaState> &state) {
  if (state->IsInteger(1)) {
    state->SetTop(1);
    state->PushNumber(0);
  } else {
    auto x = state->CheckNumber(1);
    auto[i, f]= utils::ModF(x);
    PushNumInt(state, i);
    if (std::isinf(x))
      state->PushNumber(0);
    else
      state->PushNumber(f);
  }
  return 2;
}

int32_t Abs(const std::shared_ptr<api::LuaState> &state) {
  if (state->IsInteger(1)) {
    auto x = state->ToInteger(1);
    if (x < 0)
      state->PushInteger(-x);
  } else {
    auto x = state->CheckNumber(1);
    state->PushNumber(std::abs(x));
  }
  return 1;
}

int32_t Sqrt(const std::shared_ptr<api::LuaState> &state) {
  auto x = state->CheckNumber(1);
  state->PushNumber(std::sqrt(x));
  return 1;
}

int32_t Ult(const std::shared_ptr<api::LuaState> &state) {
  auto m = state->CheckInteger(1);
  auto n = state->CheckInteger(2);
  state->PushBoolean(uint64_t(m) < uint64_t(n));
  return 1;
}

int32_t ToInteger(const std::shared_ptr<api::LuaState> &state) {
  auto[i, ok]=state->ToIntegerX(1);
  if (ok)
    state->PushInteger(i);
  else {
    state->CheckAny(1);
    state->PushNil();
  }
  return 1;
}

int32_t Type(const std::shared_ptr<api::LuaState> &state) {
  if (state->Type(1) == api::LuaValueType::kNumber) {
    if (state->IsInteger(1))
      state->PushString("integer");
    else
      state->PushString("float");
  } else {
    state->CheckAny(1);
    state->PushNil();
  }
  return 1;
}
}
}
