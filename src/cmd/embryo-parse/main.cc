//
// Created by 方泓睿 on 2020/4/6.
//

#include <vector>
#include <stdexcept>

#include <docopt/docopt.h>
#include <fmt/format.h>

#include <embryo/compiler/parser/parser.h>
#include <embryo/utils/io.h>
#include <embryo/version.h>
#include <embryo/utils/backtrace.h>

static const char kUsage[] =
    R"(embryo-parse: parser of embryo

  Usage:
    embryo-parse [--no-optimize] INPUT
    embryo-parse [--no-optimize] (-f | --file) FILE ...
    embryo-parse (-h | --help)
    embryo-parse --version

  Options:
    --no-optimize   Turn off the optimizer.
    -h --help       Show this help message.
    -f --file       Send source code from files.
    --version       Show version of embryo.
)";

static int ParseFiles(const std::vector<std::string> &file_list, bool optimize);

static int ParseInput(const std::string &input, bool optimize);

int main(int argc, const char **argv) {
  return embryo::utils::DebugEnvironment([&]() {
    auto args = docopt::docopt(kUsage,
                               {argv + 1, argv + argc},
                               true, EMBRYO_VERSION);

    auto optimize = !args["--no-optimize"].asBool();

    if (args["--file"].asBool())
      exit(ParseFiles(args["FILE"].asStringList(), optimize));

    exit(ParseInput(args["INPUT"].asString(), optimize));
  });
}

int ParseFiles(const std::vector<std::string> &file_list, bool optimize) {
  fmt::print("[");
  bool is_first = true;

  for (const auto &file_name:file_list) {
    try {
      auto file_content = embryo::utils::ReadFile(file_name);
      embryo::parser::Parser parser(file_content, file_name, optimize);

      auto parse_res = parser.Parse();

      if (is_first) is_first = false; else fmt::print(", ");
      fmt::print("{}", parse_res);
    } catch (const std::runtime_error &error) {
      fmt::print(stderr, "error: {}\n", error.what());
      fmt::print("]");
      return EXIT_FAILURE;
    }
  }

  fmt::print("]");
  return EXIT_SUCCESS;
}

int ParseInput(const std::string &input, bool optimize) {
  try {
    embryo::parser::Parser parser(input, "command_line_input", optimize);
    auto parse_res = parser.Parse();
    fmt::print("{}", parse_res);
  } catch (const std::runtime_error &error) {
    fmt::print(stderr, "error: {}\n", error.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}