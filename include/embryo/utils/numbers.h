//
// Created by 方泓睿 on 2020/4/5.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_UTILS_NUMBERS_H_
#define EMBRYO_INCLUDE_EMBRYO_UTILS_NUMBERS_H_

#include <cstdint>
#include <string>
#include <tuple>

namespace embryo::utils {
std::tuple<int64_t, bool> ParseInt(std::string lit);

std::tuple<double, bool> ParseFloat(std::string lit);

int64_t ShiftLeft(int64_t i, int64_t j);

int64_t ShiftRight(int64_t i, int64_t j);

int64_t FloorDiv(int64_t a, int64_t b);

int64_t Mod(int64_t a, int64_t b);

double FloorDiv(double a, double b);

double Mod(double a, double b);

std::tuple<int64_t, bool> FloatToInteger(double f);

std::tuple<double, double> ModF(double f);
}
#endif //EMBRYO_INCLUDE_EMBRYO_UTILS_NUMBERS_H_
