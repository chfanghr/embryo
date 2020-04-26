//
// Created by 方泓睿 on 2020/4/8.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_STATE_LUA_STATE_H_
#define EMBRYO_INCLUDE_EMBRYO_STATE_LUA_STATE_H_

#include <embryo/bin_chunk/bin_chunk.h>
#include <embryo/api/lua_vm.h>
#include <embryo/state/lua_stack.h>
#include <embryo/vm/instruction.h>
#include <embryo/utils/chan.h>

namespace embryo::state {
using api::kMultert;

using api::kLuaRIdxGlobals;
using api::kLuaMinStack;
using api::kLuaRegistryIndex;
using api::kLuaIMaxStack;
using api::kLuaRIdxMainThread;

class LuaState :
    public api::LuaVM,
    public utils::Stringer,
    public std::enable_shared_from_this<LuaState>,
    public LuaAnyType {
 private:
  utils::Chan<int32_t> co_chan_ = nullptr;
  api::Status co_status_{};
  std::shared_ptr<LuaState> co_caller_;
 private:
  std::shared_ptr<LuaStack> stack_;

  LuaTablePtr registry_;

  friend class LuaStack;
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  friend std::shared_ptr<LuaTable> GetMetaTable(const LuaValue &val, const std::shared_ptr<LuaState> &state);

  friend void SetMetaTable(const LuaValue &val,
                           const std::shared_ptr<LuaTable> &mt,
                           const std::shared_ptr<LuaState> &state);

  friend LuaValue CallMetaMethod(const LuaValue &a,
                                 const LuaValue &b,
                                 const std::string &method,
                                 const std::shared_ptr<LuaState> &state);

  friend LuaValue GetMetaField(const LuaValue &val,
                               const std::string &field_name,
                               const std::shared_ptr<LuaState> &state);
#pragma clang diagnostic pop
 public:
  static std::shared_ptr<api::LuaVM> New(size_t init_stack_size = kLuaMinStack);

  LuaState() = default;

  explicit LuaState(size_t init_stack_size);

  ~LuaState() override = default;

  [[nodiscard]] int64_t GetTop() const override;

  [[nodiscard]] int64_t AbsIndex(int64_t idx) const override;

  bool CheckStack(size_t n) override;

  void Pop(size_t n) override;

  void Copy(int64_t from, int64_t to) override;

  void PushValue(int64_t idx) override;

  void Replace(int64_t idx) override;

  void Insert(int64_t idx) override;

  void Remove(int64_t idx) override;

  void Rotate(int64_t idx, size_t n) override;

  void SetTop(int64_t idx) override;

  [[nodiscard]] std::string TypeName(api::LuaValueType type) const override;

  [[nodiscard]] api::LuaValueType Type(int64_t idx) const override;

  [[nodiscard]] bool IsNone(int64_t idx) const override;

  [[nodiscard]] bool IsNil(int64_t idx) const override;

  [[nodiscard]] bool IsNoneOrNil(int64_t idx) const override;

  [[nodiscard]] bool IsBoolean(int64_t idx) const override;

  [[nodiscard]] bool IsInteger(int64_t idx) const override;

  [[nodiscard]] bool IsNumber(int64_t idx) const override;

  [[nodiscard]] bool IsString(int64_t idx) const override;

  [[nodiscard]] bool IsFunction(int64_t idx) const override;

  [[nodiscard]] bool ToBoolean(int64_t idx) const override;

  [[nodiscard]] LuaInteger ToInteger(int64_t idx) const override;

  [[nodiscard]] std::tuple<LuaInteger, bool> ToIntegerX(int64_t idx) const override;

  [[nodiscard]] LuaNumber ToNumber(int64_t idx) const override;

  [[nodiscard]] std::tuple<LuaNumber, bool> ToNumberX(int64_t idx) const override;

  std::string ToString(int64_t idx) override;

  std::tuple<std::string, bool> ToStringX(int64_t idx) override;

  void PushNil() override;

  void PushBoolean(bool b) override;

  void PushInteger(LuaInteger integer) override;

  void PushNumber(LuaNumber number) override;

  void PushString(std::string string) override;

  void Arith(api::ArithOp op) override;

  bool Compare(int64_t idx_1, int64_t idx_2, api::CompareOp op) override;

  void Len(int64_t idx) override;

  void Concat(size_t n) override;
 private:
  [[noreturn]] void Error(const std::string &msg) const;

 public:
  [[nodiscard]] std::string String() const override;

  int32_t PC() const override;

  void AddPC(int32_t int_32) override;

  uint32_t Fetch() override;

  void GetConst(int32_t idx) override;

  void GetRK(int32_t rk) override;

  void NewTable() override;

  void CreateTable(int64_t n_arr, int64_t n_rec) override;

  api::LuaValueType GetTable(int64_t idx) override;

  api::LuaValueType GetField(int64_t idx, std::string k) override;

  api::LuaValueType GetI(int64_t idx, int64_t i) override;

  void SetTable(int64_t idx) override;

  void SetField(int64_t idx, std::string k) override;

  void SetI(int64_t idx, int64_t i) override;

 private:
  api::LuaValueType GetTable(const embryo::state::LuaValue &t, const embryo::state::LuaValue &k, bool raw);

  void SetTable(const embryo::state::LuaValue &t,
                const embryo::state::LuaValue &k,
                const embryo::state::LuaValue &v,
                bool raw);

 private:
  void PushLuaStack(const std::shared_ptr<LuaStack> &);

  void PopLuaStack();

 public:
  api::Status Load(const std::vector<Byte> &chunk, std::string chunk_name, std::string mode) override;

  api::Status Load(const std::string &file) override;

 private:
  api::Status Load(const std::shared_ptr<bin_chunk::Prototype> &proto);

  void Call(int32_t n_args, int32_t n_results) override;

 private:
  void CallLuaClosure(int32_t n_args, int32_t n_results, const LuaClosurePtr &c);

  void RunLuaClosure();
 public:
  int32_t RegisterCount() const override;

  void LoadVarArg(int32_t n) override;

  void LoadProto(int32_t idx) override;

 public:
  void PushNativeFunction(NativeFunction f) override;

  bool IsNativeFunction(int32_t idx) override;

  NativeFunction ToNativeFunction(int32_t idx) override;
 private:
  void CallNativeClosure(int32_t n_args, int32_t n_results, const LuaClosurePtr &c);

 public:
  void PushGlobalTable() override;

  api::LuaValueType GetGlobal(const std::string &name) override;

  void SetGlobal(const std::string &name) override;

  void Register(const std::string &name, NativeFunction f) override;

  void PushNativeClosure(NativeFunction f, int32_t n) override;

 public:
  void CloseUpValues(int32_t a) override;

  bool GetMetatable(int64_t idx) override;

  void SetMetatable(int64_t idx) override;

  size_t RawLen(int64_t idx) override;

  bool RawEqual(int64_t idx_1, int64_t idx_2) override;

  api::LuaValueType RawGet(int64_t idx) override;

  void RawSet(int64_t idx) override;

  api::LuaValueType RawGetI(int64_t idx, int64_t i) override;

  void RawSetI(int64_t idx, int64_t i) override;

 private:
  void RegisterInternal(const std::string &name, NativeFunction f);

 public:
  bool Next(int64_t idx) override;

 public:
  int32_t Error() override;

  api::Status PCall(int32_t n_args, int32_t n_results, int32_t msg) override;

 public:
  int32_t Error2(const std::string &msg) override;

  int32_t ArgError(int32_t arg, const std::string &extra_message) override;

  void CheckStack2(int32_t sz, const std::string &msg) override;

  void ArgCheck(bool cond, int32_t arg, const std::string &extra_msg) override;

  void CheckAny(int32_t arg) override;

  void CheckType(int32_t arg, api::LuaValueType type) override;

  int64_t CheckInteger(int32_t arg) override;

  double CheckNumber(int32_t arg) override;

  std::string CheckString(int32_t arg) override;

  int64_t OptInteger(int32_t arg, int64_t d) override;

  double OptNumber(int32_t arg, double d) override;

  std::string OptString(int32_t arg, const std::string &d) override;

  bool DoFile(const std::string &file) override;

  bool DoString(const std::string &str) override;

  api::Status LoadFile(const std::string &file) override;

  api::Status LoadFileX(const std::string &file, const std::string &mode) override;

  api::Status LoadString(const std::string &s) override;

  std::string TypeName2(int64_t idx) override;

  std::string ToString2(int64_t idx) override;

  int64_t Len2(int64_t idx) override;

  bool GetSubTable(int64_t idx, const std::string &f_name) override;

  api::LuaValueType GetMetaField(int32_t obj, const std::string &e) override;

  bool CallMeta(int32_t obj, const std::string &e) override;

  void OpenLibs() override;

  void RequireF(const std::string &mod_name, const NativeFunction &open_f, bool glb) override;

  void NewLib(const api::FuncReg &reg) override;

  void NewLibTable(const api::FuncReg &reg) override;

  void SetFuncs(const api::FuncReg &reg, int32_t n_up) override;

 private:
  void TagError(int32_t arg, api::LuaValueType tag);

  int32_t TypeError(int32_t arg, const std::string &t_name);

  void IntError(int32_t arg);

 public:
  void *ToPointer(int64_t idx) override;

  bool StringToNumber(const std::string &s) override;

 public:
  std::shared_ptr<api::LuaState> NewThread() override;

  api::Status Resume(const std::shared_ptr<api::LuaState> &from, int32_t n_args) override;

  int64_t Yield(int32_t n_results) override;

  api::Status Status() override;

  bool IsYieldable() override;

  std::shared_ptr<api::LuaState> ToThread(int64_t idx) override;

  bool PushThread() override;

  void XMove(const std::shared_ptr<api::LuaState> &to, size_t n) override;

  bool GetStack() override;

  void Destroy() override;

 private:
  bool IsMainThread() const;
};

int64_t LuaUpValueIndex(int64_t i);
}

#endif //EMBRYO_INCLUDE_EMBRYO_STATE_LUA_STATE_H_
