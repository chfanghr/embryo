//
// Created by 方泓睿 on 2020/4/2.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_LEXER_LEXER_H_
#define EMBRYO_INCLUDE_EMBRYO_LEXER_LEXER_H_

#include <utility>
#include <regex>
#include <tuple>

#include <fmt/format.h>

#ifdef BUILD_TESTS
#include <gtest/gtest_prod.h>
#endif

#include <embryo/compiler/lexer/token.h>
#include <embryo/utils/formatters.h>
#include <embryo/utils/inspection.h>

namespace embryo::lexer {
class Lexer final {
 public:
  using ScanRes=std::tuple<size_t, embryo::lexer::Token, std::string>;

 private:
  std::string chunk_; // source code
  const std::string chunk_name_; // file name of source code
  size_t line_; // current line number

  ScanRes next_{};

 public:
  Lexer(std::string chunk, std::string chunk_name);

  ScanRes LookAhead();

  ScanRes NextToken();

  ScanRes NextTokenOfKind(Token tok);

  [[nodiscard]] size_t Line() const noexcept;

  [[nodiscard]] std::string ChunkName() const noexcept;
 private:
#ifdef BUILD_TESTS
  FRIEND_TEST(LexerTestGroup, TestScanLongString);
  FRIEND_TEST(LexerTestGroup, TestSkipComment);
  FRIEND_TEST(LexerTestGroup, TestSkipWhiteSpace);
  FRIEND_TEST(LexerTestGroup, TestEscape);
  FRIEND_TEST(LexerTestGroup, TestScanShortString);
  FRIEND_TEST(LexerTestGroup, TestScanNumber);
  FRIEND_TEST(LexerTestGroup, TestScanIdentifier);
#endif

 private:
  void SkipWhiteSpace();

  void SkipComment();

 private:
  bool Test(const std::string &to_test);

  void Next(size_t n);

  std::string Escape(std::string str);
 private:
  std::string ScanLongString();

  std::string ScanShortString();

  std::string Scan(const std::regex &regex);

  std::string ScanNumber();

  std::string ScanIdentifier();
 private:
  [[noreturn]] void Error(const std::string &msg);

 private:
  static bool IsNewLine(char ch);

  static bool IsWhiteSpace(char ch);

  static bool IsDigit(char ch);

  static bool IsLetter(char ch);
 private:
  static std::regex re_opening_long_bracket_;
  static std::regex re_new_line_;
  static std::regex re_short_str_;
  static std::regex re_dec_escape_seq_;
  static std::regex re_hex_escape_seq_;
  static std::regex re_unicode_escape_seq_;
  static std::regex re_number_;
  static std::regex re_identifier_;
};
}

template<>
struct fmt::formatter<embryo::lexer::Lexer::ScanRes> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template<typename FormatContext>
  auto format(const embryo::lexer::Lexer::ScanRes &scan_res, FormatContext &ctx) {
    return format_to(ctx.out(), "{}",
                     embryo::utils::InspectionMessageBuilder(
                         embryo::utils::CppDemangle(typeid(embryo::lexer::Lexer::ScanRes).name()))
                         .Add("line_", std::get<0>(scan_res))
                         .Add("token_", AS_STRING(embryo::lexer::GetTokenName(std::get<1>(scan_res))))
                         .Add("literal_", AS_STRING(std::get<2>(scan_res)))
                         .Result());
  }
};

#endif //EMBRYO_INCLUDE_EMBRYO_LEXER_LEXER_H_
