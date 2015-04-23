#include "deviceinfo.h"

#include <windows.h>
#include "disk.h"

CDeviceInfo::CDeviceInfo()
{
}


CDeviceInfo::~CDeviceInfo()
{
}

// getLogicalDrives sets cBoxDevice with any logical drives found, as long
// as they indicate that they're either removable, or fixed and on USB bus
void CDeviceInfo::getLogicalDrives(std::vector<std::pair<wchar_t,ULONG>> *vDrives)
{
	// GetLogicalDrives returns 0 on failure, or a bitmask representing
	// the drives available on the system (bit 0 = A:, bit 1 = B:, etc)
	unsigned long driveMask = ::GetLogicalDrives();
	int i = 0;
	ULONG pID;

	while (driveMask != 0)
	{
		if (driveMask & 1)
		{
			// the "A" in drivename will get incremented by the # of bits
			// we've shifted
			std::wstring drivename = L"\\\\.\\A:\\";
			drivename[4] += i;

			if (::checkDriveType(drivename, &pID))
			{
				std::pair<wchar_t, ULONG> drive;
				drive = std::make_pair(drivename[4], pID);
				vDrives->push_back(drive);
			}
		}
		driveMask >>= 1;
		++i;
	}
}
