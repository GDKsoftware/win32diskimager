
#include "ui_helper.h"

#include <windows.h>
#include <iostream>
#include <string>


CUIHelper helperinstance;

std::wstring CUIHelper::popErrorMessage() {
	if (errormessages.size() != 0) {
		auto msg = errormessages[0];
		errormessages.erase(errormessages.begin());
		return msg;
	}

	return nullptr;
}

void CUIHelper::pushErrorMessage(std::wstring message) {
	errormessages.push_back(message);
}

bool CUIHelper::hasErrors() {
	return (errormessages.size() != 0);
}

CUIHelper *CUIHelper::GetInstance() {
	return &helperinstance;
}

void CUIHelper::critical(const wchar_t *message) {
#ifdef _CONSOLE
	std::wcerr << message;
#else
	helperinstance.pushErrorMessage(message);
#endif
}

std::wstring string2wstring(std::string s) {
	std::wstring returnstr;

	size_t convertedChars = 0;
	size_t strlength = s.length();
	unsigned int iNewLen = strlength * sizeof(wchar_t);
	wchar_t *newstr = static_cast<wchar_t *>(malloc(iNewLen + 4));
	convertedChars = mbstowcs(newstr, s.c_str(), strlength);
	returnstr = newstr;
	free(newstr);

	return returnstr;
}

void CUIHelper::critical(const char *message) {
#ifdef _CONSOLE
	std::cerr << message;
#else
	helperinstance.pushErrorMessage(string2wstring(message));
#endif
}

void CUIHelper::criticalWithCurrentError(const wchar_t *title, const wchar_t *message) {
	wchar_t *errormessage = NULL;
	::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(), 0,
		(LPWSTR)&errormessage, 0, NULL);

#ifdef _CONSOLE
	std::wcerr << title << L" - " << message << std::endl
		<< L"Error " << std::to_wstring((int)GetLastError()).c_str() << L": " << errormessage;
#else
	std::wstring s = title;
	s.append(L" - ");
	s.append(message);
	s.append(L"\n");
	s.append(L"Error ");
	s.append(std::to_wstring((int)GetLastError()));
	s.append(L": ");
	s.append(errormessage);

	helperinstance.pushErrorMessage(message);
#endif

	delete errormessage;
}

void CUIHelper::criticalWithCurrentError(const wchar_t *title, const std::wstring message) {
#ifdef _CONSOLE
	criticalWithCurrentError(title, message.c_str());
#endif
}
