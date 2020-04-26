//
// Created by 方泓睿 on 2020/4/7.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_BIN_CHUNK_BIN_CHUNK_H_
#define EMBRYO_INCLUDE_EMBRYO_BIN_CHUNK_BIN_CHUNK_H_

#include <cstdlib>
#include <array>
#include <string>
#include <utility>
#include <variant>

#include <fmt/format.h>

#include <embryo/utils/inspection.h>
#include <embryo/utils/stringer.h>
#include <embryo/utils/types.h>

namespace embryo::bin_chunk {

using LuaConstant= std::variant<std::nullptr_t,
                                bool,
                                LuaNumber,
                                LuaInteger,
                                std::string>;

enum class LuaConstantTag : Byte {
  kNil = 0x00,
  kBoolean = 0x01,
  kNumber = 0x03,
  kInteger = 0x13,
  kShortStr = 0x04,
  kLongStr = 0x14
};

LuaConstantTag TypeOf(const LuaConstant &c);

struct Header : utils::Stringer {
  std::array<Byte, 4> signature{};
  Byte version{};
  Byte format{};
  std::array<Byte, 6> luac_data{};
  Byte cint_size{};
  Byte size_t_size{};
  Byte instruction_size{};
  Byte lua_integer_size{};
  Byte lua_number_size{};
  LuaInteger luac_int{};
  LuaNumber luac_num{};

  Header() = default;
  Header(const std::array<Byte, 4> &signature,
         Byte version,
         Byte format,
         const std::array<Byte, 6> &luac_data,
         Byte cint_size,
         Byte size_t_size,
         Byte instruction_size,
         Byte lua_integer_size,
         Byte lua_number_size,
         LuaInteger luac_int,
         LuaNumber luac_num)
      : signature(signature),
        version(version),
        format(format),
        luac_data(luac_data),
        cint_size(cint_size),
        size_t_size(size_t_size),
        instruction_size(instruction_size),
        lua_integer_size(lua_integer_size),
        lua_number_size(lua_number_size),
        luac_int(luac_int),
        luac_num(luac_num) {}

  [[nodiscard]] std::string String() const override;
};

const Header kDefaultHeader{ // NOLINT(cert-err58-cpp)
    {0x1b, 0x4c, 0x75, 0x61},
    0x53,
    0,
    {0x19, 0x93, 0x0d, 0x0a, 0x1a, 0x0a},
    sizeof(int),
    sizeof(size_t),
    sizeof(uint32_t),
    sizeof(int64_t),
    sizeof(double),
    0x5678,
    370.5
};

struct UpValue : utils::Stringer {
  Byte in_stack{};
  Byte idx{};

  UpValue() = default;
  UpValue(Byte in_stack, Byte idx) : in_stack(in_stack), idx(idx) {}

  [[nodiscard]] std::string String() const override;
};

struct LocVar : utils::Stringer {
  std::string var_name;
  uint32_t start_pc{};
  uint32_t end_pc{};

  LocVar() = default;
  LocVar(std::string var_name, uint32_t start_pc, uint32_t end_pc)
      : var_name(std::move(var_name)), start_pc(start_pc), end_pc(end_pc) {}

  [[nodiscard]] std::string String() const override;
};

struct Prototype : utils::Stringer {
  std::string source;
  uint32_t line_defined{};
  uint32_t last_line_defined{};
  Byte num_params{};
  Byte is_var_arg{};
  Byte max_stack_size{};
  std::vector<uint32_t> code;
  std::vector<LuaConstant> constants;
  std::vector<UpValue> up_values;
  std::vector<std::shared_ptr<Prototype>> protos;
  std::vector<uint32_t> line_info;
  std::vector<LocVar> loc_vars;
  std::vector<std::string> up_value_names;

  Prototype() = default;
  Prototype(std::string source,
            uint32_t line_defined,
            uint32_t last_line_defined,
            Byte num_params,
            Byte is_var_arg,
            Byte max_stack_size,
            std::vector<uint32_t> code,
            std::vector<LuaConstant> constants,
            std::vector<UpValue> up_values,
            std::vector<std::shared_ptr<Prototype>> protos,
            std::vector<uint32_t> line_info,
            std::vector<LocVar> loc_vars,
            std::vector<std::string> up_value_names)
      : source(std::move(source)),
        line_defined(line_defined),
        last_line_defined(last_line_defined),
        num_params(num_params),
        is_var_arg(is_var_arg),
        max_stack_size(max_stack_size),
        code(std::move(code)),
        constants(std::move(constants)),
        up_values(std::move(up_values)),
        protos(std::move(protos)),
        line_info(std::move(line_info)),
        loc_vars(std::move(loc_vars)),
        up_value_names(std::move(up_value_names)) {}

  [[nodiscard]] std::string String() const override;

  void PrettyPrint() const;

  [[nodiscard]] std::string UpValName(size_t idx) const;
 private:
  void PrintHeader() const;

  void PrintCode() const;

  void PrintDetail() const;
};

template<typename T>
std::tuple<T, bool> TryCast(const LuaConstant &val) noexcept {
  if (std::holds_alternative<T>(val))
    return {std::get<T>(val), true};
  return {{}, false};
}

template<typename T>
T Cast(const LuaConstant &val) {
  return std::get<T>(val);
}

bool IsBinChunk(const std::vector<Byte> &chunk);
}

template<>
struct fmt::formatter<embryo::bin_chunk::LuaConstant> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template<typename FormatContext>
  auto format(const embryo::bin_chunk::LuaConstant &c, FormatContext &ctx) {
    if (std::holds_alternative<std::nullptr_t>(c))
      return format_to(ctx.out(), "null");
    if (std::holds_alternative<bool>(c))
      return format_to(ctx.out(), std::get<bool>(c) ? "true" : "false");
    if (std::holds_alternative<double>(c))
      return format_to(ctx.out(), "{}", std::get<double>(c));
    if (std::holds_alternative<int64_t>(c))
      return format_to(ctx.out(), "{}", std::get<int64_t>(c));
    return format_to(ctx.out(), "{}", AS_STRING(std::get<std::string>(c)));
  }
};
#endif //EMBRYO_INCLUDE_EMBRYO_BIN_CHUNK_BIN_CHUNK_H_
