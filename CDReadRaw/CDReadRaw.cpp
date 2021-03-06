// CDReadRaw.cpp : Defines the entry point for the console application.
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa363147(v=vs.85).aspx

#include "stdafx.h"
#include <windows.h>
#include <winioctl.h>
#include <ntddcdrm.h>
#include <ntddscsi.h>
#include <stddef.h>

BOOL GetDriveHandle(LPCTSTR lpFileName, HANDLE *hDevice)
{
	// Variables for CreateFile function.
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa363858(v=vs.85).aspx
	DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
	DWORD dwCreationDispostion = OPEN_EXISTING;
	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	HANDLE hTemplateFile = NULL;

	// Retrieve the handle.
	*hDevice = INVALID_HANDLE_VALUE;
	*hDevice = CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDispostion, dwFlagsAndAttributes, hTemplateFile);

	// Check if the returned handle is valid.
	if (*hDevice == INVALID_HANDLE_VALUE)
		return FALSE;
	else
		return TRUE;
}

BOOL GetDriveGeometry(HANDLE hDevice, DISK_GEOMETRY *diskGeo)
{
	BOOL bResult = FALSE;
	DWORD bytesReturned = 0;

	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;

	bResult = DeviceIoControl(hDevice,
		IOCTL_DISK_GET_DRIVE_GEOMETRY,
		NULL, 0,
		diskGeo, sizeof(*diskGeo),
		&bytesReturned,
		NULL);

	return (bResult);
}

BOOL GetDriveRawData(HANDLE hDevice, DISK_GEOMETRY diskGeo)
{
	BOOL bResult = FALSE;
	DWORD bytesReturned = 0;
	RAW_READ_INFO rawReadInfo;

	rawReadInfo.DiskOffset.QuadPart = 0 * diskGeo.BytesPerSector; // Sector # * 2048 | 64-bit integer
	rawReadInfo.SectorCount = 1;
	rawReadInfo.TrackMode = TRACK_MODE_TYPE::RawWithC2AndSubCode;

	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;

	// Create the output buffer.
	

	bResult = DeviceIoControl(
		hDevice,							// Device handle.
		IOCTL_CDROM_RAW_READ,				// Control code.
		&rawReadInfo, sizeof(rawReadInfo),	// Input Buffer.
		&diskGeo, sizeof(diskGeo),			// Output Buffer.
		&bytesReturned,						// Size of data buffer.
		NULL);								// lpOverlapped.

	return bResult;
}

int main(void)
{
	// Device to interact with.
	LPCTSTR lpFileName = L"\\\\.\\E:";
	LPCTSTR lpOutputFilename = L"H:\\Temp\\output.dat";
	HANDLE hDevice = INVALID_HANDLE_VALUE;

	// Geometry of the device.
	DISK_GEOMETRY diskGeo = { 0 };

	// Get the drive handle.
	printf("Get drive handle: ");
	if (GetDriveHandle(lpFileName, &hDevice))
		printf("Success\n");
	else
	{
		printf("Faile\n");
		CloseHandle(hDevice);
		return -1;
	}

	// Get the drive geometry.
	printf("Get drive geometry: ");
	if (GetDriveGeometry(hDevice, &diskGeo))
		printf("Success\n");
	else
	{
		printf("Fail\n");
		CloseHandle(hDevice);
		return -1;
	}

	// Get the raw data from the drive.
	GetDriveRawData(hDevice, diskGeo);
	
	// Pause to display screen before exit.
	CloseHandle(hDevice);
	printf("Press [Enter] to continue.");
	char c;
	//scanf_s(&c);

	return 0;
}

//int main(void)
//{
//	HANDLE fh;
//	DWORD ioctl_bytes;
//	BOOL ioctl_rv;
//	const UCHAR cdb[] = { 0xBE, 0, 0, 0, 0, 1, 0, 0, 1, 0x10, 0, 0 };
//	UCHAR buf[2352];
//	struct sptd_with_sense
//	{
//		SCSI_PASS_THROUGH_DIRECT s;
//		UCHAR sense[128];
//	} sptd;
//
//	fh = CreateFile(L"\\\\.\\E:", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	
//	memset(&sptd, 0, sizeof(sptd));
//	sptd.s.Length = sizeof(sptd.s);
//	sptd.s.CdbLength = sizeof(cdb);
//	sptd.s.DataIn = SCSI_IOCTL_DATA_IN;
//	sptd.s.TimeOutValue = 30;
//	sptd.s.DataBuffer = buf;
//	sptd.s.DataTransferLength = sizeof(buf);
//	sptd.s.SenseInfoLength = sizeof(sptd.sense);
//	sptd.s.SenseInfoOffset = offsetof(struct sptd_with_sense, sense);
//	memcpy(sptd.s.Cdb, cdb, sizeof(cdb));
//
//	ioctl_rv = DeviceIoControl(fh, IOCTL_SCSI_PASS_THROUGH_DIRECT, &sptd, sizeof(sptd), &sptd, sizeof(sptd), &ioctl_bytes, NULL);
//
//	CloseHandle(fh);
//
//    return 0;
//}

