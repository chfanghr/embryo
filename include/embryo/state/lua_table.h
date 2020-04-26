//
// Created by 方泓睿 on 2020/4/10.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_STATE_LUA_TABLE_H_
#define EMBRYO_INCLUDE_EMBRYO_STATE_LUA_TABLE_H_

#include <unordered_map>
#include <vector>

#include <embryo/state/lua_value.h>
#include <embryo/utils/stringer.h>

namespace embryo::state {
class LuaTable : public LuaAnyType, public utils::Stringer {
 private:
  std::vector<LuaValue> arr_{};
  std::unordered_map<LuaValue, LuaValue> map_{};

  std::shared_ptr<LuaTable> meta_table_{};

  friend std::shared_ptr<LuaTable> GetMetaTable(const LuaValue &val, const std::shared_ptr<LuaState> &state);
  friend void SetMetaTable(const LuaValue &val,
                           const std::shared_ptr<LuaTable> &mt,
                           const std::shared_ptr<LuaState> &state);

  std::unordered_map<LuaValue, LuaValue> keys_{};

  bool changed_{};
 public:
  explicit LuaTable(int64_t n_arr = 0, int64_t n_rec = 0);

  [[nodiscard]] LuaValue Get(const LuaValue &) const;

  void Put(const LuaValue &, const LuaValue &);

  LuaValue operator[](const LuaValue &) const;

  size_t Len() const;

 private:
  [[noreturn]] void Error(const std::string &msg) const;

  void ShrinkArray();

  void ExpandArray();

 public:
  [[nodiscard]] std::string String() const override;

  [[nodiscard]] bool HasMetaField(const std::string &field) const;

  LuaValue NextKey(const LuaValue &key);

 private:
  void InitKeys(LuaValue key);
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_STATE_LUA_TABLE_H_
