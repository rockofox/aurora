#include "fs.h"
#include "../mem.h"

fs_node_t *fs_root = 0;

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    if (node->read)
    {
        return node->read(node, offset, size, buffer);
    }
    else
    {
        return 0;
    }
}
uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    if (node->write)
    {
        return node->write(node, offset, size, buffer);
    }
    else
    {
        return 0;
    }
}
struct dir *readdir_fs(fs_node_t *node, uint32_t index)
{
    if (node->readdir)
    {
        return node->readdir(node, index);
    }
    else
    {
        return 0;
    }
}