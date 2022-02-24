#pragma once
#include <stdint.h>

typedef enum
{
    ATA_STATUS_BSY = 0x80,
    ATA_STATUS_RDY = 0x40,
    ATA_STATUS_DRQ = 0x08,
    ATA_STATUS_DF = 0x20,
    ATA_STATUS_ERR = 0x01,
    ATA_STATUS_UNKNOWN = -1
} ATA_Status_t;
void ata_read_sectors(uint64_t *target, uint32_t lba, uint8_t sectors);
void ata_write_sectors(uint32_t lba, uint8_t sectors, uint32_t *bytes);
ATA_Status_t ata_get_status();
void ata_identify(uint16_t *identify);