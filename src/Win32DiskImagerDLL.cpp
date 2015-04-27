// Win32DiskImagerDLL.cpp : Defines the exported functions for the DLL application.
//

#include "Win32DiskImagerDLL.h"

#include "ui_helper.h"
#include "deviceinfo.h"
#include "diskwriter.h"

wchar_t *lasterrormsg = nullptr;
wchar_t *availabledrives = nullptr;

bool __stdcall DiskImager_HasError() {
	return CUIHelper::GetInstance()->hasErrors();
}

wchar_t * __stdcall DiskImager_GetError() {
	if (lasterrormsg != nullptr) {
		delete lasterrormsg;
		lasterrormsg = nullptr;
	}

	auto errormsg = CUIHelper::GetInstance()->popErrorMessage();
	
	lasterrormsg = new wchar_t[errormsg.length() + 1];
	lasterrormsg[errormsg.length()] = 0x00;

	memcpy(lasterrormsg, errormsg.c_str(), errormsg.length());

	return lasterrormsg;
}

wchar_t * __stdcall DiskImager_GetAvailableDrives() {
	if (availabledrives != nullptr) {
		delete availabledrives;
		availabledrives = nullptr;
	}

	CDeviceInfo devinfo;
	std::vector<std::pair<wchar_t, ULONG>> v;

	devinfo.getLogicalDrives(&v);

	availabledrives = new wchar_t[v.size() + 1];
	availabledrives[v.size()] = 0x00;

	size_t idxChar = 0;
	for (auto p : v)
	{
		availabledrives[idxChar] = p.first;
		++idxChar;
	}

	return availabledrives;
}

bool __stdcall DiskImager_WriteToDisk(const wchar_t driveletter, const wchar_t *filename) {
	CDiskWriter writer;
	CDeviceInfo devinfo;
	std::vector<std::pair<wchar_t, ULONG>> v;

	devinfo.getLogicalDrives(&v);

	int volume = 0;
	int disk = 0;
	bool found = false;
	for (auto p : v)
	{
		if (p.first == driveletter) {
			disk = p.second;
			volume = p.first - 'A';
			found = true;
		}
	}

	if (found) {
		return writer.WriteImageToDisk(filename, disk, volume);
	}
	else
	{
		CUIHelper::critical(L"Selected disk not supported");

		return false;
	}
}

void __stdcall DiskImager_Fini() {
	if (lasterrormsg != nullptr) {
		delete lasterrormsg;
		lasterrormsg = nullptr;
	}

	if (availabledrives != nullptr) {
		delete availabledrives;
		availabledrives = nullptr;
	}
}
