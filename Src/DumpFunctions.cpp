#include "DumpFunctions.h"

#include <stdio.h>

void printCID(uint8_t * cid)
{
    printf("CID register:\n");
    printf("  Manufacturer ID: %02x\n",  cid[0]);
    printf("  OEM/Application ID: %c%c\n", cid[1], cid[2]);
    printf("  Product Name: %c%c%c%c%c\n", cid[3], cid[4], cid[5], cid[6], cid[7]);
    printf("  Product revision: %02x\n",  cid[8]);
    printf("  Product serial number: %08x\n", (cid[9] << 24) | (cid[10] << 16) | (cid[11] << 8) | cid[12]);
    printf("  Manufacturing date: Year %d Month %d\n", ((cid[13] << 4) & 0xf0) | (cid[14] >> 4 & 0x0f), cid[14] & 0x0f);
}
