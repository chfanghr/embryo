//
// Created by 方泓睿 on 2020/4/20.
//

#include <tuple>
#include <cinttypes>

#include <embryo/utils/sort.h>

namespace embryo::utils {
void InsertionSort(const std::shared_ptr<Sortable> &data, size_t a, size_t b) {
  for (auto i = a + 1; i < b; i++)
    for (auto j = i; j > a && data->Less(j, j - 1); --j)
      data->Swap(j, j - 1);
}

namespace {
void ShiftDown(const std::shared_ptr<Sortable> &data, size_t lo, size_t hi, size_t first) {
  auto root = lo;
  while (true) {
    auto child = 2 * root + 1;
    if (child >= hi)
      return;
    if (child + 1 < hi && data->Less(first + child, first + child + 1))
      ++child;
    if (!data->Less(first + root, first + child))
      return;
    data->Swap(first + root, first + child);
    root = child;
  }
}
}

void HeapSort(const std::shared_ptr<Sortable> &data, size_t a, size_t b) {
  auto first = a;
  auto lo = 0;
  auto hi = b - a;

  for (int64_t i = (hi - 1) / 2; i >= 0; --i)
    ShiftDown(data, i, hi, first);

  for (int64_t i = hi - 1; i >= 0; --i) {
    data->Swap(first, first + i);
    ShiftDown(data, lo, i, first);
  }
}

namespace {
void MedianOfThree(const std::shared_ptr<Sortable> &data, size_t m1, size_t m0, size_t m2) {
  if (data->Less(m1, m0))
    data->Swap(m1, m0);

  if (data->Less(m2, m1)) {
    data->Swap(m2, m1);
    if (data->Less(m1, m0))
      data->Swap(m1, m0);
  }
}

size_t MaxDepth(size_t n) {
  size_t depth{};
  for (int64_t i = n; i > 0; i >>= 1) // NOLINT(hicpp-signed-bitwise)
    ++depth;
  return depth;
}

std::tuple<size_t/*mid_lo*/, size_t/*mid_hi*/> DoPivot(const std::shared_ptr<Sortable> &data, size_t lo, size_t hi) {
  size_t m = (lo + hi) >> 1; // NOLINT(hicpp-signed-bitwise)
  if (hi - lo > 40) {
    auto s = (hi - lo) / 8;
    MedianOfThree(data, lo, lo + s, lo + 2 * s);
    MedianOfThree(data, m, m - s, m + s);
    MedianOfThree(data, hi - 1, hi - 1 - s, hi - 1 - 2 * s);
  }
  MedianOfThree(data, lo, m, hi - 1);

  auto pivot = lo;
  auto a = lo + 1, c = hi - 1;

  for (; a < c && data->Less(a, pivot); ++a);
  auto b = a;
  while (true) {
    for (; b < c && !data->Less(pivot, b); ++b);
    for (; b < c && data->Less(pivot, c - 1); --c);
    if (b >= c)
      break;

    data->Swap(b, c - 1);
    ++b;
    --c;
  }

  auto protect = hi - c < 5;
  if (!protect && hi - c < (hi - lo) / 4) {
    auto dups = 0;
    if (!data->Less(pivot, hi - 1)) {
      data->Swap(c, hi - 1);
      ++c;
      ++dups;
    }
    if (!data->Less(b - 1, pivot)) {
      --b;
      ++dups;
    }

    if (!data->Less(m, pivot)) {
      data->Swap(m, b - 1);
      --b;
      ++dups;
    }

    protect = dups > 1;
  }

  if (protect) {
    while (true) {
      for (; a < b && !data->Less(b - 1, pivot); --b);
      for (; a < b && data->Less(a, pivot); ++a);
      if (a >= b)
        break;

      data->Swap(a, b - 1);
      ++a;
      --b;
    }
  }

  data->Swap(pivot, b - 1);
  return {b - 1, c};
}
}

void QuickSort(const std::shared_ptr<Sortable> &data, size_t a, size_t b, size_t max_depth) {
  while (b - a > 12) {
    if (max_depth == 0) {
      HeapSort(data, a, b);
      return;
    }
    --max_depth;
    auto[m_lo, m_hi]=DoPivot(data, a, b);

    if (m_lo < b - m_hi) {
      QuickSort(data, a, m_lo, max_depth);
      a = m_hi;
    } else {
      QuickSort(data, m_hi, b, max_depth);
      b = m_lo;
    }
  }

  if (b - a > 1) {
    for (auto i = a + 6; i < b; ++i) {
      if (data->Less(i, i - 6))
        data->Swap(i, i - 6);
    }
    InsertionSort(data, a, b);
  }
}

void Sort(const std::shared_ptr<Sortable> &data) {
  auto n = data->Len();
  QuickSort(data, 0, n, MaxDepth(n));
}

SortWrapper::SortWrapper(const std::function<size_t()> &len,
                         const std::function<bool(size_t, size_t)> &less,
                         const std::function<void(size_t, size_t)> &swap) :
    len_(len), less_(less), swap_(swap) {}

size_t SortWrapper::Len() const {
  return len_();
}

bool SortWrapper::Less(size_t i, size_t j) const {
  return less_(i, j);
}

void SortWrapper::Swap(size_t i, size_t j) {
  return swap_(i, j);
}
}