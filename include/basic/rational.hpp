#pragma once
#include <iostream>
#include <cassert>
#include <numeric> // std::gcd in C++17
namespace stool
{
    /*! 
    * @brief Class for representing and manipulating rational numbers
    *
    * This class provides functionality for working with rational numbers (fractions)
    * represented as pairs of integers (numerator/denominator). It includes methods
    * for basic arithmetic operations, comparison, reduction to lowest terms, and
    * conversion to other numeric formats.
    *
    * The class stores rational numbers in reduced form, automatically simplifying
    * fractions using GCD when constructed or modified. It also ensures the denominator
    * is always positive by moving any negative sign to the numerator.
    *
    * Key features:
    * - Automatic reduction to lowest terms using GCD
    * - Proper handling of signs
    * - Conversion to decimal and string representations
    * - Basic arithmetic operations
    * - Comparison operations
    */

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


        uint64_t floor() const
        {
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
        Rational operator+(const Rational &other) const
        {
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
