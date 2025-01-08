

#include <cstdio>
#include <cstring>

#include "Crc.h"

int main(int argc, char* argv[])
{
    FCrc::Init();

    // Verify online using https://crccalc.com/?crc=Hello%20world&method=CRC-32/ISO-HDLC&datatype=ascii&outtype=hex
    constexpr char Data[] = "Hello world";
    uint32_t CRC = FCrc::MemCrc32(Data, strlen(Data));
    printf("CRC32 = %x\n", CRC);

    return 0;
}
