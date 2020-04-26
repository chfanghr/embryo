//
// Created by 方泓睿 on 2020/4/4.
//

#include <stdexcept>
#include <sstream>

#include <embryo/utils/inspection.h>
#include <embryo/utils/strings.h>
#include <embryo/compiler/lexer/lexer.h>

namespace embryo::lexer {
std::regex Lexer::re_opening_long_bracket_(R"(^\[=*\[)"); // NOLINT(cert-err58-cpp)
std::regex Lexer::re_new_line_(R"(\r\n|\n\r|\n|\r)"); // NOLINT(cert-err58-cpp)
std::regex Lexer::re_short_str_ // NOLINT(cert-err58-cpp)
    (R"((^'(\\\\|\\'|\\\n|\\z\s*|[^'\n])*')|(^"(\\\\|\\"|\\\n|\\z\s*|[^"\n])*"))");
std::regex Lexer::re_dec_escape_seq_(R"(^\\[0-9]{1,3})"); // NOLINT(cert-err58-cpp)
std::regex Lexer::re_hex_escape_seq_(R"(^\\x[0-9a-fA-F]{2})"); // NOLINT(cert-err58-cpp)
std::regex Lexer::re_unicode_escape_seq_(R"(^\\u\{[0-9a-fA-F]+\})"); // NOLINT(cert-err58-cpp)
std::regex Lexer::re_number_ // NOLINT(cert-err58-cpp)
    (R"(^0[xX][0-9a-fA-F]*(\.[0-9a-fA-F]*)?([pP][+\-]?[0-9]+)?|^[0-9]*(\.[0-9]*)?([eE][+\-]?[0-9]+)?)");
std::regex Lexer::re_identifier_(R"(^[_\d\w]+)"); // NOLINT(cert-err58-cpp)

bool Lexer::IsNewLine(char ch) {
  static const auto new_lines = {'\r', '\n'};

  for (const auto &n:new_lines)
    if (ch == n)
      return true;
  return false;
}

bool Lexer::IsWhiteSpace(char ch) {
  static const auto white_spaces = {'\t', '\v', '\f', '\r', ' '};
  for (const auto &w:white_spaces)
    if (ch == w)
      return true;
  return false;
}

bool Lexer::IsDigit(char ch) {
  return '0' <= ch && ch <= '9';
}

bool Lexer::IsLetter(char ch) {
  return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}

void Lexer::Error(const std::string &msg) {
  auto err_msg = fmt::format("{}: (chunk: {}, line {}) {}",
                             THIS_TYPE, chunk_name_, line_, msg);
  throw std::runtime_error(err_msg);
}

std::string Lexer::ScanIdentifier() {
  return Scan(re_identifier_);
}

std::string Lexer::ScanNumber() {
  return Scan(re_number_);
}

std::string Lexer::Scan(const std::regex &regex) {
  std::smatch match_res{};
  if (std::regex_search(chunk_, match_res, regex)) {
    std::string str = match_res[0];
    Next(str.size());
    return str;
  }
  Error("no scan result");
}

std::string Lexer::ScanShortString() {
  std::smatch match_res{};

  if (std::regex_search(chunk_, match_res, re_short_str_)) {
    std::string str = match_res[0];

    Next(str.size());
    str = str.substr(1, str.size() - 2);
    if (str.find('\\') != std::string::npos) {
      std::regex_search(str, match_res, re_new_line_);
      line_ += match_res.size();
      str = Escape(str);
    }

    return str;
  }

  Error("unfinished string");
}

std::string Lexer::ScanLongString() {
  std::smatch match_res{};
  if (!std::regex_search(chunk_, match_res, re_opening_long_bracket_))
    Error(fmt::format("invalid long string delimiter near {}", chunk_.substr(0, 2)));

  std::string opening_long_bracket = match_res[0];
  auto closing_long_bracket = utils::ReplaceAll(opening_long_bracket, '[', ']');
  auto closing_long_bracket_index = chunk_.find(closing_long_bracket);

  if (closing_long_bracket_index == std::string::npos)
    Error("undefined long string or comment");

  auto str = chunk_.substr(opening_long_bracket.size(),
                           closing_long_bracket_index - opening_long_bracket.size());
  Next(closing_long_bracket_index + closing_long_bracket.size());
  str = std::regex_replace(str, re_new_line_, "\n");

  line_ += utils::CountAll(str, "\n");

  if (!str.empty() && str[0] == '\n')
    str = str.substr(1);

  return str;
}

std::string Lexer::Escape(std::string str) {
  std::stringstream ss{};

  while (!str.empty()) {
    if (str[0] != '\\') {
      ss.write(&str[0], sizeof(char));
      str = str.substr(1);
      continue;
    }

    if (str.size() == 1)
      Error("unfinished string");

    std::smatch match_res{};

    switch (str[1]) {
      case 'a':ss << '\a';
        str = str.substr(2);
        continue;
      case 'b':ss << '\b';
        str = str.substr(2);
        continue;
      case 'f':ss << '\f';
        str = str.substr(2);
        continue;
      case 'n':
      case '\n':ss << '\n';
        str = str.substr(2);
        continue;
      case 'r':ss << '\r';
        str = str.substr(2);
        continue;
      case 't':ss << '\t';
        str = str.substr(2);
        continue;
      case 'v':ss << '\v';
        str = str.substr(2);
        continue;
      case '"':ss << '"';
        str = str.substr(2);
        continue;
      case '\'':ss << '\'';
        str = str.substr(2);
        continue;
      case '\\':ss << '\\';
        str = str.substr(2);
        continue;
      case '0' ... '9': // \ddd
        if (std::regex_search(str, match_res, re_dec_escape_seq_)) {
          std::string found = match_res[0];

          int32_t d = std::stoi(found.substr(1));

          if (d <= 0xFF) {
            ss.write((const char *) &d, sizeof(char));
            str = str.substr(found.size());
            continue;
          }

          Error(fmt::format("decimal escape too large near '{}'", found));
        }

        break;

      case 'x': // \xXXX
        if (std::regex_search(str, match_res, re_hex_escape_seq_)) {
          std::string found = match_res[0];

          int32_t d = std::stoi(found.substr(2), nullptr, 16);
          ss.write((const char *) &d, sizeof(char));
          str = str.substr(found.size());
          continue;
        }

        break;

      case 'u': // \u{XXX}
        if (std::regex_search(str, match_res, re_unicode_escape_seq_)) {
          std::string found = match_res[0];

          int32_t d = std::stoi(found.substr(3, found.size() - 4), nullptr, 16);

          if (d <= 0x10FFFF) {
            std::string u8_data = utils::UnicodeToUTF8(d);
            ss.write(u8_data.data(), u8_data.size());
            str = str.substr(found.size());
            continue;
          }

          Error(fmt::format("UTF-8 value too large near '{}'", found));
        }

        break;

      case 'z':str = str.substr(2);
        while (!str.empty() && IsWhiteSpace(str[0]))
          str = str.substr(1);
        continue;
    }

    Error(fmt::format("invalid escape sequence near '\\{}'", str[1]));
  }

  return ss.str();
}

void Lexer::Next(size_t n) {
  chunk_ = chunk_.substr(n);
}

bool Lexer::Test(const std::string &to_test) {
  return utils::HasPrefix(chunk_, to_test);
}

void Lexer::SkipComment() {
  Next(2); // skip --
  if (Test("[") && std::regex_search(chunk_, re_opening_long_bracket_)) { // Is this a long comment?
    ScanLongString(); // long comment
    return;
  }
  // short comment
  while (!chunk_.empty() && !IsNewLine(chunk_[0]))
    Next(1);
}

void Lexer::SkipWhiteSpace() {
  while (!chunk_.empty()) {
    if (Test("--"))
      SkipComment();
    else if (Test("\r\n") || Test("\n\r")) {
      Next(2);
      ++line_;
    } else if (IsNewLine(chunk_[0])) {
      Next(1);
      ++line_;
    } else if (IsWhiteSpace(chunk_[0]))
      Next(1);
    else
      break;
  }
}

Lexer::ScanRes Lexer::NextTokenOfKind(Token tok) {
  auto res = NextToken();
  if (std::get<1>(res) != tok)
    Error(fmt::format("want {}, got {}", GetTokenName(tok), GetTokenName(std::get<1>(res))));
  return res;
}

size_t Lexer::Line() const noexcept { return line_; }

std::string Lexer::ChunkName() const noexcept { return chunk_name_; }

Lexer::ScanRes Lexer::LookAhead() {
  if (std::get<0>(next_) > 0)
    goto ret;

  next_ = NextToken();

  ret:
  return next_;
}

Lexer::ScanRes Lexer::NextToken() {
  if (std::get<0>(next_) > 0) {
    auto res = next_;
    next_ = {};
    return res;
  }

  SkipWhiteSpace();
  if (chunk_.empty())
    return {line_, Token::kEOF, "EOF"};

  switch (chunk_[0]) {
    case ';':Next(1);
      return {line_, Token::kSepSemi, ";"};
    case ',':Next(1);
      return {line_, Token::kSepComma, ","};
    case '(':Next(1);
      return {line_, Token::kSepLParen, "("};
    case ')':Next(1);
      return {line_, Token::kSepRParen, ")"};
    case ']':Next(1);
      return {line_, Token::kSepRBracket, "]"};
    case '{':Next(1);
      return {line_, Token::kSepLCurly, "{"};
    case '}':Next(1);
      return {line_, Token::kSepRCurly, "}"};
    case '+':Next(1);
      return {line_, Token::kOpAdd, "+"};
    case '-':Next(1);
      return {line_, Token::kOpMinus, "-"};
    case '*':Next(1);
      return {line_, Token::kOpMul, "*"};
    case '^':Next(1);
      return {line_, Token::kOpPow, "^"};
    case '%':Next(1);
      return {line_, Token::kOpMod, "%"};
    case '&':Next(1);
      return {line_, Token::kOpBAnd, "&"};
    case '|':Next(1);
      return {line_, Token::kOpBOr, "|"};
    case '#':Next(1);
      return {line_, Token::kOpLen, "#"};
    case ':':
      if (Test("::")) {
        Next(2);
        return {line_, Token::kSepLabel, "::"};
      }
      Next(1);
      return {line_, Token::kSepColon, ":"};
    case '/':
      if (Test("//")) {
        Next(2);
        return {line_, Token::kOpIDiv, "//"};
      }
      Next(1);
      return {line_, Token::kOpDiv, "/"};
    case '~':
      if (Test("~=")) {
        Next(2);
        return {line_, Token::kOpNE, "~="};
      }
      Next(1);
      return {line_, Token::kOpWave, "~"};
    case '=':
      if (Test("==")) {
        Next(2);
        return {line_, Token::kOpEQ, "=="};
      }
      Next(1);
      return {line_, Token::kOpAssign, "="};
    case '<':
      if (Test("<<")) {
        Next(2);
        return {line_, Token::kOpShL, "<<"};
      }

      if (Test("<=")) {
        Next(2);
        return {line_, Token::kOpLE, "<="};
      }

      Next(1);
      return {line_, Token::kOpLT, "<"};
    case '>':
      if (Test(">>")) {
        Next(2);
        return {line_, Token::kOpShR, ">>"};
      }

      if (Test(">=")) {
        Next(2);
        return {line_, Token::kOpGE, ">="};
      }

      Next(1);
      return {line_, Token::kOpGT, ">"};
    case '.':
      if (Test("...")) {
        Next(3);
        return {line_, Token::kVarArg, "..."};
      }

      if (Test("..")) {
        Next(2);
        return {line_, Token::kOpConcat, ".."};
      }

      if (chunk_.size() == 1 || !IsDigit(chunk_[1])) {
        Next(1);
        return {line_, Token::kSepDot, "."};
      }

      break;
    case '[':
      if (Test("[[") || Test("[="))
        return {line_, Token::kString, ScanLongString()};

      Next(1);
      return {line_, Token::kSepLBracket, "["};
    case '\'':
    case '"':return {line_, Token::kString, ScanShortString()};
  }

  if (chunk_[0] == '.' || IsDigit(chunk_[0]))
    return {line_, Token::kNumber, ScanNumber()};

  if (chunk_[0] == '_' || IsLetter(chunk_[0])) {
    auto ident = ScanIdentifier();
    if (kKeywords.find(ident) != kKeywords.end()) {
      auto tok = kKeywords.find(ident)->second;
      return {line_, tok, ident};
    }
    return {line_, Token::kIdentifier, ident};
  }

  Error(fmt::format("unknown symbol near {}", chunk_[0]));
}

Lexer::Lexer(std::string chunk, std::string chunk_name)
    : chunk_(std::move(chunk)), chunk_name_(std::move(chunk_name)), line_(1) {}
}