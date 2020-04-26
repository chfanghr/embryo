//
// Created by 方泓睿 on 2020/4/18.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_BIN_CHUNK_WRITER_H_
#define EMBRYO_INCLUDE_EMBRYO_BIN_CHUNK_WRITER_H_

#include <memory>

#include <embryo/utils/bytes.h>
#include <embryo/bin_chunk/bin_chunk.h>

namespace embryo::bin_chunk {
class Writer {
 private:
  std::vector<Byte> data_{};

 private:
  [[noreturn]] void Error(const std::string &msg) const;

 public:
  Writer() = default;

  void WriteByte(Byte b);

  void WriteBytes(const std::vector<Byte> &data, size_t size);

  void WriteBytes(const std::string &data, size_t size);

  template<typename T>
  void Write(const T &obj) {
    const Byte *obj_bytes = (const Byte *) &obj;
    std::vector<Byte> obj_data(obj_bytes, obj_bytes + sizeof(T));
    WriteBytes(obj_data, sizeof(T));
  }

  void WriteUint64(uint64_t);

  void WriteUint32(uint32_t);

  [[nodiscard]] std::vector<Byte> Data() const;

  std::vector<Byte> &Data();

  void WriteLuaInteger(LuaInteger);

  void WriteLuaNumber(LuaNumber);

  void WriteString(const std::string &);

  template<size_t S>
  void WriteByteArray(const std::array<Byte, S> &arr) {
    WriteBytes(utils::Convert<Byte, S>(arr), S);
  }

  void WriteProto(const std::shared_ptr<Prototype> &);

  void WriteProtos(const std::vector<std::shared_ptr<Prototype>> &);

  void WriteConstant(const LuaConstant &);

  void WriteConstants(const std::vector<LuaConstant> &);

  void WriteUpValues(const std::vector<UpValue> &);

  void WriteLineInfo(const std::vector<uint32_t> &);

  void WriteLocVar(const LocVar &);

  void WriteLocVars(const std::vector<LocVar> &);

  void WriteUpValuesNames(const std::vector<std::string> &);

  void WriteHeader(const Header &header);

  void WriteCode(const std::vector<uint32_t> &);

  void WriteUpValue(const UpValue &val);
};

template<typename T=std::vector<Byte>>
T Dump(const std::shared_ptr<Prototype> &);

template<>
std::vector<Byte> Dump<std::vector<Byte>>(const std::shared_ptr<Prototype> &);

template<>
std::string Dump<std::string>(const std::shared_ptr<Prototype> &);

void Dump(const std::shared_ptr<Prototype> &, const std::string &file);
}
#endif //EMBRYO_INCLUDE_EMBRYO_BIN_CHUNK_WRITER_H_
