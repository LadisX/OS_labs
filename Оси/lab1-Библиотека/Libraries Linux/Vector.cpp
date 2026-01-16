#include "Vector.h"

Vector::Vector(Number xVal, Number yVal) : x(xVal), y(yVal) {}

Number Vector::getPolarRadius() const {
    Number x_squared = x * x;
    Number y_squared = y * y;
    Number sum = x_squared + y_squared;
    return createNumber(std::sqrt(sum.getValue()));
}

Number Vector::getPolarAngle() const {
    return createNumber(std::atan2(y.getValue(), x.getValue()));
}

Vector Vector::operator+(const Vector& other) const {
    return Vector(x + other.x, y + other.y);
}

Number Vector::getX() const { return x; }
Number Vector::getY() const { return y; }

const Vector ZERO_VECTOR(ZERO, ZERO);
const Vector ONE_ONE_VECTOR(ONE, ONE);