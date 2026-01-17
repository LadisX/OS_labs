#include <iostream>

int main() {
    int x;
    while (std::cin >> x) {
        std::cout << x * 3 << std::endl;
    }
    fflush(stdout);
    return 0;
}