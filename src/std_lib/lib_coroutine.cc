//
// Created by 方泓睿 on 2020/4/24.
//

#include <embryo/utils/inspection.h>
#include <embryo/std_lib/lib_coroutine.h>

namespace embryo::std_lib {
api::FuncReg kCoroutineLib{ // NOLINT(cert-err58-cpp)
    {"create", coroutine::Create},
    {"resume", coroutine::Resume},
    {"status", coroutine::Status},
    {"yield", coroutine::Yield},
    {"isyieldable", coroutine::IsYieldable},
    {"running", coroutine::Running},
    {"wrap", coroutine::Wrap}
};

int32_t OpenCoroutineLib(const std::shared_ptr<api::LuaState> &state) {
  state->NewLib(kCoroutineLib);
  return 1;
}

namespace coroutine {
namespace {
int32_t AuxResume(const std::shared_ptr<api::LuaState> &state,
                  const std::shared_ptr<api::LuaState> &co,
                  int32_t n_args) {
  if (!state->CheckStack(n_args)) {
    state->PushString("too many arguments to resume");
    return -1;
  }
  if (co->Status() == api::Status::kOk && co->GetTop() == 0) {
    state->PushString("cannot resume dead coroutine");
    return -1;
  }
  state->XMove(co, n_args);
  auto status = co->Resume(state, n_args);
  if (status == api::Status::kOk || status == api::Status::kYield) {
    auto n_res = co->GetTop();
    if (!state->CheckStack(n_res + 1)) {
      co->Pop(n_res);
      state->PushString("too many results to resume");
      return -1;
    }
    co->XMove(state, n_res);
    return n_res;
  }
  co->XMove(state, 1);
  return -1;
}
}

int32_t Create(const std::shared_ptr<api::LuaState> &state) {
  state->CheckType(1, api::LuaValueType::kFunction);
  auto state_2 = state->NewThread();
  state->PushValue(1);
  state->XMove(state_2, 1);
  return 1;
}

int32_t Resume(const std::shared_ptr<api::LuaState> &state) {
  auto co = state->ToThread(1);
  state->ArgCheck(bool(co), 1, "thread expected");
  auto r = AuxResume(state, co, state->GetTop() - 1);
  if (r < 0) {
    state->PushBoolean(false);
    state->Insert(-2);
    return 2;
  }
  state->PushBoolean(true);
  state->Insert(-(r + 1));
  return r + 1;
}

int32_t Status(const std::shared_ptr<api::LuaState> &state) {
  auto co = state->ToThread(1);
  state->ArgCheck(bool(co), 1, "thread expected");
  if (state == co)
    state->PushString("running");
  else {
    switch (co->Status()) {
      case api::Status::kYield:state->PushString("suspended");
        break;
      case api::Status::kOk:
        if (co->GetStack()) {
          state->PushString("normal");
        } else if (co->GetTop() == 0)
          state->PushString("dead");
        else
          state->PushString("suspended");
        break;
      default:state->PushString("dead");
    }
  }
  return 1;
}

int32_t Yield(const std::shared_ptr<api::LuaState> &state) {
  return state->Yield(state->GetTop());
}

int32_t IsYieldable(const std::shared_ptr<api::LuaState> &state) {
  state->PushBoolean(state->IsYieldable());
  return 1;
}

int32_t Running(const std::shared_ptr<api::LuaState> &state) {
  auto is_main = state->PushThread();
  state->PushBoolean(is_main);
  return 2;
}

int32_t Wrap(const std::shared_ptr<api::LuaState> &) {
  THROW_NOT_IMPLEMENTED;
}
}
}
