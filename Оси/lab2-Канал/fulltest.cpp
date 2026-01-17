#include "buffered_channel.h"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cassert>

using namespace std::chrono_literals;

void run_basic_test() {
    std::cout << "Running basic test..." << std::endl;
    buffered_channel<int> pipe(5);

    pipe.send(10);
    pipe.send(20);
    pipe.send(30);

    auto [val1, ok1] = pipe.recv();
    auto [val2, ok2] = pipe.recv();
    auto [val3, ok3] = pipe.recv();

    assert(val1 == 10 && ok1);
    assert(val2 == 20 && ok2);
    assert(val3 == 30 && ok3);
    std::cout << "Basic test: PASSED" << std::endl;
}

void run_capacity_test() {
    std::cout << "Running capacity test..." << std::endl;
    buffered_channel<int> pipe(2);
    std::atomic<bool> stuck{ false };

    pipe.send(100);
    pipe.send(200);

    std::thread worker([&] {
        pipe.send(300);
        stuck.store(true);
        });

    std::this_thread::sleep_for(50ms);
    assert(!stuck.load());

    pipe.recv();
    std::this_thread::sleep_for(50ms);
    assert(stuck.load());

    worker.join();
    std::cout << "Capacity test: PASSED" << std::endl;
}

void run_shutdown_test() {
    std::cout << "Running shutdown test..." << std::endl;
    buffered_channel<int> pipe(3);
    pipe.send(1);
    pipe.send(2);
    pipe.close();

    auto [v1, s1] = pipe.recv();
    auto [v2, s2] = pipe.recv();
    auto [v3, s3] = pipe.recv();

    assert(v1 == 1 && s1);
    assert(v2 == 2 && s2);
    assert(!s3);

    bool caught_exception = false;
    try {
        pipe.send(999);
    }
    catch (const std::runtime_error&) {
        caught_exception = true;
    }
    assert(caught_exception);
    std::cout << "Shutdown test: PASSED" << std::endl;
}

void run_unblock_test() {
    std::cout << "Running unblock test..." << std::endl;
    buffered_channel<int> pipe(1);
    std::atomic<bool> woke_up{ false };

    std::thread sleeper([&] {
        auto [_, ok] = pipe.recv();
        if (!ok) woke_up.store(true);
        });

    std::this_thread::sleep_for(30ms);
    pipe.close();
    std::this_thread::sleep_for(30ms);

    assert(woke_up.load());
    sleeper.join();
    std::cout << "Unblock test: PASSED" << std::endl;
}

void run_stress_test() {
    std::cout << "Running stress test..." << std::endl;
    buffered_channel<int> pipe(8);
    const int ITEMS = 50;
    std::atomic<int> counter{ 0 };

    auto producer = [&] {
        for (int i = 0; i < ITEMS; ++i) {
            pipe.send(i);
        }
        pipe.close();
        };

    auto consumer = [&] {
        while (true) {
            auto [_, ok] = pipe.recv();
            if (!ok) break;
            counter.fetch_add(1);
        }
        };

    std::thread p1(producer);
    std::thread c1(consumer);
    std::thread c2(consumer);

    p1.join();
    c1.join();
    c2.join();

    assert(counter.load() == ITEMS);
    std::cout << "Stress test: PASSED (" << counter.load() << " items processed)" << std::endl;
}

struct CustomItem {
    int x;
    std::string tag;

    CustomItem(int a = 0, std::string b = "") : x(a), tag(std::move(b)) {}

    bool operator==(const CustomItem& other) const {
        return x == other.x && tag == other.tag;
    }
};

void run_custom_test() {
    std::cout << "Running custom type test..." << std::endl;
    buffered_channel<CustomItem> pipe(4);

    pipe.send(CustomItem(1, "first"));
    pipe.send(CustomItem(2, "second"));

    auto [item1, status1] = pipe.recv();
    auto [item2, status2] = pipe.recv();

    assert(item1 == CustomItem(1, "first") && status1);
    assert(item2 == CustomItem(2, "second") && status2);
    std::cout << "Custom type test: PASSED" << std::endl;
}

int main() {
    std::cout << "Starting channel tests" << std::endl;

    try {
        run_basic_test();
        run_capacity_test();
        run_shutdown_test();
        run_unblock_test();
        run_stress_test();
        run_custom_test();

        std::cout << "\nAll tests PASSED" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }

    return 0;
}