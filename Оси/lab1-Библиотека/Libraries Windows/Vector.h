#pragma once

#ifdef VECTOR_EXPORTS
#define VECTOR_API __declspec(dllexport)
#else
#define VECTOR_API __declspec(dllimport)
#endif

#include "Number.h"

class Vector {
private:
    Number x;
    Number y;

public:
    VECTOR_API Vector(Number xVal, Number yVal);

    VECTOR_API Number getPolarRadius() const;
    VECTOR_API Number getPolarAngle() const;

    VECTOR_API Vector operator+(const Vector& other) const;

    VECTOR_API Number getX() const;
    VECTOR_API Number getY() const;
};

extern VECTOR_API const Vector ZERO_VECTOR;
extern VECTOR_API const Vector ONE_ONE_VECTOR;