#include <iostream>
#include "Number.h"
#include "Vector.h"

int main() {

    std::cout << "=== Number Library Demonstration ===" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    Number num1 = createNumber(5.0);
    Number num2 = createNumber(3.0);

    Number result_sum = num1 + num2;
    Number result_mult = num1 * num2;

    std::cout << "Number 1: " << num1.getValue() << std::endl;
    std::cout << "Number 2: " << num2.getValue() << std::endl;
    std::cout << "Summation result: " << result_sum.getValue() << std::endl;
    std::cout << "Multiplication result: " << result_mult.getValue() << std::endl;

    std::cout << "\nLibrary constants:" << std::endl;
    std::cout << "Zero constant: " << ZERO.getValue() << std::endl;
    std::cout << "Unity constant: " << ONE.getValue() << std::endl;

    std::cout << "\n=== Vector Library Demonstration ===" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    Vector vectorA(createNumber(3.0), createNumber(4.0));
    Vector vectorB(createNumber(1.0), createNumber(2.0));

    Vector vectorSum = vectorA + vectorB;
    Vector vectorPlusUnit = vectorA + ONE_ONE_VECTOR;

    std::cout << "Vector A:" << std::endl;
    std::cout << "  x-component = " << vectorA.getX().getValue() << std::endl;
    std::cout << "  y-component = " << vectorA.getY().getValue() << std::endl;

    std::cout << "\nVector B:" << std::endl;
    std::cout << "  x-component = " << vectorB.getX().getValue() << std::endl;
    std::cout << "  y-component = " << vectorB.getY().getValue() << std::endl;

    std::cout << "\nOperations:" << std::endl;
    std::cout << "Vector A + Vector B:" << std::endl;
    std::cout << "  result x = " << vectorSum.getX().getValue() << std::endl;
    std::cout << "  result y = " << vectorSum.getY().getValue() << std::endl;

    std::cout << "\nVector A + (1,1) vector:" << std::endl;
    std::cout << "  result x = " << vectorPlusUnit.getX().getValue() << std::endl;
    std::cout << "  result y = " << vectorPlusUnit.getY().getValue() << std::endl;

    std::cout << "\nPolar coordinates for Vector A:" << std::endl;
    std::cout << "  Polar radius: " << vectorA.getPolarRadius().getValue() << std::endl;
    std::cout << "  Polar angle: " << vectorA.getPolarAngle().getValue() << " rad" << std::endl;

    std::cout << "\nZero vector representation:" << std::endl;
    std::cout << "  Coordinates: (" << ZERO_VECTOR.getX().getValue()
        << ", " << ZERO_VECTOR.getY().getValue() << ")" << std::endl;


    return 0;
}