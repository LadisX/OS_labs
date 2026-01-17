#include <iostream>

const int N = 9;

int main() {
    int x;
    while (std::cin >> x) {
        std::cout << x + N << std::endl;
    }
    fflush(stdout);
    return 0;
}