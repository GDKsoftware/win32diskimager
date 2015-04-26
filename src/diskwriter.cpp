
#include "diskwriter.h"
#include "disk.h"

void CProgressBar::setRange(INT64 iPos, INT64 iMax) {
	Max = iMax;
	Pos = iPos;
}

void CProgressBar::setValue(INT64 iPos) {
	Pos = iPos;
}


CDiskWriter::CDiskWriter() {
	hVolume = INVALID_HANDLE_VALUE;
	hFile = INVALID_HANDLE_VALUE;
	hRawDisk = INVALID_HANDLE_VALUE;
	sectorData = NULL;
	sectorsize = 0ul;
}

CDiskWriter::~CDiskWriter() {
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

bool CDiskWriter::FileExists(const std::wstring sFilename) {
	FILE *fp = _wfopen(sFilename.c_str(), L"rb");
	if (fp != 0) {
		fclose(fp);

		return true;
	}

	return false;
}

bool CDiskWriter::WriteImageToDisk(const std::wstring sFilename, int iDeviceID, int iVolumeID) {
	if (!this->FileExists(sFilename)) {
		throw new std::exception("File not found");
	}

	status = STATUS_WRITING;

	hVolume = getHandleOnVolume(iVolumeID, GENERIC_WRITE);
	if (hVolume == INVALID_HANDLE_VALUE)
	{
		status = STATUS_IDLE;
		return false;
	}
	if (!getLockOnVolume(hVolume))
	{
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		return false;
	}
	if (!unmountVolume(hVolume))
	{
		removeLockOnVolume(hVolume);
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		return false;
	}
	hFile = getHandleOnFile(sFilename.c_str(), GENERIC_READ);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		removeLockOnVolume(hVolume);
		CloseHandle(hVolume);
		status = STATUS_IDLE;
		hVolume = INVALID_HANDLE_VALUE;
		return false;
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
		return false;
	}

	unsigned long long i, availablesectors, numsectors;

	availablesectors = getNumberOfSectors(hRawDisk, &sectorsize);
	numsectors = getFileSizeInSectors(hFile, sectorsize);
	if (numsectors > availablesectors)
	{
		CUIHelper::critical(L"Not enough available space!");
		return false;
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
			return false;
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
			return false;
		}
		delete[] sectorData;
		sectorData = NULL;

		ProgressBar.setValue(i);

		::Sleep(1);
	}
	removeLockOnVolume(hVolume);
	CloseHandle(hRawDisk);
	CloseHandle(hFile);
	CloseHandle(hVolume);
	hRawDisk = INVALID_HANDLE_VALUE;
	hFile = INVALID_HANDLE_VALUE;
	hVolume = INVALID_HANDLE_VALUE;

	return true;
}
