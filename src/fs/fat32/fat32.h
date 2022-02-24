#pragma once
#include <stdint.h>
#include "../fs.h"

typedef struct fat_extBS_32
{
    //extended fat32 stuff
    unsigned int table_size_32;
    unsigned short extended_flags;
    unsigned short fat_version;
    unsigned int root_cluster;
    unsigned short fat_info;
    unsigned short backup_BS_sector;
    unsigned char reserved_0[12];
    unsigned char drive_number;
    unsigned char reserved_1;
    unsigned char boot_signature;
    unsigned int volume_id;
    unsigned char volume_label[11];
    unsigned char fat_type_label[8];

} __attribute__((packed)) fat_extBS_32_t;

typedef struct fat_BS
{
    unsigned char bootjmp[3];
    unsigned char oem_name[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sector_count;
    unsigned char table_count;
    unsigned short root_entry_count;
    unsigned short total_sectors_16;
    unsigned char media_type;
    unsigned short table_size_16;
    unsigned short sectors_per_track;
    unsigned short head_side_count;
    unsigned int hidden_sector_count;
    unsigned int total_sectors_32;

    // unsigned char extended_section[54];
    fat_extBS_32_t extended_section;

} __attribute__((packed)) fat_BS_t;
typedef struct fat_dir_entry
{
    char filename[8];
    char ext[3];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creation_time_secs;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t cluster_high;
    uint16_t mod_time;
    uint16_t mod_date;
    uint16_t cluster_low;
    uint32_t size;
} __attribute__((packed)) fat_dir_entry_t;

uint32_t fat32_get_first_data_sector(fat_BS_t *bs);
uint32_t fat32_find_cluster(fat_dir_entry_t dir);

extern fs_node_t *fat32_root;