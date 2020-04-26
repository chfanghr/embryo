//
// Created by 方泓睿 on 2020/4/15.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_API_AUX_LIB_H_
#define EMBRYO_INCLUDE_EMBRYO_API_AUX_LIB_H_

#include <string>
#include <map>

#include <embryo/api/basic_api.h>

namespace embryo::api {
class AuxLib {
 public:
  virtual int32_t Error2(const std::string &msg) = 0;
  virtual int32_t ArgError(int32_t arg, const std::string &extra_message) = 0;

  virtual void CheckStack2(int32_t sz, const std::string &msg) = 0;
  virtual void ArgCheck(bool cond, int32_t arg, const std::string &extra_msg) = 0;
  virtual void CheckAny(int32_t arg) = 0;
  virtual void CheckType(int32_t arg, LuaValueType type) = 0;
  virtual int64_t CheckInteger(int32_t arg) = 0;
  virtual double CheckNumber(int32_t arg) = 0;
  virtual std::string CheckString(int32_t arg) = 0;
  virtual int64_t OptInteger(int32_t arg, int64_t d) = 0;
  virtual double OptNumber(int32_t arg, double d) = 0;
  virtual std::string OptString(int32_t arg, const std::string &d) = 0;

  virtual bool DoFile(const std::string &file) = 0;
  virtual bool DoString(const std::string &str) = 0;
  virtual Status LoadFile(const std::string &file) = 0;
  virtual Status LoadFileX(const std::string &file, const std::string &mode) = 0;
  virtual Status LoadString(const std::string &s) = 0;

  virtual std::string TypeName2(int64_t idx) = 0;
  virtual std::string ToString2(int64_t idx) = 0;
  virtual int64_t Len2(int64_t idx) = 0;
  virtual bool GetSubTable(int64_t idx, const std::string &f_name) = 0;
  virtual LuaValueType GetMetaField(int32_t obj, const std::string &e) = 0;
  virtual bool CallMeta(int32_t obj, const std::string &e) = 0;
  virtual void OpenLibs() = 0;
  virtual void RequireF(const std::string &mod_name, const NativeFunction &open_f, bool glb) = 0;
  virtual void NewLib(const FuncReg &reg) = 0;
  virtual void NewLibTable(const FuncReg &reg) = 0;
  virtual void SetFuncs(const FuncReg &reg, int32_t nup) = 0;
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_API_AUX_LIB_H_
