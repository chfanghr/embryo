//
// Created by 方泓睿 on 2020/4/6.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_UTILS_INSPECTION_H_
#define EMBRYO_INCLUDE_EMBRYO_UTILS_INSPECTION_H_

#include <string>
#include <vector>
#include <typeinfo>
#include <stdexcept>

#include <fmt/format.h>

namespace embryo::utils {
std::string CppDemangle(const char *abi_name);
}

#define KV(FIELD) #FIELD, FIELD
#define DEMANGLE(TYPE_NAME) embryo::utils::CppDemangle( TYPE_NAME)
#define TYPE_NAME(TYPE) DEMANGLE(typeid( TYPE).name())
#define THIS_TYPE embryo::utils::CppDemangle(typeid(*this).name())
#define INSPECT_THIS embryo::utils::InspectionMessageBuilder(THIS_TYPE)
#define AS_STRING(STR) fmt::format(R"("{}")", STR)
#define AS_STRINGS(STR_VEC) embryo::utils::JsonStrings( STR_VEC)
#define KV_AS_STRING(FIELD) #FIELD, AS_STRING( FIELD )
#define KV_AS_STRINGS(FIELD) #FIELD, AS_STRINGS( FIELD )
#define THROW_NOT_IMPLEMENTED do {throw std::runtime_error(MSG_WITH_FUNC_NAME("not implemented")); }while(0)

#ifdef DEBUG
#define MSG_WITH_FUNC_NAME(MSG) fmt::format("{}, line {}: {}: {}", __FILE__, __LINE__, embryo::utils::CppDemangle(__FUNCTION__),(MSG))
#else
#define MSG_WITH_FUNC_NAME(MSG) fmt::format("{}: {}", __FUNCTION__,(MSG))
#endif

namespace embryo::utils {
std::vector<std::string> JsonStrings(const std::vector<std::string> &);

template<bool use_json = true>
class InspectionMessageBuilder {
 private:
  const std::string type_name_{};
  std::string msg_{};
 public:
  explicit InspectionMessageBuilder(std::string type_name = "")
      : type_name_(std::move(type_name)),
        msg_("{" + fmt::format(R"("type_name_": {})", AS_STRING(type_name_))) {}

  template<typename K, typename V, typename std::enable_if_t<use_json> * = nullptr>
  InspectionMessageBuilder &Add(const K &key, const V &value) {
    msg_ += ", ";
    msg_ += fmt::format(R"("{}": {})", key, value);
    return *this;
  }

  template<typename K, typename V, typename std::enable_if_t<use_json> * = nullptr>
  InspectionMessageBuilder &AddNoQuote(const K &key, const V &value) {
    msg_ += ", ";
    msg_ += fmt::format(R"({}: {})", key, value);
    return *this;
  }

  std::string Result() {
    return msg_ + "}";
  }
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_UTILS_INSPECTION_H_
