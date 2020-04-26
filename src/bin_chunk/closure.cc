//
// Created by 方泓睿 on 2020/4/10.
//

#include <embryo/bin_chunk/closure.h>

namespace embryo::bin_chunk {
Closure::Closure(std::shared_ptr<Prototype> proto) : proto_(std::move(proto)) {
  auto n_up_vals = proto_->up_values.size();
  if (n_up_vals > 0)
    up_values.resize(n_up_vals);
}

Closure::Closure(api::NativeFunction f, size_t n_up_vals) : native_function_(std::move(f)) {
  if (n_up_vals > 0)
    up_values.resize(n_up_vals);
}
}