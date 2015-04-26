
#include <windows.h>
#include <string>

class CProgressBar {
protected:
	INT64 Pos, Max;
public:
	void setRange(INT64 iPos, INT64 iMax);
	void setValue(INT64 iPos);
};

class CDiskWriter {
protected:
	enum Status { STATUS_IDLE = 0, STATUS_READING, STATUS_WRITING, STATUS_EXIT, STATUS_CANCELED };

	HANDLE hVolume;
	HANDLE hFile;
	HANDLE hRawDisk;
	char *sectorData;
	unsigned long long sectorsize;

	bool FileExists(const std::wstring sFilename);
public:
	int status;
	CProgressBar ProgressBar;

	CDiskWriter();
	virtual ~CDiskWriter();

	bool WriteImageToDisk(const std::wstring sFilename, int iDeviceID, int iVolumeID);
};
