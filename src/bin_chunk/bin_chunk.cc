//
// Created by 方泓睿 on 2020/4/18.
//

#include <stdexcept>

#include <embryo/utils/inspection.h>
#include <embryo/bin_chunk/bin_chunk.h>

namespace embryo::bin_chunk {
std::string Prototype::UpValName(size_t idx) const {
  if (!up_value_names.empty())
    return up_value_names[idx];

  return "-";
}

bool IsBinChunk(const std::vector<Byte> &chunk) {
  return chunk.size() >= 4 && std::equal(chunk.begin(), chunk.begin() + 4, kDefaultHeader.signature.begin());
}

LuaConstantTag TypeOf(const LuaConstant &c) {
  {
    auto[val, ok]= TryCast<std::nullptr_t>(c);
    if (ok) return LuaConstantTag::kNil;
  }
  {
    auto[val, ok]= TryCast<bool>(c);
    if (ok) return LuaConstantTag::kBoolean;
  }
  {
    auto[val, ok]= TryCast<LuaNumber>(c);
    if (ok) return LuaConstantTag::kNumber;
  }
  {
    auto[val, ok]= TryCast<LuaInteger>(c);
    if (ok) return LuaConstantTag::kInteger;
  }
  {
    auto[val, ok]= TryCast<std::string>(c);
    if (val.size() <= 0xFE)
      return LuaConstantTag::kShortStr;
    else
      return LuaConstantTag::kLongStr;
  }
  throw std::runtime_error(MSG_WITH_FUNC_NAME("unsupported lua constant type"));
}
}