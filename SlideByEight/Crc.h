#pragma once
#include <cstdint>

// Code structure inspired from Unreal Engine at \Engine\Source\Runtime\Core\Public\Misc\Crc.h
// Bibliography:
// CRC32 Demystified: https://github.com/Michaelangel007/crc32
// A painless guide to CRC algorithms: http://ross.net/crc/download/crc_v3.txt

struct FCrc
{
    /**
     * Lookup table with precalculated CRC values - slicing by 8 implementation
     */
    static uint32_t CRCTablesSB8[8][256];

    /**
     * Initializes the CRC lookup table. Must be called before any of the
     * CRC functions are used.
     */
    static void Init();

    /**
     * Calculate the Crc32 using the polynomial 0x04C11DB7
     * @param Data The data from which to calculate the CRC
     * @param Length The length of data in bytes
     * @param CRC The initial value of the CRC
     * @return 
     */
    static uint32_t MemCrc32(const void* Data, int32_t Length, uint32_t CRC = 0);
};
