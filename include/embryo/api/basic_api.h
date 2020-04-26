//
// Created by 方泓睿 on 2020/4/19.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_API_BASIC_API_H_
#define EMBRYO_INCLUDE_EMBRYO_API_BASIC_API_H_

#include <cstdlib>
#include <string>
#include <tuple>
#include <functional>
#include <memory>
#include <map>

#include <embryo/api/constants.h>
#include <embryo/utils/types.h>

namespace embryo::api {
class LuaState;

using NativeFunction=std::function<int32_t(std::shared_ptr<LuaState>)>;
using FuncReg= std::map<std::string, NativeFunction>;

class BasicApi {
 public:
  [[nodiscard]] virtual int64_t GetTop() const = 0;
  [[nodiscard]] virtual int64_t AbsIndex(int64_t idx) const = 0;
  virtual bool CheckStack(size_t n) = 0;
  virtual void Pop(size_t n) = 0;
  virtual void Copy(int64_t from, int64_t to) = 0;
  virtual void PushValue(int64_t idx) = 0;
  virtual void Replace(int64_t idx) = 0;
  virtual void Insert(int64_t idx) = 0;
  virtual void Remove(int64_t idx) = 0;
  virtual void Rotate(int64_t idx, size_t n) = 0;
  virtual void SetTop(int64_t idx) = 0;

  [[nodiscard]] virtual std::string TypeName(LuaValueType type) const = 0;
  [[nodiscard]] virtual LuaValueType Type(int64_t idx) const = 0;

  [[nodiscard]] virtual bool IsNone(int64_t idx) const = 0;
  [[nodiscard]] virtual bool IsNil(int64_t idx) const = 0;
  [[nodiscard]] virtual bool IsNoneOrNil(int64_t idx) const = 0;
  [[nodiscard]] virtual bool IsBoolean(int64_t idx) const = 0;
  [[nodiscard]] virtual bool IsInteger(int64_t idx) const = 0;
  [[nodiscard]] virtual bool IsNumber(int64_t idx) const = 0;
  [[nodiscard]] virtual bool IsString(int64_t idx) const = 0;
  [[nodiscard]] virtual bool IsFunction(int64_t idx) const = 0;
  [[nodiscard]] virtual bool ToBoolean(int64_t idx) const = 0;
  [[nodiscard]] virtual LuaInteger ToInteger(int64_t idx) const = 0;
  [[nodiscard]] virtual std::tuple<LuaInteger, bool> ToIntegerX(int64_t idx) const = 0;
  [[nodiscard]] virtual LuaNumber ToNumber(int64_t idx) const = 0;
  [[nodiscard]] virtual std::tuple<LuaNumber, bool> ToNumberX(int64_t idx) const = 0;
  virtual std::string ToString(int64_t idx) = 0;
  virtual std::tuple<std::string, bool> ToStringX(int64_t idx) = 0;
  virtual void *ToPointer(int64_t idx) = 0;

  virtual void PushNil() = 0;
  virtual void PushBoolean(bool) = 0;
  virtual void PushInteger(LuaInteger) = 0;
  virtual void PushNumber(LuaNumber) = 0;
  virtual void PushString(std::string) = 0;

  virtual void Arith(ArithOp op) = 0;
  virtual bool Compare(int64_t idx_1, int64_t idx_2, CompareOp op) = 0;
  virtual void Len(int64_t idx) = 0;
  virtual void Concat(size_t n) = 0;

  virtual void NewTable() = 0;
  virtual void CreateTable(int64_t n_arr, int64_t n_rec) = 0;
  virtual LuaValueType GetTable(int64_t idx) = 0;
  virtual LuaValueType GetField(int64_t idx, std::string k) = 0;
  virtual LuaValueType GetI(int64_t idx, int64_t i) = 0;

  virtual void SetTable(int64_t idx) = 0;
  virtual void SetField(int64_t idx, std::string k) = 0;
  virtual void SetI(int64_t idx, int64_t i) = 0;

  virtual api::Status Load(const std::vector<Byte> &chunk,
                           std::string chunk_name, std::string mode) = 0;
  virtual api::Status Load(const std::string &file) = 0;
  virtual void Call(int32_t n_args, int32_t n_results) = 0;

  virtual void PushNativeFunction(NativeFunction f) = 0;
  virtual bool IsNativeFunction(int32_t idx) = 0;
  virtual NativeFunction ToNativeFunction(int32_t idx) = 0;

  virtual void PushGlobalTable() = 0;
  virtual LuaValueType GetGlobal(const std::string &name) = 0;
  virtual void SetGlobal(const std::string &name) = 0;
  virtual void Register(const std::string &name, NativeFunction f) = 0;

  virtual void PushNativeClosure(NativeFunction f, int32_t n) = 0;

  virtual bool GetMetatable(int64_t idx) = 0;
  virtual void SetMetatable(int64_t idx) = 0;
  virtual size_t RawLen(int64_t idx) = 0;
  virtual bool RawEqual(int64_t idx_1, int64_t idx_2) = 0;
  virtual LuaValueType RawGet(int64_t idx) = 0;
  virtual void RawSet(int64_t idx) = 0;
  virtual LuaValueType RawGetI(int64_t idx, int64_t i) = 0;
  virtual void RawSetI(int64_t idx, int64_t i) = 0;

  virtual bool Next(int64_t idx) = 0;

  virtual int32_t Error() = 0;
  virtual api::Status PCall(int32_t n_args, int32_t n_results, int32_t msg) = 0;

  virtual bool StringToNumber(const std::string &s) = 0;

  virtual std::shared_ptr<LuaState> NewThread() = 0;
  virtual Status Resume(const std::shared_ptr<LuaState> &from, int32_t n_args) = 0;
  virtual int64_t Yield(int32_t n_results) = 0;
  virtual Status Status() = 0;
  virtual bool IsYieldable() = 0;
  virtual std::shared_ptr<LuaState> ToThread(int64_t idx) = 0;
  virtual bool PushThread() = 0;
  virtual void XMove(const std::shared_ptr<LuaState> &to, size_t n) = 0;
  virtual bool GetStack() = 0;

  virtual void Destroy() = 0;
};

}

#endif //EMBRYO_INCLUDE_EMBRYO_API_BASIC_API_H_
