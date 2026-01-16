#pragma once

class Number {
private:
    double value;

public:
    Number(double val = 0.0);

    Number operator+(const Number& other) const;
    Number operator-(const Number& other) const;
    Number operator*(const Number& other) const;
    Number operator/(const Number& other) const;

    double getValue() const;
};

extern const Number ZERO;
extern const Number ONE;

Number createNumber(double value);

extern double getLibraryVersion();