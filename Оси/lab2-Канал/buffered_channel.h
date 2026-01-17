#pragma once
#include <utility>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <stdexcept>

template<typename T>
class buffered_channel {
public:
    explicit buffered_channel(size_t buffer_size)
        : capacity_(buffer_size), closed_(false) {
    }

    buffered_channel(const buffered_channel&) = delete;
    buffered_channel& operator=(const buffered_channel&) = delete;

    void send(const T& value) {
        std::unique_lock<std::mutex> lock(mtx_);

        // Исправлено: добавили скобки для ясности
        cv_not_full_.wait(lock, [this]() {
            return closed_ || queue_.size() < capacity_;
            });

        if (closed_) {
            throw std::runtime_error("send on closed channel");
        }

        queue_.push(value);
        cv_not_empty_.notify_one();
    }

    // Добавляем move версию для эффективности
    void send(T&& value) {
        std::unique_lock<std::mutex> lock(mtx_);

        cv_not_full_.wait(lock, [this]() {
            return closed_ || queue_.size() < capacity_;
            });

        if (closed_) {
            throw std::runtime_error("send on closed channel");
        }

        queue_.push(std::move(value));
        cv_not_empty_.notify_one();
    }

    std::pair<T, bool> recv() {
        std::unique_lock<std::mutex> lock(mtx_);

        cv_not_empty_.wait(lock, [this]() {
            return closed_ || !queue_.empty();
            });

        if (!queue_.empty()) {
            T value = std::move(queue_.front());
            queue_.pop();
            cv_not_full_.notify_one();
            return { std::move(value), true };
        }

        return { T{}, false };
    }

    void close() {
        std::unique_lock<std::mutex> lock(mtx_);
        closed_ = true;
        cv_not_empty_.notify_all();
        cv_not_full_.notify_all();
    }

private:
    std::queue<T> queue_;
    size_t capacity_;
    bool closed_;
    mutable std::mutex mtx_;
    std::condition_variable cv_not_empty_;
    std::condition_variable cv_not_full_;
};