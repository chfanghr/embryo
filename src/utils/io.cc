//
// Created by 方泓睿 on 2020/4/6.
//

#include <fstream>
#include <streambuf>

#include <fmt/format.h>

#include <embryo/utils/inspection.h>
#include <embryo/utils/io.h>

namespace embryo::utils {
template<>
std::string ReadFile<std::string>(const std::string &file_name) {
  std::ifstream fs(file_name);
  if (!fs)
    throw std::runtime_error(
        MSG_WITH_FUNC_NAME(fmt::format("failed to open {}: {}\n", file_name, strerror(errno))));

  return std::string((std::istreambuf_iterator<char>(fs)),
                     std::istreambuf_iterator<char>());
}

template<>
std::vector<Byte> ReadFile<std::vector<Byte>>(const std::string &file_name) {
  std::ifstream fs(file_name);
  if (!fs)
    throw std::runtime_error(
        MSG_WITH_FUNC_NAME(fmt::format("failed to open {}: {}\n", file_name, strerror(errno))));

  return std::vector<Byte>((std::istreambuf_iterator<char>(fs)),
                           std::istreambuf_iterator<char>());
}

template<>
void WriteFile<std::string>(const std::string &data, const std::string &file_name) {
  std::ofstream fs(file_name);
  if (!fs)
    throw std::runtime_error(
        MSG_WITH_FUNC_NAME(fmt::format("failed to open {}: {}\n", file_name, strerror(errno))));

  std::copy(data.begin(), data.end(), std::ostream_iterator<char>(fs));
}

template<>
void WriteFile<std::vector<Byte>>(const std::vector<Byte> &data, const std::string &file_name) {
  std::ofstream fs(file_name);
  if (!fs)
    throw std::runtime_error(
        MSG_WITH_FUNC_NAME(fmt::format("failed to open {}: {}\n", file_name, strerror(errno))));

  std::copy(data.begin(), data.end(), std::ostream_iterator<char>(fs));
}
}