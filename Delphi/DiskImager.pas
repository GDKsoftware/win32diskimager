unit DiskImager;

interface

  function DiskImager_HasError(): Boolean; stdcall; external 'win32diskimager.dll' name 'DiskImager_HasError';

  function DiskImager_GetError(): PWideChar; stdcall; external 'win32diskimager.dll' name 'DiskImager_GetError';

  function DiskImager_GetAvailableDrives(): PWideChar; stdcall; external 'win32diskimager.dll' name 'DiskImager_GetAvailableDrives';

  function DiskImager_WriteToDisk(const DriveLetter: WideChar; const Filename: PWideChar): Boolean; stdcall; external 'win32diskimager.dll' name 'DiskImager_WriteToDisk';

  procedure DiskImager_Fini(); stdcall; external 'win32diskimager.dll' name 'DiskImager_Fini';

implementation

end.
