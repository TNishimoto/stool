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

        /*! 
        * @brief Constructs a Rational number
        * 
        * This constructor initializes the numerator and denominator of the rational number
        * and reduces it to its simplest form.
        *
        * @param num The numerator of the rational number
        * @param den The denominator of the rational number
        */
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

        /*! 
        * @brief Returns the integer part of the rational number
        * 
        * This function returns the integer part of the rational number by performing
        * integer division of the numerator by the denominator.
        *
        * @return The integer part of the rational number
        */
        uint64_t floor() const
        {
            uint64_t p = this->numerator / this->denominator;
            return p;
        }

        /*! 
        * @brief Displays the rational number in the form of numerator/denominator
        * 
        * This function prints the rational number in the format "numerator/denominator"
        * to the standard output stream.
        */
        void display() const
        {
            std::cout << numerator << "/" << denominator << std::endl;
        }

        /*! 
        * @brief Converts the rational number to a double
        * 
        * This function returns the double representation of the rational number
        * by dividing the numerator by the denominator.
        *
        * @return The double representation of the rational number
        */
        double to_double() const
        {
            return (double)this->numerator / (double)this->denominator;
        }

        /*! 
        * @brief Compares two rational numbers for equality
        * 
        * This function checks if two rational numbers are equal by comparing their
        * numerators and denominators.
        *
        * @param other The rational number to compare with
        * @return True if the rational numbers are equal, false otherwise
        */
        bool operator==(const Rational &other) const
        {
            return numerator == other.numerator && denominator == other.denominator;
        }

        /*! 
        * @brief Compares two rational numbers for less than
        * 
        * This function checks if the current rational number is less than another
        * rational number by comparing their numerators and denominators.
        *
        * @param other The rational number to compare with
        * @return True if the current rational number is less than the other, false otherwise
        */
        bool operator<(const Rational &other) const
        {
            return numerator * other.denominator < other.numerator * denominator;
        }

        /*! 
        * @brief Compares two rational numbers for greater than
        * 
        * This function checks if the current rational number is greater than another
        * rational number by comparing their numerators and denominators.
        *
        * @param other The rational number to compare with
        * @return True if the current rational number is greater than the other, false otherwise
        */
        bool operator>(const Rational &other) const
        {
            return numerator * other.denominator > other.numerator * denominator;
        }

        /*! 
        * @brief Compares two rational numbers for less than or equal to
        * 
        * This function checks if the current rational number is less than or equal to another
        * rational number by comparing their numerators and denominators.
        *
        * @param other The rational number to compare with
        * @return True if the current rational number is less than or equal to the other, false otherwise
        */
        bool operator<=(const Rational &other) const
        {
            return *this < other || *this == other;
        }

        /*! 
        * @brief Compares two rational numbers for greater than or equal to
        * 
        * This function checks if the current rational number is greater than or equal to another
        * rational number by comparing their numerators and denominators.
        *
        * @param other The rational number to compare with
        * @return True if the current rational number is greater than or equal to the other, false otherwise
        */
        bool operator>=(const Rational &other) const
        {
            return *this > other || *this == other;
        }

        /*! 
        * @brief Subtracts two rational numbers
        * 
        * This function subtracts another rational number from the current one by
        * calculating the difference of their numerators and denominators.
        *
        * @param other The rational number to subtract
        * @return The result of the subtraction
        */
        Rational operator-(const Rational &other) const
        {
            int new_numerator = numerator * other.denominator - other.numerator * denominator;
            int new_denominator = denominator * other.denominator;
            return Rational(new_numerator, new_denominator);
        }

        /*! 
        * @brief Adds two rational numbers
        * 
        * This function adds another rational number to the current one by
        * calculating the sum of their numerators and denominators.
        *
        * @param other The rational number to add
        * @return The result of the addition
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
        * This function returns the string representation of the rational number.
        * If the second argument is true, it returns the string in the form of "p(r)"
        * where p is the integer part and r is the fractional part.
        *
        * @param taibunsuu If true, returns the string in the form of "p(r)"
        * @return The string representation of the rational number
        */
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
