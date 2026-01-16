#include <iostream>
#include "Number.h"
#include "Vector.h"
#include <windows.h>

int main() {
    std::cout << "=== Number Operations ===" << std::endl;

    Number first = createNumber(8.0);
    Number second = createNumber(4.0);

    Number addition = first + second;
    Number multiplication = first * second;

    std::cout << "First number: " << first.getValue() << std::endl;
    std::cout << "Second number: " << second.getValue() << std::endl;
    std::cout << "Sum: " << addition.getValue() << std::endl;
    std::cout << "Product: " << multiplication.getValue() << std::endl;

    std::cout << "Constants:" << std::endl;
    std::cout << "  Zero: " << ZERO.getValue() << std::endl;
    std::cout << "  One: " << ONE.getValue() << std::endl;

    std::cout << "\n=== Vector Operations ===" << std::endl;

    Vector vec1(createNumber(2.0), createNumber(5.0));
    Vector vec2(createNumber(3.0), createNumber(1.0));

    Vector vecSum = vec1 + vec2;
    Vector vecPlusOne = vec1 + ONE_ONE_VECTOR;

    std::cout << "Vector A coordinates:" << std::endl;
    std::cout << "  X: " << vec1.getX().getValue() << std::endl;
    std::cout << "  Y: " << vec1.getY().getValue() << std::endl;

    std::cout << "Vector B coordinates:" << std::endl;
    std::cout << "  X: " << vec2.getX().getValue() << std::endl;
    std::cout << "  Y: " << vec2.getY().getValue() << std::endl;

    std::cout << "Vector sum (A + B):" << std::endl;
    std::cout << "  X: " << vecSum.getX().getValue() << std::endl;
    std::cout << "  Y: " << vecSum.getY().getValue() << std::endl;

    std::cout << "Vector A plus (1,1):" << std::endl;
    std::cout << "  X: " << vecPlusOne.getX().getValue() << std::endl;
    std::cout << "  Y: " << vecPlusOne.getY().getValue() << std::endl;

    std::cout << "\nPolar representation of Vector A:" << std::endl;
    std::cout << "  Radius: " << vec1.getPolarRadius().getValue() << std::endl;
    std::cout << "  Angle: " << vec1.getPolarAngle().getValue() << " radians" << std::endl;

    std::cout << "\nZero vector: (" << ZERO_VECTOR.getX().getValue()
        << ", " << ZERO_VECTOR.getY().getValue() << ")" << std::endl;

    return 0;
}