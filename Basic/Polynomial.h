#pragma once
#include <string>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

struct x;

// Util data structures for calculating a basic cyclic redundancy check 
// https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
// https://en.wikipedia.org/wiki/Mathematics_of_cyclic_redundancy_checks

/**
 * Holds a bit string that represents the coefficients of a polynomial,
 * the input string is interpreted as having MSB endian,
 * e.g. 0b1011 is interpreted as the polynomial x3 + 0*x2 + x1 + x0 
 */
struct PolynomialBitString
{
    PolynomialBitString(uint32_t InBitString, uint32_t InLength);
    uint8_t operator[](uint32_t Index) const; // Index 0 represent the term with the highest degree
    PolynomialBitString Substring(uint32_t TotalElements) const;
    PolynomialBitString Substring(uint32_t Start, uint32_t TotalElements) const;
    uint32_t GetDegreeAt(uint32_t Index) const; // Degree of the term at given index
    uint32_t GetLength() const;
    uint32_t GetData() const;
    std::string ToString(uint32_t InitialDegree = 0) const; 

private:
    uint32_t BitString;
    uint32_t Length;    
};

/**
 * A collection of terms in a polynomial e.g. 1x^5 + 1x^2 + 1x
 */
struct Polynomial
{
    static Polynomial FromBitString(const PolynomialBitString& BitString);
    static const Polynomial& Zero();

    Polynomial() = default;
    Polynomial(std::initializer_list<x> InTerms);
    Polynomial(const x& InTerm);

    // Copy & Move
    Polynomial(const Polynomial& Other) = default;
    Polynomial(Polynomial&& Other) noexcept;
    Polynomial& operator=(const Polynomial& Other);
    Polynomial& operator=(Polynomial&& Other) noexcept;

    ~Polynomial() = default;

    bool operator==(const Polynomial& Other) const;
    bool operator!=(const Polynomial& Other) const;

    Polynomial operator+(const Polynomial& Other) const;
    Polynomial operator+(const x& Term) const;
    
    Polynomial operator-(const Polynomial& Other) const;
    Polynomial operator-(const x& Term) const;

    Polynomial operator*(const Polynomial& Other) const;
    Polynomial operator*(const x& Term) const;

    Polynomial operator/(const Polynomial& Other) const;
    Polynomial operator/(const x& Term) const;
    bool HasTerms() const;
    uint32_t GetDegree() const;
    size_t TotalTerms() const;
    
    std::string ToString() const;
    std::string ToDebugString(uint32_t InitialDegree = 0) const;

private:
    using BinaryOperationFunction = std::function<x(const x& A, const x& B)>;
    static void AddTerms(Polynomial& Result, const Polynomial& Other);
    static void DistributeTerms(Polynomial& Result, const Polynomial& Other, const BinaryOperationFunction& BinaryOperation);

    std::vector<x> Terms;
};

/**
 * Represents a polynomial term in the ring of polynomials over GF(2)
 * (the finite field of integers modulo 2). That is, the set of polynomials
 * where each coefficient is either zero or one,
 * and arithmetic operations wrap around.
 */
struct x
{
    explicit x(uint32_t InExponent);

    /**
     * Check if two terms are equal
     * @param Other Other term to compare
     * @return True if both terms are equal
     */
    bool operator==(const x& Other) const;

    /**
     * Check if two terms are equal
     * @param Other Other term to compare
     * @return True if both terms are equal
     */
    bool operator!=(const x& Other) const;
    
    /**
     * Multiplies This term by Other term
     * @param Other Term to multiply
     * @return Returns the multiplication of two terms i.e. This * Other
     */
    Polynomial operator*(const x& Other) const;

    /**
     * Divides This term by Other term
     * @param Other Divisor term
     * @return The division of This and Other i.e. This / Other
     */
    Polynomial operator/(const x& Other) const;

    /**
     * Adds Other term to This term
     * @param Other Term to add
     * @return The addition of Other and This i.e. This + Other
     */
    Polynomial operator+(const x& Other) const;

    /**
     * Subtracts Other from This
     * @param Other Term to subtract
     * @return The subtraction of This and other i.e. This - Other
     */
    Polynomial operator-(const x& Other) const;

    /**
     * Checks if the coefficient of this term is zero
     * @return True if the coefficient is zero
     */
    bool IsZero() const;
    
    /**
     * Gets the coefficient
     * @return The coefficient of this term
     */
    uint8_t GetCoefficient() const;

    /**
     * Gets the exponent
     * @return The exponent of this term
     */
    uint32_t GetExponent() const;

    /**
     * Generate a string representing this polynomial term
     * @return Readable string representing this polynomial term
     */
    std::string ToString() const;

private:
    /**
     * The exponent of the polynomial term
     */
    uint32_t Exponent;

    /**
     * A member of the field GF(2), default value is 1
     */
    uint8_t Coefficient : 1;

    // Friends
    friend x operator*(uint8_t Coefficient, const x& Right);
};

/**
 * Multiplies a coefficient to a polynomial term
 * @param Coefficient The coeffiecient of the polynomial term, this number is only 1 or 0, so this value will be mod2
 * @param Right The polynomial term
 * @return A new polynomial term of type Cx^n where n is an element of GF(2) i.e. 0 or 1    
 */
x operator*(uint8_t Coefficient, const x& Right);
