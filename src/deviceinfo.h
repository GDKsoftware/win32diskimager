#pragma once

#include <vector>
#include <windows.h>

class CDeviceInfo
{
public:
	CDeviceInfo();
	virtual ~CDeviceInfo();

	void getLogicalDrives(std::vector<std::pair<wchar_t, ULONG>> *vDrives);
};

