//
// Created by 方泓睿 on 2020/4/8.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_STATE_LUA_STACK_H_
#define EMBRYO_INCLUDE_EMBRYO_STATE_LUA_STACK_H_

#include <memory>
#include <vector>
#include <map>

#include <embryo/utils/stringer.h>
#include <embryo/state/lua_value.h>
#include <embryo/bin_chunk/closure.h>

namespace embryo::state {
class LuaState;

class LuaStack :
    public std::enable_shared_from_this<LuaStack> {
 public:
  std::vector<LuaValue> slots_;
  int64_t top_;

  std::shared_ptr<LuaStack> prev_{};
  std::shared_ptr<LuaClosure> closure_{};
  std::vector<LuaValue> var_args_{};

  int32_t pc_{0};

  LuaState *state_;

  std::map<int32_t, std::shared_ptr<UpValue>> open_uvs{};
 public:
  explicit LuaStack(size_t init_size, LuaState *lua_state);

  void Check(size_t n);

  void Push(LuaValue);

  LuaValue Pop();

  [[nodiscard]] int64_t AbsIndex(int64_t idx) const noexcept;

  [[nodiscard]] bool IsValidIndex(int64_t idx) const;

  [[nodiscard]] LuaValue Get(int64_t idx) const;

  void *GetPointer(int64_t idx);

  void Set(int64_t idx, LuaValue value);

  void Reverse(int64_t from, int64_t to);

  std::vector<LuaValue> PopN(size_t n);

  void PushN(const std::vector<LuaValue> &vals, int64_t n);
 private:
  void ValidateIndex(int64_t idx) const;

  [[noreturn]] void Error(const std::string &msg) const;
};

}
#endif //EMBRYO_INCLUDE_EMBRYO_STATE_LUA_STACK_H_
