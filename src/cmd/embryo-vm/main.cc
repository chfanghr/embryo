//
// Created by 方泓睿 on 2020/4/11.
//

#include <docopt/docopt.h>

#include <embryo/state/lua_state.h>
#include <embryo/utils/backtrace.h>
#include <embryo/utils/io.h>
#include <embryo/version.h>

static const char kUsage[] =
    R"(embryo-vm: execute luac binary chunks

    Usage:
      embryo-vm (-f | --file) FILE ...
      embryo-vm (-h | --help)
      embryo-vm --version

    Options:
      -h --help       Show this help message.
      -f --file       Binary chunks to be executed.
      --version       Show version of embryo.
)";

int main(int argc, const char **argv) {
  return embryo::utils::DebugEnvironment([&] {
    auto vm = embryo::state::LuaState::New();

    auto args = docopt::docopt(kUsage,
                               {argv + 1, argv + argc},
                               true, EMBRYO_VERSION);

    auto files = args["FILE"].asStringList();
    for (const auto &file:files) {
      vm->Load( // TODO
          embryo::utils::ReadFile<std::vector<embryo::Byte>>(file),
          file, "b");
    }

    vm->Call(0, -1);
  });
}