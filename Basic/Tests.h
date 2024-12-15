#pragma once

#include <assert.h>
#include <cstdio>
#include "Polynomial.h"

struct Tests
{
    Tests()
    {
        printf("Tests Started:\n");

        // Test that mod2 is performed on coefficient
        Polynomial SumA = 4 * x{7} + 3 * x{ 5 } + 5 * x{7} + 3 * x{ 8 } + 2 * x{ 1 } + 1 * x{ 0 };
        assert(SumA == x{8} + x{7} + x{5} + x{0});

        // Test that sub of two polynomials is correct
        Polynomial SumB = 1 * x{ 7 } + 1 * x{ 5 };
        Polynomial SumResult = SumA - SumB;
        assert(SumResult == x{8} + x{0});
    
        printf(
            "(%s) - (%s) = %s\n",
            SumA.ToString().c_str(),
            SumB.ToString().c_str(),
            SumResult.ToString().c_str());

        // Test that product of two polynomials is correct
        Polynomial FactorA = x{7} + x{5} + x{0};
        Polynomial FactorB = 1 * x{ 1 };
        Polynomial Multiplication = FactorA * FactorB;
        assert(Multiplication == x{8} + x{6} + x{1});

        printf(
            "(%s) * (%s) = %s\n",
            FactorA.ToString().c_str(),
            FactorB.ToString().c_str(),
            Multiplication.ToString().c_str());

        printf("Tests finished\n\n");
    }
};

static Tests DoTests;
