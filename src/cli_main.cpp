
#include "deviceinfo.h"
#include "diskwriter.h"
#include "ui_helper.h"

#include <iostream>
#include <locale>

// http://stackoverflow.com/questions/13871617/winmain-and-main-in-c-extended

extern int wmain(int, wchar_t**);

#include <windows.h>    // GetCommandLine, CommandLineToArgvW, LocalFree

int main()
{
	struct Args
	{
		int n;
		wchar_t** p;

		~Args() { if (p != 0) { ::LocalFree(p); } }
		Args() : p(::CommandLineToArgvW(::GetCommandLine(), &n)) {}
	};

	Args    args;

	if (args.p == 0)
	{
		return EXIT_FAILURE;
	}
	return wmain(args.n, args.p);
}


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
			bool found = false;
			for (auto p : v)
			{
				if (p.first == driveletter[0]) {
					disk = p.second;
					volume = p.first - 'A';
					found = true;
				}
			}

			if (found) {
				writer.WriteImageToDisk(filename, disk, volume);
			}
			else
			{
				CUIHelper::critical(L"Selected disk not supported");

				return 1;
			}
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
