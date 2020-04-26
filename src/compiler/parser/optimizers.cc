//
// Created by 方泓睿 on 2020/4/6.
//

#include <memory>

#include <embryo/compiler/ast/exp.h>
#include <embryo/utils/numbers.h>

namespace embryo::parser {
static bool IsTrue(const std::shared_ptr<ast::Exp> &in);

static bool IsFalse(const std::shared_ptr<ast::Exp> &in);

static bool IsVarargOrFuncCall(const std::shared_ptr<ast::Exp> &in);

static std::tuple<int64_t, bool> CastToInt(const std::shared_ptr<ast::Exp> &in);

static std::tuple<double, bool> CastToFloat(const std::shared_ptr<ast::Exp> &in);

static std::shared_ptr<ast::Exp> UnmOptimizer(const std::shared_ptr<ast::Exp> &in);

static std::shared_ptr<ast::Exp> NotOptimizer(const std::shared_ptr<ast::Exp> &in);

static std::shared_ptr<ast::Exp> BNotOptimizer(const std::shared_ptr<ast::Exp> &in);

std::shared_ptr<ast::Exp> LogicalOrOptimizer(const std::shared_ptr<ast::Exp> &in) {
  auto exp = std::dynamic_pointer_cast<ast::BinOpExp>(in);
  if (!exp)
    return in;
  if (IsTrue(exp->exp_1_))
    return exp->exp_1_;
  if (IsFalse(exp->exp_1_) && !IsVarargOrFuncCall(exp->exp_2_))
    return exp->exp_2_;
  return exp;
}

std::shared_ptr<ast::Exp> LogicalAndOptimizer(const std::shared_ptr<ast::Exp> &in) {
  auto exp = std::dynamic_pointer_cast<ast::BinOpExp>(in);
  if (!exp)
    return in;
  if (IsFalse(exp->exp_1_))
    return exp->exp_1_;
  if (IsTrue(exp->exp_1_) && !IsVarargOrFuncCall(exp->exp_2_))
    return exp->exp_2_;
  return exp;
}

std::shared_ptr<ast::Exp> BitWiseBinOpOptimizer(const std::shared_ptr<ast::Exp> &in) {
  auto exp = std::dynamic_pointer_cast<ast::BinOpExp>(in);
  if (!exp)
    return in;
  auto exp_1 = exp->exp_1_;
  auto[exp_1_i, ok_1] = CastToInt(exp_1);
  auto exp_2 = exp->exp_2_;
  auto[exp_2_i, ok_2] = CastToInt(exp_2);

  if (!(ok_1 || ok_2))
    return exp;

  if (ok_1 && ok_2) {
    auto make_res = [&](int64_t val) {
      return std::dynamic_pointer_cast<ast::Exp>(
          std::make_shared<ast::IntegerExp>(exp->line_, val));
    };

    switch (exp->op_) {
      case lexer::Token::kOpBAnd:return make_res(exp_1_i & exp_2_i); // NOLINT(hicpp-signed-bitwise)
      case lexer::Token::kOpBOr:return make_res(exp_1_i | exp_2_i); // NOLINT(hicpp-signed-bitwise)
      case lexer::kOpBXor:return make_res(exp_1_i ^ exp_2_i); // NOLINT(hicpp-signed-bitwise)
      case lexer::Token::kOpShL:return make_res(utils::ShiftLeft(exp_1_i, exp_2_i));
      case lexer::Token::kOpShR:return make_res(utils::ShiftRight(exp_1_i, exp_2_i));
      default: return in;
    }
  }

  if (ok_1)
    exp_1 = std::dynamic_pointer_cast<ast::Exp>(
        std::make_shared<ast::IntegerExp>(exp->line_, exp_1_i));

  if (ok_2)
    exp_2 = std::dynamic_pointer_cast<ast::Exp>(
        std::make_shared<ast::IntegerExp>(exp->line_, exp_2_i));

  return std::dynamic_pointer_cast<ast::Exp>(
      std::make_shared<ast::BinOpExp>(exp->line_, exp->op_, exp_1, exp_2));
}

std::shared_ptr<ast::Exp> ArithBinaryOpOptimizer(const std::shared_ptr<ast::Exp> &in) {
  auto exp = std::dynamic_pointer_cast<ast::BinOpExp>(in);
  if (!exp)
    return in;

  {
    auto x = std::dynamic_pointer_cast<ast::IntegerExp>(exp->exp_1_);
    auto y = std::dynamic_pointer_cast<ast::IntegerExp>(exp->exp_2_);

    if (x && y) {
      auto make_res = [&](int64_t val) {
        return std::dynamic_pointer_cast<ast::Exp>(
            std::make_shared<ast::IntegerExp>(exp->line_, val));
      };

      auto check_y_not_zero = [&]() {
        if (y->val_ == 0)
          throw std::runtime_error("cannot divide zero");
      };

      switch (exp->op_) {
        case lexer::Token::kOpAdd:return make_res(x->val_ + y->val_);
        case lexer::kOpSub:return make_res(x->val_ - y->val_);
        case lexer::Token::kOpMul:return make_res(x->val_ * y->val_);
        case lexer::Token::kOpDiv:check_y_not_zero();
          return make_res(x->val_ / y->val_);
        case lexer::Token::kOpIDiv:check_y_not_zero();
          return make_res(utils::FloorDiv(x->val_, y->val_));
        case lexer::Token::kOpMod:check_y_not_zero();
          return make_res(utils::Mod(x->val_, y->val_));
        default:;
      }
    }
  }

  auto[x, ok_x] =CastToFloat(exp->exp_1_);
  auto[y, ok_y] =CastToFloat(exp->exp_2_);

  if (ok_x && ok_y) {
    auto make_res = [&](int64_t val) {
      return std::dynamic_pointer_cast<ast::Exp>(
          std::make_shared<ast::FloatExp>(exp->line_, val));
    };
    auto check_y_not_zero = [&](double y) {
      if (y == 0)
        throw std::runtime_error("cannot divide zero");
    };
    switch (exp->op_) {
      case lexer::Token::kOpAdd:return make_res(x + y);
      case lexer::kOpSub:return make_res(x - y);
      case lexer::Token::kOpMul:return make_res(x * y);
      case lexer::Token::kOpDiv:check_y_not_zero(y);
        return make_res(x / y);
      case lexer::Token::kOpIDiv:check_y_not_zero(y);
        return make_res(utils::FloorDiv(x, y));
      case lexer::Token::kOpMod:check_y_not_zero(y);
        return make_res(utils::Mod(x, y));
      default:;
    }
  }

  return in;
}

std::shared_ptr<ast::Exp> PowOptimizer(const std::shared_ptr<ast::Exp> &in) {
  auto exp = std::dynamic_pointer_cast<ast::BinOpExp>(in);
  if (!exp)
    return in;

  if (exp->op_ == lexer::Token::kOpPow)
    exp->exp_2_ = PowOptimizer(exp->exp_2_);

  return ArithBinaryOpOptimizer(std::dynamic_pointer_cast<ast::Exp>(exp));
}

std::shared_ptr<ast::Exp> UnaryOpOptimizer(const std::shared_ptr<ast::Exp> &in) {
  auto exp = std::dynamic_pointer_cast<ast::UnOpExp>(in);
  if (!exp)
    return in;

  switch (exp->op_) {
    case lexer::kOpUnM:return UnmOptimizer(exp);
    case lexer::Token::kOpNot:return NotOptimizer(exp);
    case lexer::kOpBNot:return BNotOptimizer(exp);
    default:return in;
  }
}

bool IsTrue(const std::shared_ptr<ast::Exp> &in) {
  return std::dynamic_pointer_cast<ast::TrueExp>(in) ||
      std::dynamic_pointer_cast<ast::IntegerExp>(in) ||
      std::dynamic_pointer_cast<ast::FloatExp>(in) ||
      std::dynamic_pointer_cast<ast::StringExp>(in);
}

bool IsFalse(const std::shared_ptr<ast::Exp> &in) {
  return std::dynamic_pointer_cast<ast::FalseExp>(in) ||
      std::dynamic_pointer_cast<ast::NilExp>(in);
}

bool IsVarargOrFuncCall(const std::shared_ptr<ast::Exp> &in) {
  return std::dynamic_pointer_cast<ast::VarArgExp>(in) ||
      std::dynamic_pointer_cast<ast::FuncCallExp>(in);
}

std::tuple<int64_t, bool> CastToInt(const std::shared_ptr<ast::Exp> &in) {
  if (std::dynamic_pointer_cast<ast::IntegerExp>(in))
    return {std::dynamic_pointer_cast<ast::IntegerExp>(in)->val_, true};
  if (std::dynamic_pointer_cast<ast::FloatExp>(in))
    return {int64_t(std::dynamic_pointer_cast<ast::FloatExp>(in)->val_), true};
  return {0, false};
}

static std::tuple<double, bool> CastToFloat(const std::shared_ptr<ast::Exp> &in) {
  if (std::dynamic_pointer_cast<ast::FloatExp>(in))
    return {std::dynamic_pointer_cast<ast::FloatExp>(in)->val_, true};
  if (std::dynamic_pointer_cast<ast::IntegerExp>(in))
    return {double(std::dynamic_pointer_cast<ast::IntegerExp>(in)->val_), true};
  return {0, false};
}

static std::shared_ptr<ast::Exp> UnmOptimizer(const std::shared_ptr<ast::Exp> &in) {
  auto exp = std::dynamic_pointer_cast<ast::UnOpExp>(in);

  {
    auto x = std::dynamic_pointer_cast<ast::IntegerExp>(exp->exp_);
    if (x) {
      x->val_ = -x->val_;
      return std::dynamic_pointer_cast<ast::Exp>(x);
    }
  }

  {
    auto x = std::dynamic_pointer_cast<ast::FloatExp>(exp->exp_);
    if (x) {
      x->val_ = -x->val_;
      return std::dynamic_pointer_cast<ast::Exp>(x);
    }
  }

  return in;
}

static std::shared_ptr<ast::Exp> NotOptimizer(const std::shared_ptr<ast::Exp> &in) {
  auto exp = std::dynamic_pointer_cast<ast::UnOpExp>(in);
  if (IsFalse(exp->exp_))
    return std::make_shared<ast::TrueExp>(exp->line_);
  if (IsTrue(exp->exp_))
    return std::make_shared<ast::FalseExp>(exp->line_);
  return in;
}

static std::shared_ptr<ast::Exp> BNotOptimizer(const std::shared_ptr<ast::Exp> &in) {
  auto exp = std::dynamic_pointer_cast<ast::UnOpExp>(in);

  {
    auto x = std::dynamic_pointer_cast<ast::IntegerExp>(exp->exp_);
    if (x) {
      x->val_ = ~x->val_; // NOLINT(hicpp-signed-bitwise)
      return std::dynamic_pointer_cast<ast::Exp>(x);
    }
  }

  {
    auto x = std::dynamic_pointer_cast<ast::FloatExp>(exp->exp_);
    if (x) {
      return std::dynamic_pointer_cast<ast::Exp>(
          std::make_shared<ast::IntegerExp>(exp->line_, ~int64_t(x->val_))); // NOLINT(hicpp-signed-bitwise)
    }
  }

  return in;
}
}