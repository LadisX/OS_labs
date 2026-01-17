#include <iostream>
#include <thread>
#include "buffered_channel.h"

int main() {
    buffered_channel<int> ch(2);

    ch.send(1);
    ch.send(2);

    auto [v1, s1] = ch.recv();
    auto [v2, s2] = ch.recv();

    std::cout << v1 << " " << v2 << std::endl;

    std::thread t1([&]() { ch.send(3); });
    std::thread t2([&]() {
        auto [v3, s3] = ch.recv();
        std::cout << v3 << std::endl;
        });

    t1.join();
    t2.join();

    ch.close();

    // Проверяем работу после закрытия
    try {
        ch.send(100); // Должно бросить исключение
    }
    catch (const std::runtime_error& e) {
        std::cout << "Caught expected exception: " << e.what() << std::endl;
    }

    auto [v4, s4] = ch.recv();
    if (!s4) {
        std::cout << "Channel is closed" << std::endl;
    }

    std::cout << "Finished" << std::endl;
    return 0;
}