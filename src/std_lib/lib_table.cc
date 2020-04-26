//
// Created by 方泓睿 on 2020/4/21.
//

#include <vector>

#include <embryo/std_lib/lib_math.h>
#include <embryo/std_lib/lib_table.h>
#include <embryo/utils/strings.h>
#include <embryo/utils/sort.h>

#include <fmt/format.h>

namespace embryo::std_lib {
api::FuncReg kTableLib{ // NOLINT(cert-err58-cpp)
    {"move", table::Move},
    {"insert", table::Insert},
    {"remove", table::Remove},
    {"sort", table::Sort},
    {"concat", table::Concat},
    {"pack", table::Pack},
    {"unpack", table::Unpack}
};

int32_t OpenTableLib(const std::shared_ptr<api::LuaState> &state) {
  state->NewLib(kTableLib);
  return 1;
}

namespace table {
namespace {
bool CheckField(const std::shared_ptr<api::LuaState> &state, const std::string &key, int32_t &n) {
  state->PushString(key);
  ++n;
  return state->RawGet(-n) != api::LuaValueType::kNil;
}

void CheckTab(const std::shared_ptr<api::LuaState> &state, int32_t arg, TableOp what) {
  if (state->Type(arg) != api::LuaValueType::kTable) {
    int32_t n = 1;
    if (state->GetMetatable(arg) &&
        ((static_cast<int32_t>(what) & static_cast<int32_t>(TableOp::kR)) != 0// NOLINT(hicpp-signed-bitwise)
            || CheckField(state, "__index", n)) &&
        ((static_cast<int32_t>(what) & static_cast<int32_t>(TableOp::kW)) != 0 // NOLINT(hicpp-signed-bitwise)
            || CheckField(state, "__newindex", n)) &&
        ((static_cast<int32_t>(what) & static_cast<int32_t>(TableOp::kL)) != 0 // NOLINT(hicpp-signed-bitwise)
            || CheckField(state, "__len", n)))
      state->Pop(n);
    else
      state->CheckType(arg, api::LuaValueType::kTable);
  }
}

int64_t AuxGetN(const std::shared_ptr<api::LuaState> &state, int32_t n, TableOp w) {
  CheckTab(state,
           n,
           static_cast<TableOp>(static_cast<int32_t>(w) // NOLINT(hicpp-signed-bitwise)
               | static_cast<int32_t>(TableOp::kL))); // NOLINT(hicpp-signed-bitwise)
  return state->Len2(n);
}
}

int32_t Move(const std::shared_ptr<api::LuaState> &state) {
  auto f = state->CheckInteger(2);
  auto e = state->CheckInteger(3);
  auto t = state->CheckInteger(4);
  int32_t tt = 1;

  if (!state->IsNoneOrNil(5))
    tt = 5;

  CheckTab(state, 1, TableOp::kR);
  CheckTab(state, tt, TableOp::kW);

  if (e >= f) {
    int64_t n{}, i{};
    state->ArgCheck(f > 0 || e < math::kMaxInteger + f, 3, "too many elements to move");
    n = e - f + 1;
    state->ArgCheck(t <= math::kMaxInteger - n + 1, 4, "destination wrap around");
    if (t > e || t <= f || (tt != 1 && !state->Compare(1, tt, api::CompareOp::kEQ))) {
      for (i = 0; i < n; i++) {
        state->GetI(1, f + i);
        state->SetI(tt, t + i);
      }
    } else {
      for (i = n - 1; i >= 0; --i) {
        state->GetI(1, f + 1);
        state->SetI(tt, t + i);
      }
    }
  }
  state->PushValue(tt);
  return 1;
}

int32_t Insert(const std::shared_ptr<api::LuaState> &state) {
  auto e = AuxGetN(state, 1, TableOp::kRW) + 1;
  int64_t pos{};
  switch (state->GetTop()) {
    case 2:pos = e;
      break;
    case 3:pos = state->CheckInteger(2);
      state->ArgCheck(1 <= pos && pos <= e, 2, "position out of bound");
      for (auto i = e; i > pos; --i) {
        state->GetI(1, i - 1);
        state->SetI(1, i);
      }
      break;
    default:return state->Error2("wrong number of arguments to 'insert'");
  }
  state->SetI(1, pos);
  return 0;
}

int32_t Remove(const std::shared_ptr<api::LuaState> &state) {
  auto size = AuxGetN(state, 1, TableOp::kRW);
  auto pos = state->OptInteger(2, size);
  if (pos != size)
    state->ArgCheck(1 <= pos && pos <= size + 1, 1, "position out of bounds");
  state->GetI(1, pos);
  for (; pos < size; ++pos) {
    state->GetI(1, pos + 1);
    state->SetI(1, pos);
  }
  state->PushNil();
  state->SetI(1, pos);
  return 1;
}

class TableSortWrapper : public utils::Sortable {
 private:
  const std::shared_ptr<api::LuaState> &state_;

 public:
  explicit TableSortWrapper(const std::shared_ptr<api::LuaState> &state) : state_(state) {}

  [[nodiscard]] size_t Len() const override {
    return state_->Len2(1);
  }

  [[nodiscard]] bool Less(size_t i, size_t j) const override {
    if (state_->IsFunction(2)) {
      state_->PushValue(2);
      state_->GetI(1, i + 1);
      state_->GetI(1, j + 1);
      state_->Call(2, 1);
      auto b = state_->ToBoolean(-1);
      state_->Pop(1);
      return b;
    }
    state_->GetI(1, i + 1);
    state_->GetI(1, j + 1);
    auto b = state_->Compare(-2, -1, api::CompareOp::kLT);
    state_->Pop(2);
    return b;
  }

  void Swap(size_t i, size_t j) override {
    state_->GetI(1, i + 1);
    state_->GetI(1, j + 1);
    state_->SetI(1, i + 1);
    state_->SetI(1, j + 1);
  }

  ~TableSortWrapper() override = default;
};

int32_t Sort(const std::shared_ptr<api::LuaState> &state) {
  auto wrapper = std::make_shared<TableSortWrapper>(state);
  state->ArgCheck(wrapper->Len() < kMaxLen, 1, "array too big");
  utils::Sort(wrapper);
  return 0;
}

int32_t Concat(const std::shared_ptr<api::LuaState> &state) {
  auto tab_len = AuxGetN(state, 1, TableOp::kR);
  auto sep = state->OptString(2, "");
  auto i = state->OptInteger(3, 1);
  auto j = state->OptInteger(4, tab_len);

  if (i > j) {
    state->PushString("");
    return 1;
  }

  std::vector<std::string> buf(j - i + 1);
  for (auto k = i; k > 0 && k <= j; ++k) {
    state->GetI(1, k);
    if (!state->IsString(-1))
      state->Error2(fmt::format("invalid value({}) at index {} in table for 'concat'",
                                state->TypeName2(-1), i));
    buf[k - i] = state->ToString(-1);
    state->Pop(1);
  }
  state->PushString(utils::Join(buf, sep));
  return 1;
}

int32_t Pack(const std::shared_ptr<api::LuaState> &state) {
  int64_t n = state->GetTop();
  state->CreateTable(n, 1);
  state->Insert(1);
  for (auto i = n; i >= 1; --i)
    state->SetI(1, i);
  state->PushInteger(n);
  state->SetField(1, "n");
  return 1;
}

int32_t Unpack(const std::shared_ptr<api::LuaState> &state) {
  auto i = state->OptInteger(2, 1);
  auto e = state->OptInteger(3, state->Len2(1));
  if (i > e)
    return 0;

  auto n = e - i + 1;
  if (n <= 0 || n >= kMaxLen || !state->CheckStack(n))
    return state->Error2("too many results to unpack");

  for (; i < e; ++i)
    state->GetI(1, i);

  state->GetI(1, e);
  return n;
}
}
}