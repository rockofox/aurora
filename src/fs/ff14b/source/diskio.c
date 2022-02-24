/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"		/* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "../../../drivers/ata/ata.h"
#include "../../../serial.h"

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	serial_println("disk_status");
	ATA_Status_t status = ata_get_status();
	if (status == ATA_STATUS_BSY)
	{
		return STA_NOINIT;
	}
	else if (status == ATA_STATUS_RDY)
	{
		return 0;
	}
	else if (status == ATA_STATUS_DRQ)
	{
		return 0;
	}
	else if (status == ATA_STATUS_DF)
	{
		return STA_PROTECT;
	}
	else if (status == ATA_STATUS_ERR)
	{
		return STA_PROTECT;
	}
	else
	{
		return STA_PROTECT;
	}
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	serial_println("disk_initialize");
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
	BYTE pdrv,	  /* Physical drive nmuber to identify the drive */
	BYTE *buff,	  /* Data buffer to store read data */
	LBA_t sector, /* Start sector in LBA */
	UINT count	  /* Number of sectors to read */
)
{
	serial_printf("disk_read: %d %d %d\n", pdrv, sector, count);
	ata_read_sectors(buff, sector, count);
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
	BYTE pdrv,		  /* Physical drive nmuber to identify the drive */
	const BYTE *buff, /* Data to be written */
	LBA_t sector,	  /* Start sector in LBA */
	UINT count		  /* Number of sectors to write */
)
{

	serial_println("disk_write");
	ata_write_sectors(sector, count, (uint32_t *)buff);

	return RES_OK;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
	BYTE pdrv, /* Physical drive nmuber (0..) */
	BYTE cmd,  /* Control code */
	void *buff /* Buffer to send/receive control data */
)
{
	DRESULT res;
	uint32_t result;
	serial_println("disk_ioctl");
	uint16_t identify[256] = {0};
	ata_identify(identify);
	switch (cmd)
	{
	case GET_SECTOR_COUNT:
		result = identify[60];
		result |= identify[61] << 8;
		*(DWORD *)buff = result;
		res = RES_OK;
		break;
	case GET_BLOCK_SIZE:
		*(WORD *)buff = 512;
		break;
	case CTRL_SYNC:
		res = RES_OK;
		break;
	default:
		res = RES_PARERR;
		serial_println("disk_ioctl: unknown command");
		break;
	}
	return RES_OK;
}
