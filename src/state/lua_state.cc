//
// Created by 方泓睿 on 2020/4/8.
//

#include <utility>
#include <vector>
#include <stdexcept>

#include <embryo/utils/formatters.h>
#include <embryo/utils/inspection.h>
#include <embryo/state/lua_state.h>
#include <embryo/vm/opcodes.h>
#include <embryo/state/lua_table.h>
USING_EMBRYO_FORMATTERS();

namespace embryo::state {

LuaState::LuaState(size_t init_stack_size) :
    stack_(std::make_shared<LuaStack>(init_stack_size, this)),
    registry_(std::make_shared<LuaTable>(0, 0)) {
  auto w_ptr = std::shared_ptr<LuaState>(this, [](LuaState *) {});
  (void) (w_ptr);

  registry_->Put(kLuaRIdxMainThread, std::dynamic_pointer_cast<LuaAnyType>(shared_from_this()));
  registry_->Put(kLuaRIdxGlobals,
                 std::dynamic_pointer_cast<LuaAnyType>(
                     std::make_shared<LuaTable>(0, 20)));
}

void LuaState::Destroy() {
  registry_ = nullptr;
  stack_ = nullptr;
  co_caller_ = nullptr;
}

bool LuaState::IsMainThread() const {
  return Cast<LuaAnyTypePtr>(registry_->Get(kLuaRIdxMainThread)).get() == this;
}

void LuaState::Error(const std::string &msg) const {
  throw std::runtime_error(fmt::format("{}: {}", THIS_TYPE, msg));
}

std::string LuaState::String() const {
  auto inspector = INSPECT_THIS;

  auto top = GetTop();
  std::vector<std::string> stack_vals{};

  for (int64_t i = 1; i <= top; i++) {
    switch (Type(i)) {
      case api::LuaValueType::kBoolean: {
        stack_vals.push_back(fmt::format("{}", ToBoolean(i)));
        break;
      }
      case api::LuaValueType::kNumber: {
        stack_vals.push_back(fmt::format("{}", ToNumber(i)));
        break;
      }
      case api::LuaValueType::kString: {
        stack_vals.push_back(fmt::format("{}",
                                         AS_STRING(Cast<std::string>(stack_->Get(i)))));
        break;
      }
      case api::LuaValueType::kNil: {
        stack_vals.emplace_back("null");
        break;
      }
      case api::LuaValueType::kTable: {
        stack_vals.push_back(Cast<LuaTablePtr>(stack_->Get(i))->String());
        break;
      }
      default:stack_vals.emplace_back(R"("<unknown>")");
    }
  }

  inspector.Add("stack_", stack_vals);

  return inspector.Result();
}

std::shared_ptr<api::LuaVM> LuaState::New(
    size_t init_stack_size) {
  auto state = std::make_shared<LuaState>(init_stack_size);
  state->OpenLibs();
  return state;
}

void LuaState::PushLuaStack(const std::shared_ptr<LuaStack> &stack) {
  stack->prev_ = stack_;
  stack_ = stack;
}

void LuaState::PopLuaStack() {
  auto stack = stack_;
  stack_ = stack->prev_;
  stack->prev_ = nullptr;
}

int64_t LuaUpValueIndex(int64_t i) {
  return kLuaRegistryIndex - i;
}
}