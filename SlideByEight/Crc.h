#pragma once
#include <cstdint>

// Code structure inspired from Unreal Engine at \Engine\Source\Runtime\Core\Public\Misc\Crc.h
// Bibliography:
// - CRC32 Demystified: https://github.com/Michaelangel007/crc32
// - A painless guide to CRC algorithms: http://ross.net/crc/download/crc_v3.txt
// - Programmer notes on CRC computation: https://rawsourcecode.io/posts/programmer-notes-on-crc-computation

struct FCrc
{
    /**
     * Lookup table with precalculated CRC values - slicing by 8 implementation
     * Romu: Algorithm proposed at paper "A systematic approach to building high performance, software based, CRC generators By Michael E. Kounavis and Frank L. Berry"
     */
    static uint32_t CRCTablesSB8[8][256];

    /**
     * Initializes the CRC lookup table. Must be called before any of the CRC functions are used.
     * Romu: Currently this doesn't really initialize anything, it's used for validations, and the tables are initialized in a hardcoded table
     */
    static void Init();

    /**
     * Calculate the Crc32 using the polynomial 0x04C11DB7, this follows the algorithm stated in the following standards
     * CRC-32/ISO-HDLC, CRC-32, CRC-32/ADCCP, CRC-32/V-42, CRC-32/XZ, PKZIP, see wikipedia article https://en.wikipedia.org/wiki/High-Level_Data_Link_Control
     * In other words Unreal uses the same generator used by OSI Layer 2 (Data Link Layer)
     * 
     * Verify results online using the following calculator: https://crccalc.com/?crc=Hello%20world&method=CRC-32/ISO-HDLC&datatype=ascii&outtype=hex
     * 
     * @param Data The data from which to calculate the CRC
     * @param Length The length of data in bytes
     * @param CRC The initial value of the CRC
     * @return The calculated CRC value
     */
    static uint32_t MemCrc32(const void* Data, int32_t Length, uint32_t CRC = 0);
};
