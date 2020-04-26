//
// Created by 方泓睿 on 2020/4/6.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_UTILS_IO_H_
#define EMBRYO_INCLUDE_EMBRYO_UTILS_IO_H_

#include <string>
#include <vector>
#include <embryo/utils/types.h>

namespace embryo::utils {
template<typename T=std::string>
T ReadFile(const std::string &file_name);

template<>
std::string ReadFile<std::string>(
    const std::string &file_name);

template<>
std::vector<Byte> ReadFile<std::vector<Byte>>(
    const std::string &file_name);

template<typename T=std::string>
void WriteFile(const T &data, const std::string &file_name);

template<>
void WriteFile<std::string>(const std::string &data, const std::string &file_name);

template<>
void WriteFile<std::vector<Byte>>(const std::vector<Byte> &data, const std::string &file_name);
}
#endif //EMBRYO_INCLUDE_EMBRYO_UTILS_IO_H_
