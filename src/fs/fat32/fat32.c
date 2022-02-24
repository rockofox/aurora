#include "fat32.h"
#include "../../mem.h"
#include "../mbr.h"

unsigned char buf[512];
fs_node_t *fat32_root = NULL;
uint32_t fat32_get_first_data_sector(fat_BS_t *bs)
{
    uint32_t root_dir_sectors = ((bs->root_entry_count * 32) + (bs->bytes_per_sector - 1)) / bs->bytes_per_sector;
    return bs->reserved_sector_count + (bs->table_count * bs->extended_section.table_size_32) + root_dir_sectors;
}
uint32_t fat32_find_cluster(fat_dir_entry_t dir)
{
    return (dir.cluster_high << 16) | dir.cluster_low;
}

static uint32_t fat32_read(fs_node_t *node, uint8_t *buffer)
{
    if (node->flags | FS_DIRECTORY)
    {
        return 0;
    }
}
static struct dir *fat32_readdir(fs_node_t *node, uint32_t index)
{
    if (!node->flags | FS_DIRECTORY)
    {
        return 0;
    }
}
void fat32_init()
{
    fs_root = (fs_node_t *)alloc_block(sizeof(fs_node_t));
    fs_root->flags = FS_DIRECTORY;
    fs_root->read = &fat32_read;
    fs_root->write = 0;
    fs_root->open = 0;
    fs_root->close = 0;
    fs_root->readdir = &fat32_readdir;
    fs_root->finddir = 0;
    ata_read_sectors(buf, 0, 1);
    uint32_t papos = ((mbr_t *)buf)->partitions[0].first_sector_lba;
    ata_read_sectors(buf, papos, 1);
    ata_read_sectors(buf, papos + fat32_get_first_data_sector(buf), 1);
    // fat_dir_entry_t *de = (fat_dir_entry_t *)(buf + 0x20 * 2);
    // uint32_t i = fat32_find_cluster(*de);
}