//
// Created by 方泓睿 on 2020/4/22.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_UTILS_CHAN_H_
#define EMBRYO_INCLUDE_EMBRYO_UTILS_CHAN_H_

#include <mutex>
#include <deque>
#include <type_traits>
#include <stdexcept>
#include <memory>
#include <tuple>
#include <optional>

#include <embryo/utils/inspection.h>

namespace embryo::utils {
template<typename T>
class Chan {
 private:
  class Impl {
   public:
    std::deque<T> data_;
    std::mutex mutex_;
    std::atomic_bool closed_ = false;
    const size_t buffer_size_;

    explicit Impl(size_t buffer_size) : buffer_size_(buffer_size) {
      if (buffer_size_ == 0)
        throw std::invalid_argument("buffer size should larger than 0");
    }
  };

  std::shared_ptr<Impl> impl_;

  [[nodiscard]] bool IsFull() const { return impl_->data_.size() >= impl_->buffer_size_; }
  [[nodiscard]] bool IsEmpty() const { return impl_->data_.empty(); }

  [[noreturn]] void Error(const std::string &msg) const {
    throw std::runtime_error(fmt::format("chan {}: {}", (void *) this, msg));
  }
 public:
  explicit Chan(size_t buffer_size = 1) : impl_(std::make_shared<Impl>(buffer_size)) {}
  Chan(const Chan &obj) : impl_(obj.impl_) {}
  Chan(Chan &&obj) noexcept : impl_(std::move(obj.impl_)) {}
  Chan &operator=(const Chan &obj) {
    if (&obj == this)
      return *this;
    impl_ = obj.impl_;
    return *this;
  }
  Chan(std::nullptr_t) { impl_ = nullptr; } // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

  template<bool is_try = false, typename std::enable_if_t<(!is_try)> * = nullptr>
  T Receive() const {
    if (!impl_)
      Error("trying to operate a empty channel");
    if (impl_->closed_) {
      impl_->mutex_.lock();
      if (IsEmpty()) {
        impl_->mutex_.unlock();
        Error("reading from a empty closed channel");
      }
      T res = impl_->data_.front();
      impl_->data_.pop_front();
      impl_->mutex_.unlock();
      return res;
    }
    while (true) {
      impl_->mutex_.lock();
      if (IsEmpty()) {
        if (impl_.unique())
          throw std::runtime_error("no available writer, dead lock");
        impl_->mutex_.unlock();
        continue;
      }
      T res = impl_->data_.front();
      impl_->data_.pop_front();
      impl_->mutex_.unlock();
      return res;
    }
  }

  template<bool is_try = false, typename std::enable_if_t<is_try> * = nullptr>
  std::tuple<std::optional<T>, bool> Receive() const {
    if (!impl_)
      return false;

    if (impl_->mutex_.try_lock()) {
      if (IsEmpty()) {
        impl_->mutex_.unlock();
        return {{}, false};
      }
      T res = impl_->data_.front();
      impl_->data_.pop_front();
      impl_->mutex_.unlock();
      return {res, true};
    }
    return {{}, false};
  }

  template<bool is_try = false, typename std::enable_if_t<(!is_try)> * = nullptr>
  void Send(const T &obj) const {
    if (!impl_)
      Error("trying to operate a empty channel");

    if (impl_->closed_)
      Error("writing to a closed channel");

    while (true) {
      impl_->mutex_.lock();
      if (IsFull()) {
        if (impl_.unique())
          throw std::runtime_error("no available reader, dead lock");
        impl_->mutex_.unlock();
        continue;
      }
      impl_->data_.push_back(obj);
      impl_->mutex_.unlock();
      break;
    }
  }

  template<bool is_try = false, typename std::enable_if_t<is_try> * = nullptr>
  bool Send(const T &obj) const {
    if (!impl_)
      return false;

    if (impl_->closed_)
      return false;

    if (impl_->mutex_.try_lock()) {
      impl_->mutex_.lock();
      if (IsFull()) {
        impl_->mutex_.unlock();
        return false;
      }
      impl_->data_.push_back(obj);
      impl_->mutex_.unlock();
      return true;
    }

    return false;
  }

  void Close() const { impl_->closed_ = true; }

  [[nodiscard]] bool IsClosed() const { return !impl_ || impl_->closed_; }

  explicit operator bool() const { return !IsClosed(); }

  class Sender {
   private:
    Chan channel_;

   public:
    Sender(const Chan &channel) : channel_(channel) {} // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

    Sender(std::nullptr_t) : channel_(nullptr) {} // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

    explicit operator bool() const { return bool(channel_); }

    template<bool is_try = false, typename std::enable_if_t<(!is_try)> * = nullptr>
    void Send(const T &obj) const {
      channel_.Send(obj);
    }

    template<bool is_try = false, typename std::enable_if_t<(is_try)> * = nullptr>
    bool Send(const T &obj) const {
      return channel_.Send<true>();
    }

    friend Sender operator<<(Sender sender, const T &obj) {
      sender.Send(obj);
      return sender;
    }
  };

  class Receiver {
   private:
    Chan channel_;

   public:
    Receiver(const Chan &channel) // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
        : channel_(channel) {}

    Receiver(std::nullptr_t) : channel_(nullptr) {} // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

    explicit operator bool() const { return bool(channel_); }

    template<bool is_try = false, typename std::enable_if_t<(!is_try)> * = nullptr>
    T Receive() const {
      return channel_.Receive();
    }

    template<bool is_try = false, typename std::enable_if_t<(is_try)> * = nullptr>
    std::tuple<std::optional<T>, bool> Receive() const {
      return channel_.Receive<true>();
    }

    friend Receiver operator>>(Receiver receiver, T &obj) {
      obj = receiver.Receive();
      return receiver;
    }
  };

  explicit operator Sender() const { return Sender(*this); }

  explicit operator Receiver() const { return Receiver(*this); }

  friend Chan operator<<(Chan channel, const T &obj) {
    channel.Send(obj);
    return channel;
  }

  friend Chan operator>>(Chan channel, T &obj) {
    obj = channel.Receive();
    return channel;
  }

  Sender GetSender() const { return Sender(*this); }

  Receiver GetReceiver() const { return Receiver(*this); }

  void Initialize(size_t buffer_size = 1) {
    impl_ = std::make_shared<Impl>(buffer_size);
  }
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_UTILS_CHAN_H_
