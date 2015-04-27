#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32DISKIMAGERDLL_EXPORTS
#define WIN32DISKIMAGERDLL_API __declspec(dllexport)
#else
#define WIN32DISKIMAGERDLL_API __declspec(dllimport)
#endif

WIN32DISKIMAGERDLL_API bool __stdcall DiskImager_HasError();

WIN32DISKIMAGERDLL_API wchar_t * __stdcall DiskImager_GetError();

WIN32DISKIMAGERDLL_API wchar_t * __stdcall DiskImager_GetAvailableDrives();

WIN32DISKIMAGERDLL_API bool __stdcall DiskImager_WriteToDisk(const wchar_t driveletter, const wchar_t *filename);

WIN32DISKIMAGERDLL_API void __stdcall DiskImager_Fini();

#ifdef __cplusplus
}
#endif
