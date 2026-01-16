#include "pch.h"

Number::Number(double val) : value(val) {}

Number Number::operator+(const Number& other) const {
    return Number(value + other.value);
}

Number Number::operator-(const Number& other) const {
    return Number(value - other.value);
}

Number Number::operator*(const Number& other) const {
    return Number(value * other.value);
}

Number Number::operator/(const Number& other) const {
    if (other.value == 0.0) {
        throw std::invalid_argument("Division by zero!");
    }
    return Number(value / other.value);
}

double Number::getValue() const {
    return value;
}

const Number ZERO(0.0);
const Number ONE(1.0);
const Number TWO(2.0);

Number createNumber(double value) {
    return Number(value);
}

double getLibraryVersion() {
    return 1.42;
}