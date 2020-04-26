#include <vector>
#include <random>
#include <numeric>
#include <chrono>

#include <docopt/docopt.h>
#include <fmt/format.h>

#include <embryo/utils/sort.h>
#include <embryo/version.h>

static const char kUsage[] =
    R"(embryo-sort-benchmark: benchmark sort algorithm implemented inside embryo

  Usage:
    embryo-sort-benchmark [options]
    embryo-sort-benchmark --version

  Options:
    -s --data_size DATA_SIZE  Size of benchmark data.
    -r --repeat REPEAT        Times to repeat.
    -h --help                 Show this help message.
    --version                 Show version of embryo.
)";

class Vector : public std::vector<int>, public embryo::utils::Sortable {
 public:
  template<typename ...Arg>
  explicit Vector(Arg... args) :std::vector<int>{args...} {}

  [[nodiscard]] size_t Len() const override {
    return this->size();
  }
  [[nodiscard]] bool Less(size_t i, size_t j) const override {
    return (*this)[i] < (*this)[j];
  }
  void Swap(size_t i, size_t j) override {
    std::tie((*this)[i], (*this)[j]) = std::make_tuple((*this)[j], (*this)[i]);
  }
};

static size_t kDataSize = 10000;
static size_t kRepeat = 1;

void Measure(const std::function<void(const std::shared_ptr<Vector> &)> &task, const std::string &task_name) {
  double avg_time{};

  for (size_t i = 0; i < kRepeat; ++i) {
    auto begin = std::chrono::high_resolution_clock::now();

    auto vec = std::make_shared<Vector>();
    vec->resize(kDataSize);

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type>
        dist(std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max());

    for (size_t j = 0; j < kDataSize; ++j)
      (*vec)[i] = dist(rng);

    task(vec);

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - begin);

    avg_time += duration.count() / kRepeat;
  }

  fmt::print("{:<10}: avg {}s\n", task_name, avg_time);
}

int main(int argc, const char **argv) {
  auto args = docopt::docopt(kUsage,
                             {argv + 1, argv + argc},
                             true, EMBRYO_VERSION);
  if (args["--data_size"]) kDataSize = args["--data_size"].asLong();
  if (args["--repeat"]) kRepeat = args["--repeat"].asLong();

  fmt::print("{:<10}: {}\n", "data size", kDataSize);
  fmt::print("{:<10}: {}\n\n", "repeat", kRepeat);

  Measure([](const std::shared_ptr<Vector> &vec) { embryo::utils::Sort(vec); }, "embryo");

  Measure([](const std::shared_ptr<Vector> &vec) { std::sort(vec->begin(), vec->end()); }, "std");

  return EXIT_SUCCESS;
}