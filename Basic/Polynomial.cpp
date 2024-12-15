#include "Polynomial.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <sstream>

// https://en.wikipedia.org/wiki/Field_(mathematics)
// https://en.wikipedia.org/wiki/Ring_(mathematics)
// https://en.wikipedia.org/wiki/Polynomial_ring
// https://en.wikipedia.org/wiki/GF(2)

// Description of the field FG(2)
// Elements: 0, 1
// binary operations: +, *

// operation + on GF(2)
// 0 + 0 = 0
// 0 + 1 = 1
// 1 + 0 = 1
// 1 + 1 = 0

// operation * on GF(2)
// 0 * 0 = 0
// 0 * 1 = 0
// 1 * 0 = 0
// 1 * 1 = 1

// Field axioms:
// - Associativity of addition and multiplication: a + (b + c) = (a + b) + c and a * (b * c) = (a * b)
// GF(2) example:
// 0 + (1 + 1) = (0 + 1) + 1

// - Commutativity of addition and multiplication: a + b = b + a and a * b = b * a
// GF(2) example:
// 0 + 1 = 1 + 0 and 1 * 0 = 0 * 1

// - Additive and multiplicative identity: there exist two distinct elements 0 and 1 in F such that a + 0 = a and a * 1 = a
// GF(2) example:
// Additive identity is element 0
// Multiplicative identity is element 1

// - Additive inverses: for every a in F, there exists an element in F, denoted -a, called the additive inverse of a, such that a + (−a) = 0.
// GF(2) example:
// Additive inverse of element 0 is element 0
// Additive inverse of element 1 is element 1

// - Multiplicative inverses: for every a ≠ 0 in F, there exists an element in F, denoted by a−1 or 1/a,
// GF(2) example:
// Multiplicative inverse of element 1 is element 1

// - Distributivity of multiplication over addition: a * (b + c) = (a * b) + (a * c)
// GF(2) example:
// 1 * (1 + 0) = (1 * 1) + (1 * 0)

namespace
{
    void SortTerms(std::vector<x>& Terms)
    {
        // Sort terms by degree
        std::sort(Terms.begin(), Terms.end(),
            [](const x& A, const x& B)
            {
                return A.GetExponent() > B.GetExponent();
            });
    }

    void EraseZeroTerms(std::vector<x>& Terms)
    {
        // Remove terms with coefficient zero
        const std::vector<x>::iterator BeginErase = std::remove_if(Terms.begin(), Terms.end(),
            [](const x& Term)
            {
                return Term.IsZero();
            });
        Terms.erase(BeginErase, Terms.end());
    }

    x AddTerms(const x& A, const x& B)
    {
        uint8_t NewCoefficient = A.GetCoefficient() + B.GetCoefficient();
        return NewCoefficient * x{ A.GetExponent() };
    }

    x MultiplyTerms(const x& A, const x& B)
    {
        uint8_t NewCoefficient = A.GetCoefficient() * B.GetCoefficient();
        return NewCoefficient * x{ A.GetExponent() + B.GetExponent() };
    }
    
    x DivideTerms(const x& A, const x& B)
    {
        assert(B.GetCoefficient() != 0);
        uint8_t NewCoefficient = A.GetCoefficient() * B.GetCoefficient();
        return NewCoefficient * x{ A.GetExponent() - B.GetExponent() };
    }
}

PolynomialBitString::PolynomialBitString(uint32_t InBitString, uint32_t InLength)
    : BitString(InBitString), Length(InLength)
{
}

uint8_t PolynomialBitString::operator[](uint32_t Index) const
{
    if (Index >= Length)
    {
        return 0;
    }

    // Index zero represents the highest degree term, we could also
    // revert the endian-ess of the BitString field, but let's be fancy
    uint32_t StartIndex = Length - 1;
    uint32_t BitMask = 1 << (StartIndex - Index);
    return (BitString & BitMask) == BitMask;    
}

PolynomialBitString PolynomialBitString::Substring(uint32_t TotalElements) const
{
    assert(TotalElements <= Length);
    uint32_t TotalDiscardedBits = Length - TotalElements;
    uint32_t NewSubstring = BitString >> TotalDiscardedBits;
    
    return PolynomialBitString{ NewSubstring, TotalElements };
}

PolynomialBitString PolynomialBitString::Substring(uint32_t Start, uint32_t TotalElements) const
{
    assert(Start + TotalElements <= Length);
    uint32_t TotalDiscardedBits = Length - Start - TotalElements;
    uint32_t NewSubstring = BitString >> TotalDiscardedBits;
    return PolynomialBitString{ NewSubstring, TotalElements };
}

uint32_t PolynomialBitString::GetDegreeAt(uint32_t Index) const
{
    uint32_t StartIndex = Length - 1;
    return StartIndex - Index; 
}

uint32_t PolynomialBitString::TotalTerms() const
{
    return Length;
}

uint32_t PolynomialBitString::GetData() const
{
    return BitString;
}

std::string PolynomialBitString::ToString(uint32_t InitialDegree) const
{
    std::stringstream ss;

    uint32_t TotalBits = InitialDegree > 0 ? InitialDegree : Length;
    for (uint32_t Index = 0; Index < TotalBits; ++Index)
    {
        uint32_t Value = (*this)[Index] ? 1 : 0;
        ss << Value;
    }

    return ss.str();
}

Polynomial Polynomial::FromBitString(const PolynomialBitString& BitString)
{
    Polynomial Result;
    for (uint32_t Index = 0; Index < BitString.TotalTerms(); ++Index)
    {
        if (BitString[Index])
        {
            Result.Terms.emplace_back(BitString.GetDegreeAt(Index));
        }
    }

    return Result;
}

const Polynomial& Polynomial::Zero()
{
    static Polynomial ZeroPolynomial;
    return ZeroPolynomial;
}

Polynomial::Polynomial(std::initializer_list<x> InTerms)
    : Terms(InTerms)
{
    if (Terms.size() <= 1)
    {
        return;
    }

    EraseZeroTerms(Terms);
    SortTerms(Terms);
}

Polynomial::Polynomial(const x& InTerm)
{
    if (InTerm.IsZero())
    {
        return;
    }

    Terms.push_back(InTerm);
}

Polynomial::Polynomial(Polynomial&& Other) noexcept
    : Terms(std::move(Other.Terms))
{
}

Polynomial& Polynomial::operator=(const Polynomial& Other)
{
    if (this == &Other)
        return *this;
    Terms = Other.Terms;
    return *this;
}

Polynomial& Polynomial::operator=(Polynomial&& Other) noexcept
{
    if (this == &Other)
        return *this;
    Terms = std::move(Other.Terms);
    return *this;
}

bool Polynomial::operator==(const Polynomial& Other) const
{
    if (Terms.size() != Other.Terms.size())
    {
        return false;
    }

    for (size_t i = 0; i < Terms.size(); i++)
    {
        if (Terms[i] != Other.Terms[i])
        {
            return false;
        }
    }

    return true;
}

bool Polynomial::operator!=(const Polynomial& Other) const
{
    return !(*this == Other);
}

void Polynomial::AddTerms(
    Polynomial& Result,
    const Polynomial& Other)
{
    // Check if other terms are already in the Result polynomial
    for (const x& OtherTerm : Other.Terms)
    {
        bool bIsTermAlreadyInResult = false;

        for (x& Term : Result.Terms)
        {
            if (Term.GetExponent() == OtherTerm.GetExponent())
            {
                Term = ::AddTerms(Term, OtherTerm);
                bIsTermAlreadyInResult = true;
                break;
            }
        }

        // Add only if unique and non zero
        if (!bIsTermAlreadyInResult && !OtherTerm.IsZero())
        {
            Result.Terms.push_back(OtherTerm);
        }
    }
}

void Polynomial::DistributeTerms(
    Polynomial& Result,
    const Polynomial& Other,
    const BinaryOperationFunction& BinaryOperation)
{
    // Distribute the multiplication/division of two polynomials
    // e.g. (1x2 + 1x3) (1x4 + 1x5) = 1x2 (1x4 + 1x5) + 1x3 (1x4 + 1x5) 
    for (const x& OtherTerm : Other.Terms)
    {
        for (x& Term : Result.Terms)
        {
            Term = BinaryOperation(Term, OtherTerm);
        }
    }
}

Polynomial Polynomial::operator+(const Polynomial& Other) const
{
    if (!Other.HasTerms())
    {
        return Polynomial{ *this };    
    }

    // Result is stored in a new object
    Polynomial Result{ *this };
    AddTerms(Result, Other);
    SortTerms(Result.Terms);
    EraseZeroTerms(Result.Terms);
    return Result;
}

Polynomial Polynomial::operator+(const x& Term) const
{
    if (Term.IsZero())
    {
        return Polynomial{ *this };
    }

    return *this + Polynomial{ Term };
}

Polynomial Polynomial::operator-(const Polynomial& Other) const
{
    return *this + Other;
}

Polynomial Polynomial::operator-(const x& Term) const
{
    return *this - Polynomial{ Term };
}

Polynomial Polynomial::operator*(const Polynomial& Other) const
{
    if (!Other.HasTerms())
    {
        return Polynomial{ *this };    
    }

    // Result is stored in a new object
    Polynomial Result{ *this };
    DistributeTerms(Result, Other, &MultiplyTerms);
    SortTerms(Result.Terms);
    EraseZeroTerms(Result.Terms);
    return Result;
}

Polynomial Polynomial::operator*(const x& Term) const
{
    return *this * Polynomial{ Term };
}

Polynomial Polynomial::operator/(const Polynomial& Other) const
{
    if (!Other.HasTerms())
    {
        return Polynomial{ *this };    
    }

    // Result is stored in a new object
    Polynomial Result{ *this };
    DistributeTerms(Result, Other, &DivideTerms);
    SortTerms(Result.Terms);
    EraseZeroTerms(Result.Terms);
    return Result;
}

Polynomial Polynomial::operator/(const x& Term) const
{
    return *this / Polynomial{ Term };
}

bool Polynomial::HasTerms() const
{
    return !Terms.empty();
}

uint32_t Polynomial::GetDegree() const
{
    if (Terms.empty())
    {
        return 0;
    }

    return Terms[0].GetExponent();    
}

size_t Polynomial::TotalTerms() const
{
    return Terms.size();
}

std::string Polynomial::ToString() const
{
    std::stringstream ss;
    for (const x& Term : Terms)
    {
        bool bIsLastElement = &Term == &*(Terms.end() - 1);
        if (bIsLastElement)
        {
            ss << Term.ToString();
        }
        else
        {
            ss << Term.ToString() << " + ";
        }
    }
    return ss.str();
}

std::string Polynomial::ToDebugString(uint32_t InitialDegree /* = 0 */) const
{
    std::stringstream ss;

    for (uint32_t Index = InitialDegree > 0 ? InitialDegree : GetDegree(); ; --Index)
    {
        // Check if a term of current degree (index) exists
        std::vector<x>::const_iterator FoundTerm = std::find_if(Terms.begin(), Terms.end(),
            [Index](const x& Term)
            {
                return Term.GetExponent() == Index;
            });

        uint32_t Value = FoundTerm != Terms.end() ? 1 : 0;
        ss << Value;

        // Exit if place zero was already added into the string
        if (Index == 0)
        {
            break;
        }
    }

    return ss.str();
}

x::x(uint32_t InExponent)
    : Exponent(InExponent), Coefficient(1)
{}

bool x::operator==(const x& Other) const
{
    return Coefficient == Other.Coefficient && Exponent == Other.Exponent;
}

bool x::operator!=(const x& Other) const
{
    return !(*this == Other);
}

Polynomial x::operator*(const x& Other) const
{
    return Polynomial{ MultiplyTerms(*this, Other) };
}

Polynomial x::operator/(const x& Other) const
{
    // Division is defined as:
    // 0 / 0 = not defined
    // 0 / 1 = 0
    // 1 / 0 = not defined
    // 1 / 1 = 1
    return Polynomial{ DivideTerms(*this, Other) };
}

Polynomial x::operator+(const x& Other) const
{
    // Sum both terms if the degree of the terms is not the same
    if (Exponent != Other.Exponent)
    {
        return Polynomial{ *this, Other };
    }

    return Polynomial{ AddTerms(*this, Other) };
}

Polynomial x::operator-(const x& Other) const
{
    // We define the subtraction as a sum because on the field GF(2) the subtraction is defined as:
    // 0 - 0 = 0
    // 0 - 1 = 1
    // 1 - 0 = 1
    // 1 - 1 = 0
    // And the sum is defined as:
    // 0 + 0 = 0
    // 0 + 1 = 1
    // 1 + 0 = 1
    // 1 + 1 = 0
    return *this + Other;  
}

bool x::IsZero() const
{
    return Coefficient == 0;
}

uint8_t x::GetCoefficient() const
{
    return Coefficient;
}

uint32_t x::GetExponent() const
{
    return Exponent;
}

std::string x::ToString() const
{
    std::stringstream ss;
    ss << static_cast<uint32_t>(Coefficient) << "x^" << Exponent;
    return ss.str();
}

x operator*(uint8_t Coefficient, const x& Right)
{
    x Result{ Right.Exponent };

    // Right.Coefficient is a bitfield of 1 bit
    uint8_t InCoefficientMod2 = Coefficient % 2;
    Result.Coefficient = Right.Coefficient * InCoefficientMod2;

    return Result;
}