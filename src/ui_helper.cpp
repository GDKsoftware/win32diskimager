
#include "ui_helper.h"

#include <windows.h>
#include <iostream>
#include <string>

void CUIHelper::critical(const wchar_t *message) {
#ifdef _CONSOLE
	std::wcerr << message;
#endif
}

void CUIHelper::criticalWithCurrentError(const wchar_t *title, const wchar_t *message) {
#ifdef _CONSOLE
	wchar_t *errormessage = NULL;
	::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0,
		(LPWSTR)&errormessage, 0, NULL);

	std::wcerr << title << L" - " << message << std::endl
		<< L"Error " << std::to_wstring((int)GetLastError()).c_str() << L": " << errormessage;
#endif
}

void CUIHelper::criticalWithCurrentError(const wchar_t *title, const std::wstring message) {
#ifdef _CONSOLE
	criticalWithCurrentError(title, message.c_str());
#endif
}
