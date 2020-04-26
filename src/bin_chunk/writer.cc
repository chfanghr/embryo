//
// Created by 方泓睿 on 2020/4/18.
//

#include <stdexcept>

#include <embryo/utils/io.h>
#include <embryo/utils/inspection.h>
#include <embryo/bin_chunk/writer.h>

namespace embryo::bin_chunk {
void Writer::WriteByte(Byte b) {
  data_.push_back(b);
}

void Writer::WriteBytes(const std::vector<Byte> &data, size_t size) {
  if (data.size() < size)
    Error(MSG_WITH_FUNC_NAME("desired data size is larger than the available data"));

  for (size_t i = 0; i < size; i++)
    WriteByte(data[i]);
}

void Writer::WriteBytes(const std::string &data, size_t size) {
  if (data.size() < size)
    Error(MSG_WITH_FUNC_NAME("desired data size is larger than the available data"));

  for (size_t i = 0; i < size; i++)
    WriteByte(data[i]);
}

void Writer::Error(const std::string &msg) const {
  throw std::runtime_error(fmt::format("{}: {}", THIS_TYPE, msg));
}

void Writer::WriteUint64(uint64_t val) { Write(val); }

void Writer::WriteUint32(uint32_t val) { Write(val); }

std::vector<Byte> &Writer::Data() { return data_; }

std::vector<Byte> Writer::Data() const { return data_; }

void Writer::WriteLuaInteger(LuaInteger val) { Write(val); }

void Writer::WriteLuaNumber(LuaNumber val) { Write(val); }

void Writer::WriteString(const std::string &str) {
  if (str.empty()) {
    WriteByte(0);
    return;
  } else if (str.size() >= 0xFE) {
    WriteByte(0xFF);
    WriteUint64(str.size() + 1);
  } else
    WriteByte(str.size() + 1);
  WriteBytes(str, str.size());
}

void Writer::WriteConstant(const LuaConstant &c) {
  auto constant_type = TypeOf(c);
  WriteByte(static_cast<Byte>(constant_type));
  switch (constant_type) {
    case LuaConstantTag::kNil:return;
    case LuaConstantTag::kBoolean:Write(Cast<bool>(c) ? 1 : 0);
      return;
    case LuaConstantTag::kNumber: WriteLuaNumber(Cast<LuaNumber>(c));
      return;
    case LuaConstantTag::kInteger: WriteLuaInteger(Cast<LuaInteger>(c));
      return;
    case LuaConstantTag::kShortStr:
    case LuaConstantTag::kLongStr:WriteString(Cast<std::string>(c));
  }
}

void Writer::WriteHeader(const Header &header) {
  WriteByteArray(header.signature);
  WriteByte(header.version);
  WriteByte(header.format);
  WriteByteArray(header.luac_data);
  WriteByte(header.cint_size);
  WriteByte(header.size_t_size);
  WriteByte(header.instruction_size);
  WriteByte(header.lua_integer_size);
  WriteByte(header.lua_number_size);
  WriteLuaInteger(header.luac_int);
  WriteLuaNumber(header.luac_num);
}

void Writer::WriteConstants(const std::vector<LuaConstant> &cs) {
  WriteUint32(cs.size());
  for (const auto &c:cs)
    WriteConstant(c);
}

void Writer::WriteCode(const std::vector<uint32_t> &cs) {
  WriteUint32(cs.size());
  for (const auto &c:cs)
    WriteUint32(c);
}

void Writer::WriteUpValue(const UpValue &val) {
  WriteByte(val.in_stack);
  WriteByte(val.idx);
}

void Writer::WriteUpValues(const std::vector<UpValue> &vals) {
  WriteUint32(vals.size());
  for (const auto &val:vals)
    WriteUpValue(val);
}

void Writer::WriteLineInfo(const std::vector<uint32_t> &ls) {
  WriteUint32(ls.size());
  for (const auto &l:ls)
    WriteUint32(l);
}

void Writer::WriteLocVar(const LocVar &var) {
  WriteString(var.var_name);
  WriteUint32(var.start_pc);
  WriteUint32(var.end_pc);
}

void Writer::WriteLocVars(const std::vector<LocVar> &vars) {
  WriteUint32(vars.size());
  for (const auto &var :vars)
    WriteLocVar(var);
}

void Writer::WriteUpValuesNames(const std::vector<std::string> &names) {
  WriteUint32(names.size());
  for (const auto &name:names)
    WriteString(name);
}

void Writer::WriteProto(const std::shared_ptr<Prototype> &proto) {
  WriteString(proto->source);
  WriteUint32(proto->line_defined);
  WriteUint32(proto->last_line_defined);
  WriteByte(proto->num_params);
  WriteByte(proto->is_var_arg);
  WriteByte(proto->max_stack_size);
  WriteCode(proto->code);
  WriteConstants(proto->constants);
  WriteUpValues(proto->up_values);
  WriteProtos(proto->protos);
  WriteLineInfo(proto->line_info);
  WriteLocVars(proto->loc_vars);
  WriteUpValuesNames(proto->up_value_names);
}

void Writer::WriteProtos(const std::vector<std::shared_ptr<Prototype> > &protos) {
  WriteUint32(protos.size());
  for (const auto &proto:protos)
    WriteProto(proto);
}

template<>
std::vector<Byte> Dump<std::vector<Byte>>(const std::shared_ptr<Prototype> &p) {
  Writer writer{};
  writer.WriteHeader(kDefaultHeader);
  writer.WriteByte(0);
  writer.WriteProto(p);
  return writer.Data();
}

template<>
std::string Dump<std::string>(const std::shared_ptr<Prototype> &p) {
  return utils::Convert(Dump(p));
}

void Dump(const std::shared_ptr<Prototype> &p, const std::string &file) {
  utils::WriteFile<std::vector<Byte>>(Dump(p), file);
}
}