//
// Created by 方泓睿 on 2020/4/10.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_BIN_CHUNK_CLOSURE_H_
#define EMBRYO_INCLUDE_EMBRYO_BIN_CHUNK_CLOSURE_H_

#include <memory>

#include <embryo/api/lua_state.h>
#include <embryo/bin_chunk/bin_chunk.h>
#include <embryo/state/lua_value.h>

namespace embryo::bin_chunk {
class Closure : public state::LuaAnyType,
                public std::enable_shared_from_this<Closure> {
 public:
  std::shared_ptr<Prototype> proto_{};
  api::NativeFunction native_function_{};
  std::vector<std::shared_ptr<state::UpValue>> up_values{};
 public:
  explicit Closure(std::shared_ptr<Prototype> proto);

  explicit Closure(api::NativeFunction f, size_t n_up_vals);
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_BIN_CHUNK_CLOSURE_H_
