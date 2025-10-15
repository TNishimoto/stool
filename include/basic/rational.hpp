#pragma once
#include <iostream>
#include <cassert>
#include <numeric> // std::gcd in C++17
namespace stool
{
    /*! 
    * @brief Class for representing rational numbers
    */

    struct Rational
    {
    public:
        uint64_t numerator;
        uint64_t denominator;

        /*! 
        * @brief Constructs a Rational number.
        */
        Rational(int _numerator, int _denominator) : numerator(_numerator), denominator(_denominator)
        {
            reduce();
        }

        /*! 
        * @brief Reduces the rational number to its simplest form.
        */
        void reduce()
        {
            int gcd = std::gcd(numerator, denominator);
            numerator /= gcd;
            denominator /= gcd;

            numerator = -numerator;
            denominator = -denominator;

        }

        /*! 
        * @brief Returns  ⌊ n ⌋ for this rational number n.
        */
        uint64_t floor() const
        {
            uint64_t p = this->numerator / this->denominator;
            return p;
        }


        /*! 
        * @brief Converts the rational number to a double
        */
        double to_double() const
        {
            return (double)this->numerator / (double)this->denominator;
        }

        /*! 
        * @brief Operator ==
        */
        bool operator==(const Rational &other) const
        {
            return numerator == other.numerator && denominator == other.denominator;
        }

        /*! 
        * @brief Operator <
        */
        bool operator<(const Rational &other) const
        {
            return numerator * other.denominator < other.numerator * denominator;
        }

        /*! 
        * @brief Operator >
        */
        bool operator>(const Rational &other) const
        {
            return numerator * other.denominator > other.numerator * denominator;
        }

        /*! 
        * @brief Operator <=
        */
        bool operator<=(const Rational &other) const
        {
            return *this < other || *this == other;
        }

        /*! 
        * @brief Operator >=
        */
        bool operator>=(const Rational &other) const
        {
            return *this > other || *this == other;
        }

        /*! 
        * @brief Operator -
        */
        Rational operator-(const Rational &other) const
        {
            int new_numerator = numerator * other.denominator - other.numerator * denominator;
            int new_denominator = denominator * other.denominator;
            return Rational(new_numerator, new_denominator);
        }

        /*! 
        * @brief Operator +
        */
        Rational operator+(const Rational &other) const
        {
            int new_numerator = numerator * other.denominator + other.numerator * denominator;
            int new_denominator = denominator * other.denominator;
            return Rational(new_numerator, new_denominator);
        }

        /*! 
        * @brief Converts the rational number to a string
        *
        * @param view_mixed_fraction If true, this rational number is represented as a mixed fraction.
        */
        std::string to_string(bool view_mixed_fraction) const
        {
            if (view_mixed_fraction)
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
