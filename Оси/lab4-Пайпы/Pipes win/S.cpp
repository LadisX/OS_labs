#include <iostream>

int main() {
    int x;
    int sum = 0;
    while (std::cin >> x) {
        sum += x;
    }
    std::cout << sum << std::endl;;
    fflush(stdout);
    return 0;
}