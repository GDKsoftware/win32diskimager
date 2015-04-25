
#include "disk.h"
#include "deviceinfo.h"

#include <iostream>

class CProgressBar {
protected:
	INT64 Pos, Max;

public:
	void setRange(INT64 iPos, INT64 iMax) {
		Pos = iPos;
		Max = iMax;
	}

	void setValue(INT64 iPos) {
		Pos = iPos;

		std::wcout << std::to_wstring(Pos) << L" / " << std::to_wstring(Max) << std::endl;
	}
};

class CDiskWriter {
protected:
	enum Status { STATUS_IDLE = 0, STATUS_READING, STATUS_WRITING, STATUS_EXIT, STATUS_CANCELED };

	HANDLE hVolume;
	HANDLE hFile;
	HANDLE hRawDisk;
	char *sectorData;
	unsigned long long sectorsize;
	int status;

	CProgressBar ProgressBar;

	void Sleep(int iDuration) {
		//
	}

public:
	CDiskWriter() {
		hVolume = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		hRawDisk = INVALID_HANDLE_VALUE;
		sectorData = NULL;
		sectorsize = 0ul;
	}

	virtual ~CDiskWriter() {
		if (hRawDisk != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hRawDisk);
			hRawDisk = INVALID_HANDLE_VALUE;
		}
		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}
		if (hVolume != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hVolume);
			hVolume = INVALID_HANDLE_VALUE;
		}
		if (sectorData != NULL)
		{
			delete[] sectorData;
			sectorData = NULL;
		}
	}

	bool FileExists(const std::wstring sFilename) {
		return true;
	}

	void WriteImageToDisk(const std::wstring sFilename, int iDeviceID, int iVolumeID) {
		if (!this->FileExists(sFilename)) {
			throw new std::exception("File not found");
		}

		status = STATUS_WRITING;

		hVolume = getHandleOnVolume(iVolumeID, GENERIC_WRITE);
		if (hVolume == INVALID_HANDLE_VALUE)
		{
			status = STATUS_IDLE;
			return;
		}
		if (!getLockOnVolume(hVolume))
		{
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		if (!unmountVolume(hVolume))
		{
			removeLockOnVolume(hVolume);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		hFile = getHandleOnFile(sFilename.c_str(), GENERIC_READ);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			removeLockOnVolume(hVolume);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		hRawDisk = getHandleOnDevice(iDeviceID, GENERIC_WRITE);
		if (hRawDisk == INVALID_HANDLE_VALUE)
		{
			removeLockOnVolume(hVolume);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hVolume = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			return;
		}

		unsigned long long i, availablesectors, numsectors;

		availablesectors = getNumberOfSectors(hRawDisk, &sectorsize);
		numsectors = getFileSizeInSectors(hFile, sectorsize);
		if (numsectors > availablesectors)
		{
			CUIHelper::critical(L"Not enough available space!");
			return;
		}

		ProgressBar.setRange(0, (numsectors == 0ul) ? 100 : (int)numsectors);
		for (i = 0ul; i < numsectors && status == STATUS_WRITING; i += 1024ul)
		{
			sectorData = readSectorDataFromHandle(hFile, i, (numsectors - i >= 1024ul) ? 1024ul : (numsectors - i), sectorsize);
			if (sectorData == NULL)
			{
				removeLockOnVolume(hVolume);
				CloseHandle(hRawDisk);
				CloseHandle(hFile);
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				hRawDisk = INVALID_HANDLE_VALUE;
				hFile = INVALID_HANDLE_VALUE;
				hVolume = INVALID_HANDLE_VALUE;
				return;
			}
			if (!writeSectorDataToHandle(hRawDisk, sectorData, i, (numsectors - i >= 1024ul) ? 1024ul : (numsectors - i), sectorsize))
			{
				delete[] sectorData;
				removeLockOnVolume(hVolume);
				CloseHandle(hRawDisk);
				CloseHandle(hFile);
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				sectorData = NULL;
				hRawDisk = INVALID_HANDLE_VALUE;
				hFile = INVALID_HANDLE_VALUE;
				hVolume = INVALID_HANDLE_VALUE;
				return;
			}
			delete[] sectorData;
			sectorData = NULL;

			ProgressBar.setValue(i);

			Sleep(1);
		}
		removeLockOnVolume(hVolume);
		CloseHandle(hRawDisk);
		CloseHandle(hFile);
		CloseHandle(hVolume);
		hRawDisk = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		hVolume = INVALID_HANDLE_VALUE;
	}
};


#include <locale>

int wmain(int argc, wchar_t *argv[]) {
	std::locale::global(std::locale(""));

	CDiskWriter writer;
	CDeviceInfo devinfo;
	std::vector<std::pair<wchar_t, ULONG>> v;

	try {
		devinfo.getLogicalDrives(&v);

		std::wstring driveletter, filename;
		if (argc == 3) {
			driveletter = argv[1];
			filename = argv[2];

			int volume = 0;
			int disk = 0;
			for (auto p : v)
			{
				if (p.first == driveletter[0]) {
					disk = p.second;
					volume = p.first - 'A';
				}
			}

			writer.WriteImageToDisk(filename, disk, volume);
		}
		else
		{
			CUIHelper::critical(L"Usage: Win32DiskImagerCLI.exe <driveletter> <filename>");

			return 1;
		}

	}
	catch (std::exception e)
	{
		CUIHelper::critical(e.what());

		return 1;
	}

	return 0;
}
