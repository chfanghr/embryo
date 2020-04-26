//
// Created by 方泓睿 on 2020/4/20.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_UTILS_SORT_H_
#define EMBRYO_INCLUDE_EMBRYO_UTILS_SORT_H_

#include <memory>
#include <cstdlib>
#include <functional>

#include <embryo/utils/types.h>

namespace embryo::utils {
class Sortable {
 public:
  [[nodiscard]] virtual size_t Len() const = 0;
  [[nodiscard]] virtual bool Less(size_t i, size_t j) const = 0;
  virtual void Swap(size_t i, size_t j) = 0;

  virtual ~Sortable() = default;
};

void InsertionSort(const std::shared_ptr<Sortable> &data, size_t a, size_t b);

void HeapSort(const std::shared_ptr<Sortable> &data, size_t a, size_t b);

void QuickSort(const std::shared_ptr<Sortable> &data, size_t a, size_t b, size_t max_depth);

void Sort(const std::shared_ptr<Sortable> &data);

class SortWrapper : public Sortable {
 public:
  ~SortWrapper() override = default;
 private:
  const std::function<size_t()> &len_;
  const std::function<bool(size_t i, size_t j)> &less_;
  const std::function<void(size_t i, size_t j)> &swap_;
 public:
  SortWrapper(const std::function<size_t()> &len,
              const std::function<bool(size_t, size_t)> &less,
              const std::function<void(size_t, size_t)> &swap);

  [[nodiscard]] size_t Len() const override;
  [[nodiscard]] bool Less(size_t i, size_t j) const override;
  void Swap(size_t i, size_t j) override;
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_UTILS_SORT_H_
