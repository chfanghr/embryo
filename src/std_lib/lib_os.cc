//
// Created by 方泓睿 on 2020/4/21.
//

//#include <sys/syslimits.h>
//#include <fcntl.h>
//
//#include <cerrno>
#include <cstdio>
#include <ctime>
#include <chrono>

#include <fmt/format.h>

#include <embryo/utils/inspection.h>
#include <embryo/std_lib/lib_os.h>

namespace embryo::std_lib {
api::FuncReg kOsLib{ // NOLINT(cert-err58-cpp)
    {"clock", os::Clock},
    {"difftime", os::DiffTime},
    {"time", os::Time},
    {"date", os::Date},
    {"remove", os::Remove},
    {"rename", os::Rename},
    {"tmpname", os::TmpName},
    {"getenv", os::GetEnv},
    {"execute", os::Execute},
    {"exit", os::Exit},
    {"setlocale", os::SetLocale}
};

int32_t OpenOsLib(const std::shared_ptr<api::LuaState> &state) {
  state->NewLib(kOsLib);
  return 1;
}

namespace os {
int32_t Clock(const std::shared_ptr<api::LuaState> &state) {
  auto c = (double) std::clock() / CLOCKS_PER_SEC;
  state->PushNumber(c);
  return 1;
}

int32_t DiffTime(const std::shared_ptr<api::LuaState> &state) {
  time_t t2 = state->CheckInteger(1);
  time_t t1 = state->CheckInteger(2);
  state->PushInteger(std::difftime(t2, t1));
  return 1;
}

namespace {
int64_t GetField(const std::shared_ptr<api::LuaState> &state, const std::string &key, int64_t dft) {
  auto t = state->GetField(-1, key);
  auto[res, is_num]= state->ToIntegerX(-1);
  if (!is_num) {
    if (t != api::LuaValueType::kNil)
      return state->Error2(fmt::format("field '{}' is not a integer", key));
    else if (dft < 0)
      return state->Error2(fmt::format("field '{}' missing in date table", key));
    res = dft;
  }
  state->Pop(1);
  return res;
}
}

int32_t Time(const std::shared_ptr<api::LuaState> &state) {
  if (state->IsNoneOrNil(1)) {
    using namespace std::chrono;
    int64_t time_stamp = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    state->PushInteger(time_stamp);
  } else {
    state->CheckType(1, api::LuaValueType::kTable);
    std::tm given_time{
        .tm_sec=static_cast<int>(GetField(state, "sec", 0)),
        .tm_min=static_cast<int>(GetField(state, "min", 0)),
        .tm_hour=static_cast<int>(GetField(state, "hour", 12)),
        .tm_mday=static_cast<int>(GetField(state, "day", -1)),
        .tm_mon=static_cast<int>(GetField(state, "month", -1)),
        .tm_year=static_cast<int>(GetField(state, "year", -1)),
    };
    int64_t time_stamp = std::mktime(&given_time);
    state->PushInteger(time_stamp);
  }
  return 1;
}

int32_t Date(const std::shared_ptr<api::LuaState> &state) {
//  auto format = state->OptString(1, "%c");
//
//  return 0;
// TODO
  THROW_NOT_IMPLEMENTED;
}

int32_t Remove(const std::shared_ptr<api::LuaState> &state) {
  auto file_name = state->CheckString(1);
  auto status = std::remove(file_name.c_str());
  if (status) {
    state->PushNil();
    state->PushString(fmt::format("error removing {}: {}", file_name, std::strerror(status)));
    return 2;
  }
  state->PushBoolean(true);
  return 1;
}

int32_t Rename(const std::shared_ptr<api::LuaState> &state) {
  auto old_name = state->CheckString(1);
  auto new_name = state->CheckString(2);
  auto status = std::rename(old_name.c_str(), new_name.c_str());
  if (status) {
    state->PushNil();
    state->PushString(fmt::format("error renaming {} to {}: {}", old_name, new_name, std::strerror(status)));
    return 2;
  }
  state->PushBoolean(true);
  return 1;
}

int32_t TmpName(const std::shared_ptr<api::LuaState> &state) {
//  auto fd = mkstemp(nullptr); // TODO support Windows
//  if (fd == -1) goto error;
//
//  char path[PATH_MAX];
//  if (fcntl(fd, F_GETPATH, path) != -1) {
//    state->PushString(path);
//    return 1;
//  }
//
//  error:
//  state->PushNil();
//  state->PushString(fmt::format("error creating tmp file: {}", std::strerror(errno)));
//  return 2;
  THROW_NOT_IMPLEMENTED;
}

int32_t GetEnv(const std::shared_ptr<api::LuaState> &state) {
  auto key = state->CheckString(1);
  auto env = std::getenv(key.c_str());
  if (env)
    state->PushString(env);
  else
    state->PushNil();
  return 1;
}

int32_t Execute(const std::shared_ptr<api::LuaState> &) {
  // TODO
  THROW_NOT_IMPLEMENTED;
}

int32_t Exit(const std::shared_ptr<api::LuaState> &state) {
  if (state->IsBoolean(1)) {
    if (state->ToBoolean(1))
      std::exit(EXIT_SUCCESS);
    else
      std::exit(EXIT_FAILURE); // TODO
  }

  auto code = state->OptInteger(1, 1);
  std::exit(code);

  // TODO GC stuff
}

int32_t SetLocale(const std::shared_ptr<api::LuaState> &) {
  // TODO
  THROW_NOT_IMPLEMENTED;
}
}
}