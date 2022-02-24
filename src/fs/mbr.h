#pragma once
#include <stdint.h>

typedef struct chs
{
    uint8_t cylinder;
    uint8_t head;
    uint8_t sector;
} __attribute__((packed)) chs_t;
typedef struct mbr_partition
{
    uint8_t status;
    chs_t first_sector;
    uint8_t partition_type;
    chs_t last_sector;
    uint32_t first_sector_lba;
    uint32_t sectors;
} __attribute__((packed)) mbr_partition_t;
typedef struct mbr
{
    uint8_t code[440];
    uint16_t unused;
    uint32_t disk_signature;
    mbr_partition_t partitions[4];
    uint16_t mbr_signature;
} __attribute__((packed)) mbr_t;