//
// Created by 方泓睿 on 2020/4/24.
//

#include <vector>
#include <filesystem>

#include <embryo/std_lib/lib_package.h>
#include <embryo/state/lua_state.h>
#include <embryo/utils/strings.h>

#include <fmt/format.h>

namespace embryo::std_lib {
api::FuncReg kPackageLib{ // NOLINT(cert-err58-cpp)
    {"searchpath", package::SearchPath},
    {"preload", nullptr},
    {"cpath", nullptr},
    {"path", nullptr},
    {"searchers", nullptr},
    {"loaded", nullptr},
};

api::FuncReg kLlFuncs{ // NOLINT(cert-err58-cpp)
    {"require", package::Require}
};

const std::string kLuaLoadedTable = "_LOADED"; // NOLINT(cert-err58-cpp)
const std::string kLuaPreloadTable = "_PRELOAD"; // NOLINT(cert-err58-cpp)

// TODO
const std::string kLuaDirSep = "/"; // NOLINT(cert-err58-cpp)
const std::string kLuaPathSep = ";"; // NOLINT(cert-err58-cpp)
const std::string kLuaPathMark = "?"; // NOLINT(cert-err58-cpp)
const std::string kLuaExecDir = "!"; // NOLINT(cert-err58-cpp)
const std::string kLuaIGMask = "-"; // NOLINT(cert-err58-cpp)

namespace {
int32_t PreloadSearcher(const std::shared_ptr<api::LuaState> &state) {
  auto name = state->CheckString(1);
  state->GetField(api::kLuaRegistryIndex, "_PRELOAD");
  if (state->GetField(-1, name) == api::LuaValueType::kNil)
    state->PushString(fmt::format("\n\tno field package.preload[{}]", name));
  return 1;
}

std::tuple<std::string, std::string> SearchPathInternal(std::string name,
                                                        const std::string &path,
                                                        const std::string &sep,
                                                        const std::string &dir_sep) {
  if (!sep.empty())
    name = utils::ReplaceAll(name, sep, dir_sep);

  std::string err_msg{};

  for (auto file_name:utils::Split(path, kLuaPathSep)) {
    file_name = utils::ReplaceAll(file_name, kLuaPathMark, name);
    if (std::filesystem::exists(file_name))
      return {file_name, ""};
    err_msg += fmt::format("\n\tno file '{}'", file_name);
  }
  return {"", err_msg};
}

int32_t LuaSearcher(const std::shared_ptr<api::LuaState> &state) {
  auto name = state->CheckString(1);
  state->GetField(state::LuaUpValueIndex(1), "path");
  auto[path, ok]=state->ToStringX(-1);
  if (!ok)
    state->Error2("'package.path' must be a string");

  auto[file_name, err_msg]=SearchPathInternal(name, path, ".", kLuaDirSep);
  if (!err_msg.empty()) {
    state->PushString(err_msg);
    return 1;
  }

  if (state->LoadFile(file_name) == api::Status::kOk) {
    state->PushString(file_name);
    return 2;
  }

  return state->Error2(fmt::format("error loading module '{}' from file '{}':\n\t{}",
                                   state->CheckString(1),
                                   file_name,
                                   state->CheckString(-1)));
}

void CreateSearchersTable(const std::shared_ptr<api::LuaState> &state) {
  auto searchers = std::vector<api::NativeFunction>{
      PreloadSearcher, LuaSearcher
  };

  state->CreateTable(searchers.size(), 0);

  for (size_t i = 0; i < searchers.size(); ++i) {
    state->PushValue(-2);
    state->PushNativeClosure(searchers[i], 1);
    state->RawSetI(-2, i + 1);
  }
  state->SetField(-2, "searchers");
}

void FindLoader(const std::shared_ptr<api::LuaState> &state, const std::string &name) {
  if (state->GetField(state::LuaUpValueIndex(1), "searchers") != api::LuaValueType::kTable)
    state->Error2("'package.searchers' must be a table");

  std::string err_msg = fmt::format("module '{}' not found", name);

  for (int64_t i = 1;; ++i) {
    if (state->RawGetI(3, i) == api::LuaValueType::kNil) {
      state->Pop(1);
      state->Error2(err_msg);
    }

    state->PushString(name);
    state->Call(1, 2);
    if (state->IsFunction(-2))
      return;
    else if (state->IsString(-2)) {
      state->Pop(1);
      err_msg += state->CheckString(-1);
    } else
      state->Pop(2);
  }
}
}

int32_t OpenPackageLib(const std::shared_ptr<api::LuaState> &state) {
  state->NewLib(kPackageLib);
  CreateSearchersTable(state);

  state->PushString("./?.lua;./?/init.lua");
  state->SetField(-2, "path");

  state->PushString(
      kLuaDirSep + "\n" + kLuaPathSep + "\n" + kLuaPathMark + "\n" + kLuaExecDir + "\n" + kLuaIGMask + "\n");
  state->SetField(-2, "config");

  state->GetSubTable(api::kLuaRegistryIndex, kLuaLoadedTable);
  state->SetField(-2, "loaded");

  state->GetSubTable(api::kLuaRegistryIndex, kLuaPreloadTable);
  state->SetField(-2, "preload");
  state->PushGlobalTable();
  state->PushValue(-2);
  state->SetFuncs(kLlFuncs, 1);
  state->Pop(1);
  return 1;
}

namespace package {

int32_t SearchPath(const std::shared_ptr<api::LuaState> &state) {
  auto name = state->CheckString(1);
  auto path = state->CheckString(2);
  auto sep = state->OptString(3, ".");
  auto rep = state->OptString(4, kLuaDirSep);
  auto[file_name, err_msg]= SearchPathInternal(name, path, sep, rep);
  if (err_msg.empty()) {
    state->PushString(file_name);
    return 1;
  }

  state->PushNil();
  state->PushString(err_msg);
  return 2;
}

int32_t Require(const std::shared_ptr<api::LuaState> &state) {
  auto name = state->CheckString(1);
  state->SetTop(1);
  state->GetField(api::kLuaRegistryIndex, kLuaLoadedTable);
  state->GetField(2, name);
  if (state->ToBoolean(-1))
    return 1;
  state->Pop(1);
  FindLoader(state, name);
  state->PushString(name);
  state->Insert(-2);
  state->Call(2, 1);
  if (!state->IsNil(-1))
    state->SetField(2, name);
  if (state->GetField(2, name) == api::LuaValueType::kNil) {
    state->PushBoolean(true);
    state->PushValue(-1);
    state->SetField(2, name);
  }
  return 1;
}
}
}