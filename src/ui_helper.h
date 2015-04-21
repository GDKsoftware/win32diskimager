#ifndef UI_HELPER_H
#define UI_HELPER_H

#ifndef _CONSOLE
	#include <QtWidgets>
	#include <QString>

	#define UIString QString
#else
	#include <string>

	#define UIString std::wstring
#endif

class CUIHelper {
public:
	static void critical(const wchar_t *message);
	static void criticalWithCurrentError(const wchar_t *title, const wchar_t *message);
	static void criticalWithCurrentError(const wchar_t *title, const std::wstring message);
};

#endif //UI_HELPER_H
