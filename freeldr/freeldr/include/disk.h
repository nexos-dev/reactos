/*
 *  FreeLoader
 *  Copyright (C) 1998-2002  Brian Palmer  <brianp@sginet.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __DISK_H
#define __DISK_H


typedef struct _GEOMETRY
{
	U32		Cylinders;
	U32		Heads;
	U32		Sectors;
	U32		BytesPerSector;

} GEOMETRY, *PGEOMETRY;

//
// Define the structure of a partition table entry
//
typedef struct _PARTITION_TABLE_ENTRY
{
	U8		BootIndicator;					// 0x00 - non-bootable partition, 0x80 - bootable partition (one partition only)
	U8		StartHead;						// Beginning head number
	U8		StartSector;					// Beginning sector (2 high bits of cylinder #)
	U8		StartCylinder;					// Beginning cylinder# (low order bits of cylinder #)
	U8		SystemIndicator;				// System indicator
	U8		EndHead;						// Ending head number
	U8		EndSector;						// Ending sector (2 high bits of cylinder #)
	U8		EndCylinder;					// Ending cylinder# (low order bits of cylinder #)
	U32		SectorCountBeforePartition;		// Number of sectors preceding the partition
	U32		PartitionSectorCount;			// Number of sectors in the partition

} PACKED PARTITION_TABLE_ENTRY, *PPARTITION_TABLE_ENTRY;

//
// Define the structure of the master boot record
//
typedef struct _MASTER_BOOT_RECORD
{
	U8						MasterBootRecordCodeAndData[0x1be];
	PARTITION_TABLE_ENTRY	PartitionTable[4];
	U16						MasterBootRecordMagic;

} PACKED MASTER_BOOT_RECORD, *PMASTER_BOOT_RECORD;

//
// Partition type defines
//
#define PARTITION_ENTRY_UNUSED          0x00      // Entry unused
#define PARTITION_FAT_12                0x01      // 12-bit FAT entries
#define PARTITION_XENIX_1               0x02      // Xenix
#define PARTITION_XENIX_2               0x03      // Xenix
#define PARTITION_FAT_16                0x04      // 16-bit FAT entries
#define PARTITION_EXTENDED              0x05      // Extended partition entry
#define PARTITION_HUGE                  0x06      // Huge partition MS-DOS V4
#define PARTITION_IFS                   0x07      // IFS Partition
#define PARTITION_OS2BOOTMGR            0x0A      // OS/2 Boot Manager/OPUS/Coherent swap
#define PARTITION_FAT32                 0x0B      // FAT32
#define PARTITION_FAT32_XINT13          0x0C      // FAT32 using extended int13 services
#define PARTITION_XINT13                0x0E      // Win95 partition using extended int13 services
#define PARTITION_XINT13_EXTENDED       0x0F      // Same as type 5 but uses extended int13 services
#define PARTITION_PREP                  0x41      // PowerPC Reference Platform (PReP) Boot Partition
#define PARTITION_LDM                   0x42      // Logical Disk Manager partition
#define PARTITION_UNIX                  0x63      // Unix
#define PARTITION_LINUX_SWAP			0x82      // Linux Swap Partition
#define PARTITION_EXT2					0x83      // Linux Ext2/Ext3

///////////////////////////////////////////////////////////////////////////////////////
//
// BIOS Disk Functions
//
///////////////////////////////////////////////////////////////////////////////////////
int		biosdisk(int cmd, int drive, int head, int track, int sector, int nsects, void *buffer); // Implemented in asmcode.S

BOOL	BiosInt13Read(U32 Drive, U32 Head, U32 Track, U32 Sector, U32 SectorCount, PVOID Buffer); // Implemented in asmcode.S
BOOL	BiosInt13ReadExtended(U32 Drive, U32 Sector, U32 SectorCount, PVOID Buffer); // Implemented in asmcode.S
BOOL	BiosInt13ExtensionsSupported(U32 Drive);
U32		BiosInt13GetLastErrorCode(VOID);

void	StopFloppyMotor(void);		// Implemented in asmcode.S
int		get_heads(int drive);		// Implemented in asmcode.S
int		get_cylinders(int drive);	// Implemented in asmcode.S
int		get_sectors(int drive);		// Implemented in asmcode.S
BOOL	BiosInt13GetDriveParameters(U32 Drive, PGEOMETRY Geometry);	// Implemented in disk.S

///////////////////////////////////////////////////////////////////////////////////////
//
// FreeLoader Disk Functions
//
///////////////////////////////////////////////////////////////////////////////////////
VOID	DiskError(PUCHAR ErrorString);
BOOL	DiskGetDriveGeometry(U32 DriveNumber, PGEOMETRY DriveGeometry);
BOOL	DiskReadLogicalSectors(U32 DriveNumber, U32 SectorNumber, U32 SectorCount, PVOID Buffer);

///////////////////////////////////////////////////////////////////////////////////////
//
// Fixed Disk Partition Management Functions
//
///////////////////////////////////////////////////////////////////////////////////////
BOOL	DiskIsDriveRemovable(U32 DriveNumber);
BOOL	DiskIsDriveCdRom(U32 DriveNumber);
BOOL	DiskGetActivePartitionEntry(U32 DriveNumber, PPARTITION_TABLE_ENTRY PartitionTableEntry);
BOOL	DiskGetPartitionEntry(U32 DriveNumber, U32 PartitionNumber, PPARTITION_TABLE_ENTRY PartitionTableEntry);
BOOL	DiskGetFirstPartitionEntry(PMASTER_BOOT_RECORD MasterBootRecord, PPARTITION_TABLE_ENTRY PartitionTableEntry);
BOOL	DiskGetFirstExtendedPartitionEntry(PMASTER_BOOT_RECORD MasterBootRecord, PPARTITION_TABLE_ENTRY PartitionTableEntry);
BOOL	DiskReadBootRecord(U32 DriveNumber, U32 LogicalSectorNumber, PMASTER_BOOT_RECORD BootRecord);

#endif  // defined __DISK_H
