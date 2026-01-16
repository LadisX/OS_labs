// Vector.h
#pragma once

#include <iostream>
#include <cmath>
#include <stdexcept>
#include "Number.h"

class Vector {
private:
    Number x;
    Number y;

public:
    Vector(Number xVal, Number yVal);

    Number getPolarRadius() const;
    Number getPolarAngle() const;

    Vector operator+(const Vector& other) const;

    Number getX() const;
    Number getY() const;
};

extern const Vector ZERO_VECTOR;
extern const Vector ONE_ONE_VECTOR;