#include "Polynomial.h"
#include "Tests.h"

void DoExample1();
void DoExample2();
void DoExample3();
void DoExample4();
void DoExample5();

int main(int argc, char* argv[])
{
    // DoExample1();
    // DoExample2();
    // DoExample3();
    // DoExample4();
    DoExample5();
    return 0;
}

/**
 * The message M(x) is 8bits long, the CRC G(x) is 9bits long,
 * Which means, degree of M and G is 7 and 8 respectively
 * The CRC is considered CRC8 because it will lead to 8 bits in the remainder,
 * Note that an n-bit binary CRC, requires a n+1 bit pattern
 */
void DoExample1()
{
    // For illustration, we will use the CRC-8-ATM (HEC) https://en.wikipedia.org/wiki/CRC-based_framing
    // x8 + x2 + x + 1
    Polynomial Crc8Atm = Polynomial::FromBitString(PolynomialBitString{0b1'0000'0111, 9});
    printf("Crc8Atm = %s [%s]\n", Crc8Atm.ToDebugString().c_str(), Crc8Atm.ToString().c_str());
    
    // The example message
    PolynomialBitString BitString{ 0b0101'0111, 8 };
    Polynomial Remainder = Polynomial::FromBitString(BitString);
    printf("InitialMessage = %s [%s]\n\n", Remainder.ToDebugString().c_str(), Remainder.ToString().c_str());

    // Message lenght is 8bits
    assert(BitString.GetLength() == 8); // Message length must be 8

    for (size_t Index = 0; Index < BitString.GetLength(); ++Index)
    {
        Remainder = Remainder * x{ 1 };
        printf("%s - [%s]\n",
            Remainder.ToDebugString(Crc8Atm.GetDegree()).c_str(),
            Remainder.ToString().c_str());
    
        if (Remainder.GetDegree() == Crc8Atm.GetDegree())
        {
            Remainder = Remainder - Crc8Atm;
            printf("%s [%s]\n------------------------\n%s [%s]\n\n",
                Crc8Atm.ToDebugString(Crc8Atm.GetDegree()).c_str(),
                Crc8Atm.ToString().c_str(),
                Remainder.ToDebugString(Crc8Atm.GetDegree()).c_str(),
                Remainder.ToString().c_str());
        }
        else
        {
            printf("%s\n------------------------\n%s [%s]\n\n",
                Polynomial::Zero().ToDebugString(Crc8Atm.GetDegree()).c_str(),
                Remainder.ToDebugString(Crc8Atm.GetDegree()).c_str(),
                Remainder.ToString().c_str());
        }
    }
}

/**
 * The message M(x) is 14bits long, the CRC G(x) is 4bits long,
 * Which means, degree of M and G is 13 and 3 respectively
 * The CRC is considered CRC3 because it will lead to 3 bits in the remainder.
 * Note that an n-bit binary CRC, requires a n+1 bit pattern
 */
void DoExample2()
{
    // For illustration, we will follow the example in wikipedia here https://en.wikipedia.org/wiki/Cyclic_redundancy_check#Computation
    // x3 + x + 1
    Polynomial Crc3 = Polynomial::FromBitString(PolynomialBitString{ 0b1011, 4 });
    printf("Crc3 = %s [%s]\n", Crc3.ToDebugString().c_str(), Crc3.ToString().c_str());

    // The example message
    PolynomialBitString Message{ 0b11'0100'1110'1100, 14 };
    printf("Message = %s\n", Message.ToString().c_str());

    // We first get n bits of the message where n is the degree of the CRC,
    // or equivalently the size of the final sequence of bits which is the n in the function name CRCn
    Polynomial Remainder = Polynomial::FromBitString(Message.Substring(Crc3.GetDegree()));
    printf("InitialMessage = %s [%s]\n\n", Remainder.ToDebugString().c_str(), Remainder.ToString().c_str());
    
    for (uint32_t Index = 0; Index < Message.GetLength(); ++Index)
    {
        Remainder = Remainder * x{ 1 } + Message[Crc3.GetDegree() + Index] * x{ 0 };
        printf("%s - [%s]\n",
            Remainder.ToDebugString(Crc3.GetDegree()).c_str(),
            Remainder.ToString().c_str());
    
        if (Remainder.GetDegree() == Crc3.GetDegree())
        {
            Remainder = Remainder - Crc3;
            printf("%s [%s]\n------------------------\n%s [%s]\n\n",
                Crc3.ToDebugString(Crc3.GetDegree()).c_str(),
                Crc3.ToString().c_str(),
                Remainder.ToDebugString(Crc3.GetDegree()).c_str(),
                Remainder.ToString().c_str());
        }
        else
        {
            printf("%s\n------------------------\n%s [%s]\n\n",
                Polynomial::Zero().ToDebugString(Crc3.GetDegree()).c_str(),
                Remainder.ToDebugString(Crc3.GetDegree()).c_str(),
                Remainder.ToString().c_str());
        }
    }
}

/**
 * Example where (1) we calculate the xor one bit at a time, (2) we don't have to load a substring of n elements nor
 * (3) need to have n elements in the remainder register, n-1 is enough 
 */
void DoExample3()
{
    // For illustration, we will follow the example in wikipedia here https://en.wikipedia.org/wiki/Cyclic_redundancy_check#Computation
    // x3 + x + 1
    Polynomial Crc3 = Polynomial::FromBitString(PolynomialBitString{ 0b1'1011'0101, 9 });
    printf("Crc3 = %s [%s]\n", Crc3.ToDebugString().c_str(), Crc3.ToString().c_str());

    // The example message
    PolynomialBitString Message{ 0b11'0100'1110'1100, 14 };
    printf("Message = %s\n", Message.ToString().c_str());

    Polynomial Remainder = Polynomial::Zero();
    printf("InitialMessage = %s [%s]\n\n", Remainder.ToDebugString().c_str(), Remainder.ToString().c_str());
    
    for (uint32_t Index = 0; Index < Message.GetLength(); ++Index)
    {
        Remainder = Remainder + Message[Index] * x{ Crc3.GetDegree() - 1 };
        printf("%s - [%s]\n",
            Remainder.ToDebugString(Crc3.GetDegree()).c_str(),
            Remainder.ToString().c_str());
    
        if (Remainder.GetDegree() == (Crc3.GetDegree() - 1))
        {
            Remainder = Remainder * x{ 1 } - Crc3;
            printf("%s [%s]\n------------------------\n%s [%s]\n\n",
                Crc3.ToDebugString(Crc3.GetDegree()).c_str(),
                Crc3.ToString().c_str(),
                Remainder.ToDebugString(Crc3.GetDegree()).c_str(),
                Remainder.ToString().c_str());
        }
        else
        {
            Remainder = Remainder * x{ 1 };
            printf("%s\n------------------------\n%s [%s]\n\n",
                Polynomial::Zero().ToDebugString(Crc3.GetDegree()).c_str(),
                Remainder.ToDebugString(Crc3.GetDegree()).c_str(),
                Remainder.ToString().c_str());
        }
    }
}

/**
 * CRC 1byte (8bits) at a time
 */
void DoExample4()
{
    Polynomial Crc8 = Polynomial::FromBitString(PolynomialBitString{ 0b1'1011'0101, 9 });
    printf("Crc3 = %s [%s]\n", Crc8.ToDebugString().c_str(), Crc8.ToString().c_str());

    // The example message
    PolynomialBitString Message{ 0b0011'0100'1110'1100, 16 };
    printf("Message = %s\n", Message.ToString().c_str());

    Polynomial Remainder = Polynomial::Zero();
    printf("InitialMessage = %s [%s]\n\n", Remainder.ToDebugString().c_str(), Remainder.ToString().c_str());

    uint32_t ByteSize = 8; // Size of byte in bits
    uint32_t TotalBytes = Message.GetLength() / ByteSize;
    for (uint32_t ByteIndex = 0; ByteIndex < TotalBytes; ++ByteIndex)
    {
        Remainder = Remainder + Polynomial::FromBitString(Message.Substring(ByteIndex * ByteSize, 8)) * x{ Crc8.GetDegree() - ByteSize };
        printf("%s - [%s] byte(%d)\n",
            Remainder.ToDebugString(Crc8.GetDegree()).c_str(),
            Remainder.ToString().c_str(),
            ByteIndex);

        for (uint32_t BitIndex = 0; BitIndex < ByteSize; ++BitIndex)
        {
            if (Remainder.GetDegree() == (Crc8.GetDegree() - 1))
            {
                Remainder = Remainder * x{ 1 } - Crc8;
            }
            else
            {
                Remainder = Remainder * x{ 1 };
            }

            printf("%s - [%s]\n",
                Remainder.ToDebugString(Crc8.GetDegree()).c_str(),
                Remainder.ToString().c_str());
        }
    }
}

/**
 * CRC implementation using binary and MSB represent the highest degree coefficient, test online using https://www.sunshine2k.de/coding/javascript/crc/crc_js.html
 */
void DoExample5()
{
    // Binary message 00010000 00010001 00010010 00010011 00010100 00010101 00010110 00010111
    constexpr uint8_t Message[]{ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 };
    constexpr uint32_t TotalBytes = std::size(Message);

    // Most significant bit first (big-endian), note that the x power n bit is ignored, aka the coefficient of the msb bit
    // (x16)+x12+x5+1 = (1) 0001 0000 0010 0001 = 0x1021
    constexpr uint32_t Crc = 0x1021;
    constexpr uint32_t CrcN = 16;

    // Result of the CRC function
    uint32_t Remainder = 0;

    for (uint8_t MessageByte : Message)
    {
        // Align the message byte with the generator polynomial e.g. make 0x12 to be 0x1200 so it represents a polynomial of degree n-1
        uint32_t ShiftLeft = CrcN - TotalBytes;
        uint32_t MessageByteShifted = MessageByte << ShiftLeft;
        printf("%x << %d == %x\n", MessageByte, ShiftLeft, MessageByteShifted);

        // Calculate the remainder
        Remainder = Remainder ^ MessageByteShifted;

        for (uint32_t BitIndex = 0; BitIndex < 8; ++BitIndex)
        {
            // Check the coefficient of polynomial n-1, i.e. 1000 0000 0000 0000
            if (Remainder & 0x8000)
            {
                Remainder = (Remainder << 1) ^ Crc;
            }
            else
            {
                Remainder = Remainder << 1;
            }
        }

        // We need to mask 16bits on the remainder because we are ignoring the n coefficient of the crc polynomial
        // so we are not guaranteeing the n coefficient at remainder and crc to cancel each other
        Remainder = Remainder & 0xffff;

        printf("Remainder = %x\n", Remainder);
    }
}


/**
 * CRC implementation using binary and LSB represent the highest degree coefficient, test online using https://www.sunshine2k.de/coding/javascript/crc/crc_js.html
 */
void DoExample6()
{
    // Binary message 00010000 00010001 00010010 00010011 00010100 00010101 00010110 00010111
    constexpr uint8_t Message[]{ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 };

    // Most significant bit first (big-endian), note that the x power n bit is ignored, aka the coefficient of the msb bit
    // (x16)+x12+x5+1 = (1) 0001 0000 0010 0001 = 0x1021
    // Least significant bit first (little-endian)
    // 1+x5+x12+(x16) = 1000 0100 0000 1000 (1) = 0x8408
    constexpr uint32_t Crc = 0x8408;

    // Result of the CRC function
    uint32_t Remainder = 0;
    for (uint8_t MessageByte : Message)
    {
        // Calculate the remainder
        Remainder = Remainder ^ MessageByte;

        for (uint32_t BitIndex = 0; BitIndex < 8; ++BitIndex)
        {
            // Check the coefficient of polynomial n-1, i.e. 1000 0000 0000 0000
            if (Remainder & 0x1)
            {
                Remainder = (Remainder >> 1) ^ Crc;
            }
            else
            {
                Remainder = Remainder >> 1;
            }
        }

        // We need to mask 16bits on the remainder because we are ignoring the n coefficient of the crc polynomial
        // so we are not guaranteeing the n coefficient at remainder and crc to cancel each other
        // Remainder = Remainder & 0xffff;

        printf("Remainder = %x\n", Remainder);
    }
}
