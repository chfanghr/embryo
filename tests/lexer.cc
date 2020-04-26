//
// Created by 方泓睿 on 2020/4/2.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <map>
#include <vector>

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <embryo/lexer/lexer.h>

namespace embryo::lexer {
class LexerTestGroup : public testing::Test {};

TEST_F(LexerTestGroup, TestScanLongString) {
  std::map<std::string, std::string> cases{
      {R"([[alo
123"]])", "alo\n123\""},
      {
          R"([==[alo
123"]==])", "alo\n123\""
      }
  };

  for (const auto &[key, val]:cases)
    ASSERT_NO_THROW({
                      Lexer lexer(key, "");
                      auto res = lexer.ScanLongString();
                      ASSERT_TRUE(res == val)
                          << fmt::format("case: {}, expected: {}, result: {}", key, val, res);
                    }) << fmt::format("case: {}, expected: {}", key, val);
}

TEST_F(LexerTestGroup, TestSkipComment) {
  std::vector<std::string> cases{
      "-- short comment",
      "--> short comment",
      "--[[long comment]]",
      R"(--[===[long comment
another
long comment
]===])"
  };

  for (const auto &c:cases) {
    Lexer lexer(c, "");
    lexer.SkipComment();

    auto msg = fmt::format("case: {}", c);
    ASSERT_NO_THROW({
                      ASSERT_TRUE(lexer.chunk_.empty()) << msg;
                    }) << msg;
  }
}

TEST_F(LexerTestGroup, TestSkipWhiteSpace) {
  std::vector<std::string> cases{
      "   ",
      " ",
      "\r\n\r",
      "\n\n\r",
      "\r\r",
      "\n\n\n"
  };

  for (const auto &c:cases) {
    Lexer lexer(c, "");
    lexer.SkipWhiteSpace();

    auto msg = fmt::format("case: {}", c);
    ASSERT_NO_THROW({
                      ASSERT_TRUE(lexer.chunk_.empty())
                          << msg;
                    }) << msg;
  }
}

TEST_F(LexerTestGroup, TestEscape) {
  std::map<std::string, std::string> cases{
      {R"(\a)", "\a"},
      {R"(\b)", "\b"},
      {R"(\f)", "\f"},
      {R"(\n)", "\n"},
      {"\n", "\n"},
      {R"(\r)", "\r"},
      {R"(\t)", "\t"},
      {R"(\v)", "\v"},
      {R"(")", "\""},
      {R"(')", "\'"},
      {R"(')", "'"},
      {R"(\\)", "\\"},
      {R"(\050)", "2"},
      {R"(\x61)", "a"},
      {R"(\u{1F4a9})", "💩"},
      {R"(\u{5965}\u{5229}\u{7ed9}\u{5e72}\u{4e86}\u{5144}\u{5f1f}\u{4eec})", "奥利给干了兄弟们"},
      {R"(\vAWSL)", "\vAWSL"},
      {R"(\x61WSL)", "aWSL"},
      {R"(\u{7c73}\u{5ffd}\u{60a0}\n\045\x2D\x2d\u{6c38}\u{8fdc}\u{6ef4}\u{795e})", "米忽悠\n---永远滴神"},
      {R"(\z)", ""},
      {R"(\z    )", ""},
  };

  Lexer lexer("", "");

  for (const auto &[key, val]:cases)
    ASSERT_NO_THROW({
                      auto res = lexer.Escape(key);
                      ASSERT_TRUE(res == val)
                          << fmt::format("case: {}, expected: {}, result: {}", key, val, res);
                    }) << fmt::format("case: {}, expected: {}", key, val);
}

TEST_F(LexerTestGroup, TestScanShortString) {
  std::map<std::string, std::string> cases{
      {R"('alo\n123"')", "alo\n123\""},
      {R"("alo\n123\"")", "alo\n123\""},
      {R"('\97lo\10\04923"')", "alo\n123\""}
  };

  for (const auto &[key, val]:cases)
    ASSERT_NO_THROW({
                      Lexer lexer(key, "");
                      auto res = lexer.ScanShortString();
                      ASSERT_TRUE(res == val)
                          << fmt::format("case: {}, expected: {}, result: {}", key, val, res);
                    }) << fmt::format("case: {}, expected: {}", key, val);
}

TEST_F(LexerTestGroup, TestScanNumber) {
  std::map<std::string, std::string> cases{
      // integer constants
      {R"(3)", "3"},
      {R"(345)", "345"},
      {R"(0xff)", "0xff"},
      {R"(0xBEBADA)", "0xBEBADA"},

      // float constants
      {R"(3.0)", "3.0"},
      {R"(3.1416)", "3.1416"},
      {R"(314.16e-2)", "314.16e-2"},
      {R"(0.31416E1)", "0.31416E1"},
      {R"(34e1)", "34e1"},
      {R"(0x0.1E)", "0x0.1E"},
      {R"(0xA23p-4)", "0xA23p-4"},
      {R"(0X1.921FB54442D18P+1)", "0X1.921FB54442D18P+1"}
  };

  for (const auto &[key, val]:cases)
    ASSERT_NO_THROW({
                      Lexer lexer(key, "");
                      auto res = lexer.ScanNumber();
                      ASSERT_TRUE(res == val)
                          << fmt::format("case: {}, expected: {}, result: {}", key, val, res);
                    }) << fmt::format("case: {}, expected: {}", key, val);
}

TEST_F(LexerTestGroup, TestScanIdentifier) {
  std::map<std::string, std::string> cases{
      {R"(a)", "a"},
      {R"(and)", "and"},
      {R"(_ver)", "_ver"},
      {R"(b_)", "b_"},
      {R"(__b)", "__b"},
      {R"(b3)", "b3"},
      {R"(B_3)", "B_3"}
  };

  for (const auto &[key, val]:cases)
    ASSERT_NO_THROW({
                      Lexer lexer(key, "");
                      auto res = lexer.ScanIdentifier();
                      ASSERT_TRUE(res == val)
                          << fmt::format("case: {}, expected: {}, result: {}", key, val, res);
                    }) << fmt::format("case: {}, expected: {}", key, val);
}

TEST_F(LexerTestGroup, TestNextToken) {
  std::map<std::string, std::vector<embryo::lexer::Lexer::ScanRes>> cases{
      {"", {{1, Token::kEOF, "EOF"}}},
      {"[;,]+*(({))}){-^%&|#", {
          {1, Token::kSepLBracket, "["},
          {1, Token::kSepSemi, ";"},
          {1, Token::kSepComma, ","},
          {1, Token::kSepRBracket, "]"},
          {1, Token::kOpAdd, "+"},
          {1, Token::kOpMul, "*"},
          {1, Token::kSepLParen, "("},
          {1, Token::kSepLParen, "("},
          {1, Token::kSepLCurly, "{"},
          {1, Token::kSepRParen, ")"},
          {1, Token::kSepRParen, ")"},
          {1, Token::kSepRCurly, "}"},
          {1, Token::kSepRParen, ")"},
          {1, Token::kSepLCurly, "{"},
          {1, Token::kOpMinus, "-"},
          {1, Token::kOpPow, "^"},
          {1, Token::kOpMod, "%"},
          {1, Token::kOpBAnd, "&"},
          {1, Token::kOpBOr, "|"},
          {1, Token::kOpLen, "#"},
          {1, Token::kEOF, "EOF"}
      }},
      {
          "a==b,a~=b", {
          {1, Token::kIdentifier, "a"},
          {1, Token::kOpEQ, "=="},
          {1, Token::kIdentifier, "b"},
          {1, Token::kSepComma, ","},
          {1, Token::kIdentifier, "a"},
          {1, Token::kOpNE, "~="},
          {1, Token::kIdentifier, "b"},
          {1, Token::kEOF, "EOF"}
      }},
      {
          "~a//<b<=/b<<>=>>a.b..c...", {
          {1, Token::kOpWave, "~"},
          {1, Token::kIdentifier, "a"},
          {1, Token::kOpIDiv, "//"},
          {1, Token::kOpLT, "<"},
          {1, Token::kIdentifier, "b"},
          {1, Token::kOpLE, "<="},
          {1, Token::kOpDiv, "/"},
          {1, Token::kIdentifier, "b"},
          {1, Token::kOpShL, "<<"},
          {1, Token::kOpGE, ">="},
          {1, Token::kOpShR, ">>"},
          {1, Token::kIdentifier, "a"},
          {1, Token::kSepDot, "."},
          {1, Token::kIdentifier, "b"},
          {1, Token::kOpConcat, ".."},
          {1, Token::kIdentifier, "c"},
          {1, Token::kVarArg, "..."},
          {1, Token::kEOF, "EOF"}
      }},
      {
          ".b.314_a/114e514do", {
          {1, Token::kSepDot, "."},
          {1, Token::kIdentifier, "b"},
          {1, Token::kNumber, ".314"},
          {1, Token::kIdentifier, "_a"},
          {1, Token::kOpDiv, "/"},
          {1, Token::kNumber, "114e514"},
          {1, Token::kKwDo, "do"},
          {1, Token::kEOF, "EOF"}
      }},
      {R"("aa\n\n\naa".3[==[dsdsasd]==])", {
          {1, Token::kString, "aa\n\n\naa"},
          {1, Token::kNumber, ".3"},
          {1, Token::kString, "dsdsasd"},
          {1, Token::kEOF, "EOF"}
      }}
  };

  for (const auto &[c, exp]:cases)
    ASSERT_NO_THROW({
                      Lexer lexer(c, "");
                      embryo::lexer::Lexer::ScanRes res{};
                      size_t idx = 0;

                      do {
                        ASSERT_TRUE(idx < exp.size());
                        res = lexer.NextToken();
                        ASSERT_TRUE(res == exp[idx])
                            << fmt::format("case: {}, expected: {}, result: {}", c, exp[idx], res);
                        ++idx;
                      } while (std::get<1>(res) != Token::kEOF);
                    }) << fmt::format("case: {}, expected: {}", c, exp);
}

TEST_F(LexerTestGroup, TestLookAhead) {
  std::map<std::string, std::vector<embryo::lexer::Lexer::ScanRes>> cases{
      {"", {{1, Token::kEOF, "EOF"}}},
      {"[;,]+*(({))}){-^%&|#", {
          {1, Token::kSepLBracket, "["},
          {1, Token::kSepSemi, ";"},
          {1, Token::kSepComma, ","},
          {1, Token::kSepRBracket, "]"},
          {1, Token::kOpAdd, "+"},
          {1, Token::kOpMul, "*"},
          {1, Token::kSepLParen, "("},
          {1, Token::kSepLParen, "("},
          {1, Token::kSepLCurly, "{"},
          {1, Token::kSepRParen, ")"},
          {1, Token::kSepRParen, ")"},
          {1, Token::kSepRCurly, "}"},
          {1, Token::kSepRParen, ")"},
          {1, Token::kSepLCurly, "{"},
          {1, Token::kOpMinus, "-"},
          {1, Token::kOpPow, "^"},
          {1, Token::kOpMod, "%"},
          {1, Token::kOpBAnd, "&"},
          {1, Token::kOpBOr, "|"},
          {1, Token::kOpLen, "#"},
          {1, Token::kEOF, "EOF"}
      }},
      {
          "a==b,a~=b", {
          {1, Token::kIdentifier, "a"},
          {1, Token::kOpEQ, "=="},
          {1, Token::kIdentifier, "b"},
          {1, Token::kSepComma, ","},
          {1, Token::kIdentifier, "a"},
          {1, Token::kOpNE, "~="},
          {1, Token::kIdentifier, "b"},
          {1, Token::kEOF, "EOF"}
      }},
      {
          "~a//<b<=/b<<>=>>a.b..c...", {
          {1, Token::kOpWave, "~"},
          {1, Token::kIdentifier, "a"},
          {1, Token::kOpIDiv, "//"},
          {1, Token::kOpLT, "<"},
          {1, Token::kIdentifier, "b"},
          {1, Token::kOpLE, "<="},
          {1, Token::kOpDiv, "/"},
          {1, Token::kIdentifier, "b"},
          {1, Token::kOpShL, "<<"},
          {1, Token::kOpGE, ">="},
          {1, Token::kOpShR, ">>"},
          {1, Token::kIdentifier, "a"},
          {1, Token::kSepDot, "."},
          {1, Token::kIdentifier, "b"},
          {1, Token::kOpConcat, ".."},
          {1, Token::kIdentifier, "c"},
          {1, Token::kVarArg, "..."},
          {1, Token::kEOF, "EOF"}
      }},
      {
          ".b.314_a/114e514do", {
          {1, Token::kSepDot, "."},
          {1, Token::kIdentifier, "b"},
          {1, Token::kNumber, ".314"},
          {1, Token::kIdentifier, "_a"},
          {1, Token::kOpDiv, "/"},
          {1, Token::kNumber, "114e514"},
          {1, Token::kKwDo, "do"},
          {1, Token::kEOF, "EOF"}
      }},
      {R"("aa\n\n\naa".3[==[dsdsasd]==])", {
          {1, Token::kString, "aa\n\n\naa"},
          {1, Token::kNumber, ".3"},
          {1, Token::kString, "dsdsasd"},
          {1, Token::kEOF, "EOF"}
      }}
  };

  for (const auto &[c, exp]:cases)
    ASSERT_NO_THROW({
                      Lexer lexer(c, "");
                      embryo::lexer::Lexer::ScanRes res{};
                      size_t idx = 0;

                      do {
                        ASSERT_TRUE(idx < exp.size());
                        res = lexer.LookAhead();
                        ASSERT_TRUE(res == exp[idx])
                            << fmt::format("case: {}, expected: {}, result: {}", c, exp[idx], res);
                        res = lexer.NextToken();
                        ASSERT_TRUE(res == exp[idx])
                            << fmt::format("case: {}, expected: {}, result: {}", c, exp[idx], res);
                        ++idx;
                      } while (std::get<1>(res) != Token::kEOF);
                    }) << fmt::format("case: {}, expected: {}", c, exp);
}
}

#pragma clang diagnostic pop