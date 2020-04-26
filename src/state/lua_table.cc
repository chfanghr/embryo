//
// Created by 方泓睿 on 2020/4/10.
//

#include <embryo/utils/inspection.h>
#include <embryo/state/lua_table.h>
#include <embryo/utils/formatters.h>
USING_EMBRYO_FORMATTERS();

namespace embryo::state {
namespace {
LuaValue FloatToInteger(const LuaValue &val) {
  auto[d, ok_d]=TryCast<double>(val);
  if (ok_d)
    return {int64_t(d)};

  return val;
}
}

LuaTable::LuaTable(int64_t n_arr, int64_t n_rec) {
  if (n_arr > 0)
    arr_.resize(n_arr);
  if (n_rec > 0)
    map_.reserve(n_arr);
}

LuaValue LuaTable::Get(const LuaValue &k) const {
  auto key = FloatToInteger(k);
  auto[idx, ok] = TryCast<int64_t>(key);
  if (ok)
    if (idx >= 1 && idx <= int64_t(arr_.size()))
      return arr_[idx - 1];
  auto got = map_.find(key);
  return got != map_.end() ? got->second : LuaValue{};
}

void LuaTable::Put(const LuaValue &k, const LuaValue &v) {
  if (!k || ContainsType<std::nullptr_t>(k))
    Error(MSG_WITH_FUNC_NAME(
              "using nil or none as table key is not allowed"));
  if (IsNan(k))
    Error(MSG_WITH_FUNC_NAME(
              "using NAN as table key is not allowed"));
  if (IsNone(v))
    Error(MSG_WITH_FUNC_NAME(
              "using none as table value is not allowed"));

  auto key = FloatToInteger(k);

  auto[idx, ok_i]= TryCast<int64_t>(key);
  if (ok_i && idx >= 1) {
    auto arr_len = arr_.size();
    if (idx <= arr_len) {
      arr_[idx - 1] = v;
      if (idx == arr_len && IsNil(v))
        ShrinkArray();
      return;
    }
    if (idx == arr_len + 1) {
      map_.erase(key);
      if (!IsNil(v)) {
        arr_.push_back(v);
        ExpandArray();
      }
      return;
    }
  }

  if (!IsNil(v))
    map_[key] = v;
  else
    map_.erase(key);
}

LuaValue LuaTable::operator[](const LuaValue &k) const {
  return Get(k);
}

void LuaTable::ShrinkArray() {
  for (int64_t i = arr_.size() - 1; i >= 0; i--) {
    if (IsNil(arr_[i]) || IsNone(arr_[i]))
      arr_.pop_back();
  }
}

void LuaTable::ExpandArray() {
  for (int64_t idx = arr_.size() + 1;; idx++) {
    auto got = map_.find(idx);
    if (got != map_.end()) {
      auto val = got->second;
      map_.erase(got);
      arr_.push_back(val);
    } else break;
  }
}

void LuaTable::Error(const std::string &msg) const {
  throw std::runtime_error(fmt::format("{}: {}", THIS_TYPE, msg));
}

size_t LuaTable::Len() const {
  return arr_.size();
}

std::string LuaTable::String() const {
  auto ins = INSPECT_THIS;
  auto map_ins =
      utils::InspectionMessageBuilder("std::unordered_map<LuaValue, LuaValue>");
  for (const auto &[k, v]:map_)
    map_ins.AddNoQuote(Stringify(k), Stringify(v));

  ins.Add("map_", map_ins.Result());

  std::vector<std::string> arr_str{};
  for (const auto &v:arr_)
    arr_str.push_back(Stringify(v));

  ins.Add("arr_", arr_str);

  return ins.Result();
}

bool LuaTable::HasMetaField(const std::string &field) const {
  return meta_table_ && meta_table_->Get(field);
}

LuaValue LuaTable::NextKey(const LuaValue &key) {
  if (keys_.empty() || !key || IsNil(key)) {
    InitKeys(key);
    changed_ = false;
  }
  auto iter = keys_.find(key);
  if (iter != keys_.end())
    return iter->second;
  return {};
}

void LuaTable::InitKeys(LuaValue key) {
  keys_ = {};

  for (size_t i = 0; i < arr_.size(); i++) {
    auto v = arr_[i];
    if (v) {
      keys_[key] = int64_t(i + 1);
      key = int64_t(i + 1);
    }
  }

  for (const auto &[k, v]:map_) {
    if (v) {
      keys_[key] = k;
      key = k;
    }
  }
}
}
