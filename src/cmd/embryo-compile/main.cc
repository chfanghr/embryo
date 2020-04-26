//
// Created by 方泓睿 on 2020/4/18.
//

#include <docopt/docopt.h>
#include <fmt/format.h>

#include <embryo/utils/backtrace.h>
#include <embryo/compiler/compiler.h>
#include <embryo/bin_chunk/writer.h>
#include <embryo/version.h>

static const char kUsage[] =
    R"(embryo-compile: compile lua source to binary chunk

  Usage:
    embryo-compile [options] IN
    embryo-compile (-h | --help)
    embryo-compile --version

  Options:
    -o, --output OUT      Output binary chunk path.
    -l, --list            List symbols in the generated binary chunk.
    -h, --help            Show this help message.
    --version             Show version of embryo.
)";

int main(int argc, const char **argv) {
  return embryo::utils::DebugEnvironment([&]() {
    auto args = docopt::docopt(kUsage,
                               {argv + 1, argv + argc},
                               true, EMBRYO_VERSION);

    auto proto = embryo::compiler::Compile(args["IN"].asString());
    auto out = args["--output"] ? args["--output"].asString() : "embryoc.out";
    embryo::bin_chunk::Dump(proto, out);
    if (args["--list"])
      proto->PrettyPrint();
    fmt::print("\n");
  });
}