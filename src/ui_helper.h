#ifndef UI_HELPER_H
#define UI_HELPER_H

#ifdef _CONSOLE
	#include <string>

	#define UIString std::wstring
#else
	#ifdef _WINDLL
		#include <string>

		#define UIString std::wstring
	#else
		#include <QtWidgets>
		#include <QString>

		#define UIString QString
	#endif
#endif

#include <vector>

class CUIHelper {
protected:
	std::vector<std::wstring> errormessages;
public:
	std::wstring popErrorMessage();
	void pushErrorMessage(std::wstring message);
	bool hasErrors();

	static CUIHelper *GetInstance();

	static void critical(const wchar_t *message);
	static void critical(const char *message);
	static void criticalWithCurrentError(const wchar_t *title, const wchar_t *message);
	static void criticalWithCurrentError(const wchar_t *title, const std::wstring message);
};

#endif //UI_HELPER_H
