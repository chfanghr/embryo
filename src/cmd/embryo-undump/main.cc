//
// Created by 方泓睿 on 2020/4/7.
//

#include <functional>

#include <docopt/docopt.h>

#include <embryo/bin_chunk/reader.h>
#include <embryo/utils/formatters.h>
#include <embryo/utils/io.h>
#include <embryo/version.h>
#include <embryo/utils/backtrace.h>
USING_EMBRYO_FORMATTERS();

static const char kUsage[] =
    R"(embryo-undump: dump luac binary chunk

  Usage:
    embryo-undump (-f | --file) [-j | --json] FILE ...
    embryo-undump (-h | --help)
    embryo-undump --version

  Options:
    -j --json       Output undump result as json.
    -h --help       Show this help message.
    -f --file       Send luac binary from files.
    --version       Show version of embryo.
)";

static int UndumpFiles(const std::vector<std::string> &file_list, bool json);

static void FmtPrinter(const std::shared_ptr<embryo::bin_chunk::Prototype> &);

static void PrettyPrinter(const std::shared_ptr<embryo::bin_chunk::Prototype> &);

int main(int argc, const char **argv) {
  return embryo::utils::DebugEnvironment([&]() {
    auto args = docopt::docopt(kUsage,
                               {argv + 1, argv + argc},
                               true, EMBRYO_VERSION);

    exit(UndumpFiles(args["FILE"].asStringList(),
                     args["--json"].asBool()));
  });
}

int UndumpFiles(const std::vector<std::string> &file_list, bool json) {
  if (json)
    fmt::print("[");
  bool is_first = true;

  for (const auto &file_name:file_list) {
    try {
      auto file_content = embryo::utils::ReadFile(file_name);
      auto undump_res = embryo::bin_chunk::Undump(embryo::utils::ReadFile(file_name));
      if (json) {
        if (is_first)
          is_first = false;
        else
          fmt::print(", ");
        FmtPrinter(undump_res);

      } else {
        PrettyPrinter(undump_res);
        fmt::print("\n");
      }
    } catch (const std::runtime_error &error) {
      fmt::print(stderr, "error: {}\n", error.what());
      fmt::print("]");
      return EXIT_FAILURE;
    }
  }

  if (json)
    fmt::print("]");
  return EXIT_SUCCESS;
}

void FmtPrinter(const std::shared_ptr<embryo::bin_chunk::Prototype> &obj) {
  fmt::print("{}", obj);
}

void PrettyPrinter(const std::shared_ptr<embryo::bin_chunk::Prototype> &obj) {
  obj->PrettyPrint();
}

