#include "ata.h"
#include "../../io.h"
#include "../../serial.h"

#define ATA_PORT_STATUS 0x1F7
#define ATA_PORT_DATA 0x1F0
#define ATA_PORT_DRIVE 0x1F6
#define ATA_PORT_CMD 0x1F7
#define ATA_PORT_FEATURE 0x3F6
#define ATA_PORT_SECCOUNT 0x1F2
#define ATA_PORT_LBA0 0x1F3
#define ATA_PORT_LBA1 0x1F4
#define ATA_PORT_LBA2 0x1F5

#define ATA_COMMAND_READ_PIO 0x20
#define ATA_COMMAND_WRITE_PIO 0x30
#define ATA_COMMAND_IDENTIFY 0xEC

void ata_read_sectors(uint64_t *target, uint32_t lba, uint8_t sectors)
{
    ata_wait_bsy();
    outb(ATA_PORT_DRIVE, 0xE0 | ((lba >> 24) & 0xF));
    outb(ATA_PORT_SECCOUNT, sectors);
    outb(ATA_PORT_LBA0, lba);
    outb(ATA_PORT_LBA1, lba >> 8);
    outb(ATA_PORT_LBA2, lba >> 16);
    outb(ATA_PORT_CMD, ATA_COMMAND_READ_PIO);
    uint16_t *tmp_target = (uint16_t *)target;
    for (uint32_t i = 0; i < sectors; i++)
    {
        ata_wait_bsy();
        ata_wait_drq();
        for (int j = 0; j < 256; j++)
        {
            tmp_target[j] = inw(0x1F0);
        }
        tmp_target += 256;
    }
    serial_printf("Read %d sectors from LBA %d\n", sectors, lba);
}
void ata_write_sectors(uint32_t lba, uint8_t sectors, uint32_t *bytes)
{
    ata_wait_bsy();
    outb(ATA_PORT_DRIVE, 0xE0 | ((lba >> 24) & 0xF));
    outb(ATA_PORT_SECCOUNT, sectors);
    outb(ATA_PORT_LBA0, lba);
    outb(ATA_PORT_LBA1, lba >> 8);
    outb(ATA_PORT_LBA2, lba >> 16);
    outb(ATA_PORT_CMD, ATA_COMMAND_WRITE_PIO);

    for (uint32_t i; i < sectors; i++)
    {
        ata_wait_bsy();
        ata_wait_drq();

        for (int j = 0; j < 256; j++)
        {
            outl(ATA_PORT_DATA, bytes[j]);
        }
        // bytes += 256;
    }
}
void ata_wait_bsy()
{
    while (inb(ATA_PORT_STATUS) & ATA_STATUS_BSY)
        ;
}
void ata_wait_drq()
{
    while (!(inb(ATA_PORT_STATUS) & ATA_STATUS_RDY))
        ;
}
ATA_Status_t ata_get_status()
{
    uint8_t status = inb(ATA_PORT_STATUS);
    if (status & ATA_STATUS_BSY)
    {
        return ATA_STATUS_BSY;
    }
    else if (status & ATA_STATUS_RDY)
    {
        return ATA_STATUS_RDY;
    }
    else if (status & ATA_STATUS_DRQ)
    {
        return ATA_STATUS_DRQ;
    }
    else if (status & ATA_STATUS_DF)
    {
        return ATA_STATUS_DF;
    }
    else if (status & ATA_STATUS_ERR)
    {
        return ATA_STATUS_ERR;
    }
    else
    {
        return ATA_STATUS_UNKNOWN;
    }
}
void ata_identify(uint16_t *identify)
{
    ata_wait_bsy();
    outb(ATA_PORT_DRIVE, 0xE0);
    outb(ATA_PORT_CMD, ATA_COMMAND_IDENTIFY);
    ata_wait_drq();
    for (int i = 0; i < 256; i++)
    {
        identify[i] = inw(ATA_PORT_DATA);
    }
}