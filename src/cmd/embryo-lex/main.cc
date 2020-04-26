//
// Created by 方泓睿 on 2020/4/6.
//

#include <vector>
#include <stdexcept>

#include <docopt/docopt.h>
#include <fmt/format.h>

#include <embryo/compiler/parser/parser.h>
#include <embryo/utils/io.h>
#include <embryo/utils/inspection.h>
#include <embryo/version.h>
#include <embryo/utils/backtrace.h>

static const char kUsage[] =
    R"(embryo-lex: lexer of embryo

  Usage:
    embryo-lex INPUT
    embryo-lex (-f | --file) FILE ...
    embryo-lex (-h | --help)
    embryo-lex --version

  Options:
    -h --help   Show this help message.
    -f --file   Send source code from files.
    --version   Show version of embryo.
)";

static int LexFiles(const std::vector<std::string> &file_list);

static int LexInput(const std::string &input);

int main(int argc, const char **argv) {
  return embryo::utils::DebugEnvironment([&]() {
    auto args = docopt::docopt(kUsage,
                               {argv + 1, argv + argc},
                               true, EMBRYO_VERSION);

    if (args["--file"].asBool())
      exit(LexFiles(args["FILE"].asStringList()));

    exit(LexInput(args["INPUT"].asString()));
  });
}

std::string Lex(const std::string &chunk_, const std::string &chunk_name);

int LexFiles(const std::vector<std::string> &file_list) {
  fmt::print("[");
  bool is_first = true;

  for (const auto &file_name:file_list) {
    try {
      auto file_content = embryo::utils::ReadFile(file_name);
      auto lex_res = Lex(file_content, file_name);
      if (is_first) is_first = false; else fmt::print(", ");
      fmt::print("{}", lex_res);
    } catch (const std::runtime_error &error) {
      fmt::print(stderr, "error: {}\n", error.what());
      fmt::print("]");
      return EXIT_FAILURE;
    }
  }

  fmt::print("]");
  return EXIT_SUCCESS;
}

int LexInput(const std::string &input) {
  try {
    fmt::print("{}", Lex(input, "command_line_input"));
  } catch (const std::runtime_error &error) {
    fmt::print(stderr, "error: {}\n", error.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

std::string Lex(const std::string &chunk, const std::string &chunk_name) {
  embryo::utils::InspectionMessageBuilder builder(TYPE_NAME(embryo::lexer::Lexer));
  builder.Add("chunk_name_", AS_STRING(chunk_name));

  embryo::lexer::Lexer lexer(chunk, chunk_name);
  std::vector<embryo::lexer::Lexer::ScanRes> tokens{};

  do
    tokens.emplace_back(lexer.NextToken());
  while (std::get<1>(tokens[tokens.size() - 1]) != embryo::lexer::Token::kEOF);

  builder.Add("tokens_", tokens);
  builder.Add("last_line_", lexer.Line());

  return builder.Result();
}