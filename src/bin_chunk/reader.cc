//
// Created by 方泓睿 on 2020/4/7.
//

#include <utility>
#include <stdexcept>
#include <algorithm>

#include <embryo/bin_chunk/reader.h>
#include <embryo/utils/io.h>

namespace embryo::bin_chunk {
Reader::Reader(std::vector<Byte> data) : data_(std::move(data)) {}

void Reader::EnsureSize(size_t size) {
  if (data_.size() < size)
    Error(
        fmt::format("EnsureSize: desired size {} greater than available size {}",
                    size, data_.size()));
}

void Reader::Error(const std::string &msg) const {
  throw std::runtime_error(fmt::format("{}: {}", THIS_TYPE, msg));
}

std::vector<Byte> Reader::ReadBytes(size_t size) {
  EnsureSize(size);
  std::vector<Byte> res(data_.begin(), data_.begin() + size);
  data_.erase(data_.begin(), data_.begin() + size);
  return res;
}

uint64_t Reader::ReadUint64() { return Read<uint64_t>(); }

uint32_t Reader::ReadUint32() { return Read<uint32_t>(); }

LuaInteger Reader::ReadLuaInteger() { return Read<LuaInteger>(); }

LuaNumber Reader::ReadLuaNumber() { return Read<LuaNumber>(); }

std::string Reader::ReadString() {
  size_t size = ReadByte();
  if (size == 0) return "";
  if (size == 0xFF) size = ReadUint64();
  auto bytes = ReadBytes(size - 1);
  return std::string(bytes.begin(), bytes.end());
}

Byte Reader::ReadByte() {
  return Read<Byte>();
}

Header Reader::ReadHeader() {
  return Header{
      ReadByteArray<4>(),
      ReadByte(),
      ReadByte(),
      ReadByteArray<6>(),
      ReadByte(),
      ReadByte(),
      ReadByte(),
      ReadByte(),
      ReadByte(),
      ReadLuaInteger(),
      ReadLuaNumber()
  };
}

void Reader::ValidateHeader() {
  auto error = [&](const std::string &msg) {
    Error(fmt::format("ValidateHeader: {}", msg));
  };

  auto header = ReadHeader();

  if (!std::equal(header.signature.begin(), header.signature.end(),
                  kDefaultHeader.signature.begin()))
    error("invalid signature");

  if (header.version != kDefaultHeader.version)
    error("version mismatch");

  if (header.format != kDefaultHeader.format)
    error("format mismatch");

  if (!std::equal(header.luac_data.begin(), header.luac_data.end(),
                  kDefaultHeader.luac_data.begin()))
    error("corrupted");

  if (header.cint_size != kDefaultHeader.cint_size)
    error("int size mismatch");

  if (header.size_t_size != kDefaultHeader.size_t_size)
    error("size_t size mismatch");

  if (header.lua_integer_size != kDefaultHeader.lua_integer_size)
    error("lua_integer_size size mismatch");

  if (header.lua_number_size != kDefaultHeader.lua_number_size)
    error("lua_number_size size mismatch");

  if (header.luac_int != kDefaultHeader.luac_int)
    error("luac_int mismatch");

  if (header.luac_num != kDefaultHeader.luac_num)
    error("luac_num mismatch");
}

std::shared_ptr<Prototype> Reader::ReadProto(std::string parent_source) {
  auto source = ReadString();
  if (source.empty())
    source = std::move(parent_source);

  return std::make_shared<Prototype>(
      source,
      ReadUint32(),
      ReadUint32(),
      ReadByte(),
      ReadByte(),
      ReadByte(),
      ReadCode(),
      ReadConstants(),
      ReadUpValues(),
      ReadProtos(source),
      ReadLineInfo(),
      ReadLocVars(),
      ReadUpValueNames());
}

std::vector<uint32_t> Reader::ReadCode() {
  std::vector<uint32_t> res(ReadUint32());
  for (auto &code:res)
    code = ReadUint32();
  return res;
}

std::vector<LuaConstant> Reader::ReadConstants() {
  std::vector<LuaConstant> res(ReadUint32());

  for (auto &constant: res)
    constant = ReadConstant();

  return res;
}

LuaConstant Reader::ReadConstant() {
  switch (LuaConstantTag (ReadByte())) {
    case LuaConstantTag::kNil:return std::nullptr_t();
    case LuaConstantTag::kBoolean :return ReadByte() != 0;
    case LuaConstantTag::kInteger:return ReadLuaInteger();
    case LuaConstantTag::kNumber:return ReadLuaNumber();
    case LuaConstantTag::kShortStr:
    case LuaConstantTag::kLongStr:return ReadString();
    default:Error("ReadConstant: unknown lua constant tag");
  }
}

std::vector<UpValue> Reader::ReadUpValues() {
  std::vector<UpValue> res(ReadUint32());

  for (auto &up_value:res)
    up_value = {
        ReadByte(),
        ReadByte()
    };

  return res;
}

std::vector<std::shared_ptr<Prototype>> Reader::ReadProtos(const std::string &parent_source) {
  std::vector<std::shared_ptr<Prototype>> res(ReadUint32());

  for (auto &p:res)
    p = ReadProto(parent_source);

  return res;
}

std::vector<uint32_t> Reader::ReadLineInfo() {
  std::vector<uint32_t> res(ReadUint32());

  for (auto &l:res)
    l = ReadUint32();

  return res;
}

std::vector<LocVar> Reader::ReadLocVars() {
  std::vector<LocVar> res(ReadUint32());

  for (auto &l:res)
    l = {
        ReadString(),
        ReadUint32(),
        ReadUint32()
    };

  return res;
}

std::vector<std::string> Reader::ReadUpValueNames() {
  std::vector<std::string> res(ReadUint32());

  for (auto &s:res)
    s = ReadString();

  return res;
}

std::shared_ptr<Prototype> Undump(const std::vector<Byte> &data) {
  Reader reader(data);
  reader.ValidateHeader();
  reader.ReadByte();
  return reader.ReadProto("");
}

std::shared_ptr<Prototype> Undump(const std::string &data) {
  return Undump(utils::Convert(data));
}

std::shared_ptr<bin_chunk::Prototype> LoadBinChunk(const std::string &file) {
  return Undump(utils::ReadFile<std::vector<Byte>>(file));
}
}