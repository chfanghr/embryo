//
// Created by 方泓睿 on 2020/4/22.
//

#include <thread>

#include <embryo/state/lua_state.h>

namespace embryo::state {
std::shared_ptr<api::LuaState> LuaState::NewThread() {
  auto t = std::make_shared<LuaState>();
  t->registry_ = registry_;
  t->PushLuaStack(std::make_shared<LuaStack>(kLuaMinStack, t.get()));
  stack_->Push(std::dynamic_pointer_cast<LuaAnyType>(t));
  return t;
}

api::Status LuaState::Resume(const std::shared_ptr<api::LuaState> &from, int32_t n_args) {
  auto state_from = std::dynamic_pointer_cast<LuaState>(from);
  if (!state_from->co_chan_)
    state_from->co_chan_.Initialize();
  if (!co_chan_) {
    co_chan_.Initialize();
    co_caller_ = state_from;
    std::thread([&]() {
      this->co_status_ = this->PCall(n_args, -1, 0);
      state_from->co_chan_.Send(1);
    }).detach();
  } else {
    if (co_status_ != api::Status::kYield) {
      stack_->Push("cannot resume non-suspended coroutine");
      return api::Status::kErrRun;
    }
    co_status_ = api::Status::kOk;
    state_from->co_chan_.Send(1);
  }
  co_chan_.Receive();
  return co_status_;
}

int64_t LuaState::Yield(int32_t n_results) {
  if (co_caller_ == nullptr)
    Error("attempt to yield from outside a coroutine");
  co_status_ = api::Status::kYield;
  co_caller_->co_chan_.Send(1);
  co_chan_.Receive();
  return GetTop();
}

api::Status LuaState::Status() {
  return co_status_;
}

bool LuaState::GetStack() {
  return stack_->prev_ != nullptr;
}

bool LuaState::IsYieldable() {
  if (IsMainThread())
    return false;
  return co_status_ != api::Status::kYield;
}
}