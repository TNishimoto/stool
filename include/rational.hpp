#pragma once
#include <iostream>
#include <cassert>
#include <numeric> // std::gcd in C++17
namespace stool
{
    struct Rational
    {
    public:
        uint64_t numerator;
        uint64_t denominator;
        Rational(int num, int den) : numerator(num), denominator(den)
        {
            reduce();
        }
        void reduce()
        {
            int gcd = std::gcd(numerator, denominator);
            numerator /= gcd;
            denominator /= gcd;

            if (denominator < 0)
            {
                numerator = -numerator;
                denominator = -denominator;
            }
        }
        uint64_t floor() const {
            uint64_t p = this->numerator / this->denominator;
            return p;
        }
        void display() const
        {
            std::cout << numerator << "/" << denominator << std::endl;
        }
        double to_double() const
        {
            return (double)this->numerator / (double)this->denominator;
        }
        bool operator==(const Rational &other) const
        {
            return numerator == other.numerator && denominator == other.denominator;
        }

        bool operator<(const Rational &other) const
        {
            return numerator * other.denominator < other.numerator * denominator;
        }

        bool operator>(const Rational &other) const
        {
            return numerator * other.denominator > other.numerator * denominator;
        }

        bool operator<=(const Rational &other) const
        {
            return *this < other || *this == other;
        }

        bool operator>=(const Rational &other) const
        {
            return *this > other || *this == other;
        }
        Rational operator-(const Rational &other) const
        {
            int new_numerator = numerator * other.denominator - other.numerator * denominator;
            int new_denominator = denominator * other.denominator;
            return Rational(new_numerator, new_denominator);
        }
        Rational operator+(const Rational& other) const {
        int new_numerator = numerator * other.denominator + other.numerator * denominator;
        int new_denominator = denominator * other.denominator;
        return Rational(new_numerator, new_denominator);
    }
        std::string to_string(bool taibunsuu) const
        {
            if (taibunsuu)
            {
                uint64_t p = this->numerator / this->denominator;
                Rational r(this->numerator - (p * this->denominator), this->denominator);
                return std::to_string(p) + "(" + r.to_string(false) + ")";
            }
            else
            {
                return std::to_string(this->numerator) + "/" + std::to_string(this->denominator);
            }
        }
    };
}
