//
// Created by 方泓睿 on 2020/4/7.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_BIN_CHUNK_READER_H_
#define EMBRYO_INCLUDE_EMBRYO_BIN_CHUNK_READER_H_

#include <memory>

#include <embryo/utils/bytes.h>
#include <embryo/bin_chunk/bin_chunk.h>

namespace embryo::bin_chunk {
class Reader {
 private:
  std::vector<Byte> data_;
 private:
  [[noreturn]] void Error(const std::string &msg) const;

  void EnsureSize(size_t size);
 public:
  explicit Reader(std::vector<Byte> data);

 private:
  std::vector<Byte> ReadBytes(size_t size);

  template<typename T>
  T Read() { return *(T *) ReadBytes(sizeof(T)).data(); }

  Byte ReadByte();

  uint64_t ReadUint64();

  uint32_t ReadUint32();

  LuaInteger ReadLuaInteger();

  LuaNumber ReadLuaNumber();

  std::string ReadString();

  template<size_t S>
  std::array<Byte, S> ReadByteArray() {
    return utils::Convert<Byte, S>(ReadBytes(S));
  }

  void ValidateHeader();

  std::shared_ptr<Prototype> ReadProto(std::string parent_source);

  std::vector<std::shared_ptr<Prototype>> ReadProtos(const std::string &parent_source);

  LuaConstant ReadConstant();

  std::vector<uint32_t> ReadCode();

  std::vector<LuaConstant> ReadConstants();

  std::vector<UpValue> ReadUpValues();

  std::vector<uint32_t> ReadLineInfo();

  std::vector<LocVar> ReadLocVars();

  std::vector<std::string> ReadUpValueNames();

  Header ReadHeader();

  friend std::shared_ptr<Prototype> Undump(const std::vector<Byte> &data);
};

std::shared_ptr<Prototype> Undump(const std::vector<Byte> &data);

std::shared_ptr<Prototype> Undump(const std::string &data);

std::shared_ptr<bin_chunk::Prototype> LoadBinChunk(const std::string &file);
}
#endif //EMBRYO_INCLUDE_EMBRYO_BIN_CHUNK_READER_H_
