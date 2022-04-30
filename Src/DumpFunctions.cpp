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

    printf("RAW data:\n");
    printf("  %02x %02x %02x %02x\n", cid[0], cid[1], cid[2], cid[3]);
    printf("  %02x %02x %02x %02x\n", cid[4], cid[5], cid[6], cid[7]);
    printf("  %02x %02x %02x %02x\n", cid[8], cid[9], cid[10], cid[11]);
    printf("  %02x %02x %02x %02x\n", cid[12], cid[13], cid[14], cid[15]);

}

void printCSDv1(uint8_t * csd)
{
    printf("  Device read access time 1 (TAAC): %02x\n", csd[1]);
    printf("  Device read access time 2 (NSAC): %02x\n", csd[2]);
    printf("  Max data transfer rate (TRAN_SPEED): %02x\n", csd[3]);
    printf("  Card command classes (CCC): %03x\n", csd[4] << 4 | csd[5] >> 4);
    printf("  Max read data block length (READ_BL_LEN): %d\n", 1 << (csd[5] & 0x0f));
    printf("  Partial block read allowed (READ_BL_PARTIAL): %d\n", (csd[6] >> 7) & 0x01);
    printf("  Write block misalignment (WRITE_BL_MISALIGN): %d\n", (csd[6] >> 6) & 0x01);
    printf("  Read block misalignment (READ_BL_MISALIGN): %d\n", (csd[6] >> 5) & 0x01);
    printf("  DSR Implemented (DSR_IMP): %d\n", (csd[6] >> 4) & 0x01);
    printf("  Device size (C_SIZE): %d\n", ((csd[6] & 0x03) << 10) | (csd[7] << 2) | ((csd[8] >> 6) & 0x03) );
    printf("  Max read current min (VDD_R_CURR_MIN): %d\n", (csd[8] >> 3) & 0x7 );
    printf("  Max read current max (VDD_R_CURR_MAX): %d\n", csd[8] & 0x7 );
    printf("  Max write current min (VDD_W_CURR_MIN): %d\n", (csd[9] >> 5) & 0x7 );
    printf("  Max write current max (VDD_W_CURR_MAX): %d\n", (csd[9] >> 2) & 0x7 );
    printf("  Device size multiplier (C_SIZE_MULT): %d\n", ((csd[9] << 1) & 0x6) | ((csd[10] >> 7) & 0x1));
    printf("  Erase single block enabled (ERASE_BLK_EN): %d\n", (csd[10] >> 6) & 0x1);
    printf("  Erase sector size (SECTOR_SIZE): %02x\n", ((csd[10] & 0x3f) << 1) | ((csd[11] >> 7) & 0x1));
    printf("  Write protect group size (WP_GRP_SIZE): %02x\n", csd[11] & 0x7f);
    printf("  Write protect group enable (WP_GRP_ENABLE): %d\n", (csd[12] >> 7) & 0x1);
    printf("  Write speed factor (R2W_FACTOR): %02x\n", (csd[12] >> 2) & 0x7);
    printf("  Max write data block length (WRITE_BL_LEN): %d\n", 1 << (((csd[12] & 0x3) << 2) | ((csd[13] >> 6) & 0x3)));
    printf("  Partial blocks for write allowed (WRITE_BL_PARTIAL): %d\n", (csd[13] >> 5) & 0x1);
    printf("  File format group (FILE_FORMAT_GRP): %d\n", (csd[14] >> 7) & 0x1);
    printf("  Copy flag (COPY): %d\n", (csd[14] >> 6) & 0x1);
    printf("  Permanent write protection (PERM_WRITE_PROTECT): %d\n", (csd[14] >> 5) & 0x1);
    printf("  Temporary write protection (TMP_WRITE_PROTECT): %d\n", (csd[14] >> 4) & 0x1);
    printf("  File format (FILE_FORMAT): %02x\n", (csd[14] >> 2) & 0x3 );

    // Calculate card capacity
    uint8_t read_bl_len = csd[5] & 0x0f;
    uint16_t c_size = ((csd[6] & 0x03) << 10) | (csd[7] << 2) | ((csd[8] >> 6) & 0x03);
    uint8_t c_size_mult = ((csd[9] << 1) & 0x3) | ((csd[10] >> 7) & 0x1);

    uint32_t capacity = (uint32_t)(c_size + 1) << (c_size_mult + read_bl_len - 7);
    printf("\n");
    printf("  Calculated capacity: %lu\n", capacity);
}

void printCSDv2(uint8_t * csd)
{
    printf("  Device read access time 1 (TAAC): %02x\n", csd[1]);
    printf("  Device read access time 2 (NSAC): %02x\n", csd[2]);
    printf("  Max data transfer rate (TRAN_SPEED): %02x\n", csd[3]);
    printf("  Card command classes (CCC): %03x\n", csd[4] << 4 | csd[5] >> 4);
    printf("  Max read data block length (READ_BL_LEN): %d\n", 1 << (csd[5] & 0x0f));
    printf("  Partial block read allowed (READ_BL_PARTIAL): %d\n", (csd[6] >> 7) & 0x01);
    printf("  Write block misalignment (WRITE_BL_MISALIGN): %d\n", (csd[6] >> 6) & 0x01);
    printf("  Read block misalignment (READ_BL_MISALIGN): %d\n", (csd[6] >> 5) & 0x01);
    printf("  DSR Implemented (DSR_IMP): %d\n", (csd[6] >> 4) & 0x01);
    printf("  Device size (C_SIZE): %d\n", csd[7] << 16 | csd[8] << 8 | csd[9]);
    printf("  Erase single block enabled (ERASE_BLK_EN): %d\n", (csd[10] >> 6) & 0x1);
    printf("  Erase sector size (SECTOR_SIZE): %02x\n", ((csd[10] & 0x3f) << 1) | ((csd[11] >> 7) & 0x1));
    printf("  Write protect group size (WP_GRP_SIZE): %02x\n", csd[11] & 0x7f);
    printf("  Write protect group enable (WP_GRP_ENABLE): %d\n", (csd[12] >> 7) & 0x1);
    printf("  Write speed factor (R2W_FACTOR): %02x\n", (csd[12] >> 2) & 0x7);
    printf("  Max write data block length (WRITE_BL_LEN): %d\n", 1 << (((csd[12] & 0x3) << 2) | ((csd[13] >> 6) & 0x3)));
    printf("  Partial blocks for write allowed (WRITE_BL_PARTIAL): %d\n", (csd[13] >> 5) & 0x1);
    printf("  File format group (FILE_FORMAT_GRP): %d\n", (csd[14] >> 7) & 0x1);
    printf("  Copy flag (COPY): %d\n", (csd[14] >> 6) & 0x1);
    printf("  Permanent write protection (PERM_WRITE_PROTECT): %d\n", (csd[14] >> 5) & 0x1);
    printf("  Temporary write protection (TMP_WRITE_PROTECT): %d\n", (csd[14] >> 4) & 0x1);
    printf("  File format (FILE_FORMAT): %02x\n", (csd[14] >> 2) & 0x3 );

    // Calculate card capacity
    uint32_t capacity = ((csd[7] << 16 | csd[8] << 8 | csd[9]) + 1) * 512; // (C_SIZE+1)*512Kb
    printf("\n");
    printf("  Calculated capacity: %lu Kb\n", capacity);
}

void printCSD(uint8_t * csd)
{
    printf("CSD register:\n");
    uint8_t version = csd[0] >> 6 & 0xff;
    printf("  CSD Version: %d.0\n", version + 1);

    if(version == 0x01)
        printCSDv2(csd);
    if(version == 0x00)
        printCSDv1(csd);

    printf("RAW data:\n");
    printf("  %02x %02x %02x %02x\n", csd[0], csd[1], csd[2], csd[3]);
    printf("  %02x %02x %02x %02x\n", csd[4], csd[5], csd[6], csd[7]);
    printf("  %02x %02x %02x %02x\n", csd[8], csd[9], csd[10], csd[11]);
    printf("  %02x %02x %02x %02x\n", csd[12], csd[13], csd[14], csd[15]);
}
